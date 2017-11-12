
#include <iostream>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <iostream>
#include "joint_vo_sf.h"

using namespace std;

// -------------------------------------------------------------------------------
//								Instructions:
// Set the flag "save_results" to true if you want to save the estimated scene
// flow and the static/dynamic segmentation
// -------------------------------------------------------------------------------

void load_cam(const char* path, float* intrinsic) {
    /*
    Camera:
    Binary ".cam" file format, storing the intrinsic and extrinsic camera
    matrices in 64-bit floating-point precision, assuming little-endian order.

    offset  size    contents
    0       4       Tag: "PIEH" in ASCII, which in little endian happens to be
    the float 202021.25 (just a sanity check that floats are
    represented correctly)
    4       9*8     Intrinsic 3x3 matrix in row-major order.
    76      12*8    Extrinsic 3x4 matrix in row-major order.
    */
    fstream myFile(path, ios::in | ios::out | ios::binary);
    myFile.seekg(0);
    char tag[5]; // supposed to be PIEH
    int width, height;

    myFile.read((char*)tag, sizeof(char) * 5);
    cout << tag << endl;

    double* intrinsic_ = new double[9];

    myFile.seekg(4);
    myFile.read((char*)intrinsic_, sizeof(double)*9);
    for (int i = 0; i < 9; i++)
        intrinsic[i] = intrinsic_[i];
    delete[] intrinsic_;
}

cv::Mat load_dpt(const char* path)
{
    /*
    Depth:
    Binary ".dpt" file format, storing a 32 bit floating-point depth map.
    Floats are stored in little-endian order.

    offset  size    contents
    0       4       Tag: "PIEH" in ASCII, which in little endian happens to be
    the float 202021.25 (just a sanity check that floats are
    represented correctly)
    4       4       Width as an integer
    8       4       Height as an integer
    12      width*height*4      Data: Depth image, in row-major order.
    */
    fstream myFile(path, ios::in | ios::out | ios::binary);
    myFile.seekg(0);
    char tag[5]; // supposed to be PIEH
    int width, height;

    myFile.read((char*)tag, sizeof(char) * 5);
    cout << tag << endl;

    myFile.seekg(4);
    myFile.read((char*)&width, sizeof(int));
    cout << width << endl;

    myFile.seekg(4 + sizeof(int));
    myFile.read((char*)&height, sizeof(int));
    cout << height << endl;

    //float* data = new float[width*height * 4];
    float* data = new float[width*height];

    myFile.seekg(4 + sizeof(int) * 2);
    myFile.read((char*)data, sizeof(float)*width*height);

    float min_ = 10000.0;
    float max_ = 0.0;
    for(int i = 0; i < width*height; i++){
        float val = data[i];
        if (val < min_) min_ = val;
        if (val > max_) max_ = val;
    }

    //cout << "=================================" << endl;
    cout << "max and min: " << max_ << "," << min_ << endl;

    // this part later we could change it with
    // depth_raw1.convertTo(depth0, CV_16UC1, 1000.0); then we don't need to change maxZ and minZ
    // so conclusion:
    // either change the maxZ/minZ by fit the scale to 10.0 or
    // but for another data/example with different ranges.
    // It is safer to follow 1000.0 scale and their default range 50 to 100 is still valid.
    // currently give default 100/50 range and constZ as 10.0 , actual depth value is 500 to 1000.0
    // for the case of alley if we fit the
    cv::Mat depth(height, width, CV_32F);

    // best way is to find max/min of raw data and scale to max 1000 but depending on the input,
    // there are resolution trade off. This need open question.
    // but now let's make the work

    float scale = 1000.0;
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            depth.at<float>(row, col) = (float) scale* data[row*width + col];
            //depth.at<short>(row, col) = (ushort)constZ*(float)((minZ + (maxZ - minZ)*(data[row*width + col] - min_) / (max_ - min_) )) ;
        }
    }

    delete[] data;

    return depth;
}

int main (int argc, char* argv[]) {
    const bool save_results = true;
    const unsigned int res_factor = 2;

    if (argc < 7) {
        cout << "not sufficient input files to execute..." << endl;
        exit(0);
    }

    string intrinsic_file(argv[1]);
    string image0_file(argv[2]);
    string image1_file(argv[3]);
    string depth0_file(argv[4]);
    string depth1_file(argv[5]);
    string result_dir(argv[6]);

    cout << "load camera intrinsic file: " << intrinsic_file << endl;
    cout << "load image0 file: " << image0_file << endl;
    cout << "load image1 file: " << image1_file << endl;
    cout << "load depth0 file: " << depth0_file << endl;
    cout << "load depth1 file: " << depth1_file << endl;
    cout << "the results will be in directory: " << result_dir << endl;

    cv::Mat img0 = cv::imread(image0_file);
    cv::Mat img1 = cv::imread(image1_file);
    cv::Mat depth0 = load_dpt(depth0_file.c_str());
    cv::Mat depth1 = load_dpt(depth1_file.c_str());

    // load the camera intrinsics 3*3
    float* K = new float[9];

    load_cam(intrinsic_file.c_str(), K);

    cout << "The current camera intrinsic is: "
         << K[0] << " " << K[1] << " " << K[2] << endl
         << K[3] << " " << K[4] << " " << K[5] << endl
         << K[6] << " " << K[7] << " " << K[8] << endl;

    VO_SF cf(res_factor, K, 436, 1024);

    //Load images and create both pyramids
    cf.loadCVImagesPair(img0, img1, depth0, depth1, res_factor);

    //Create the 3D Scene
    cf.initializeSceneCamera();

    //Run the algorithm
    cf.run_VO_SF(true);

    cf.createImagesOfSegmentations();


    //Update the 3D scene
    cf.updateSceneCamera(false);

    //Save results?
    if (save_results)
        cf.saveFlowAndSegmToFile(result_dir);

    //mrpt::system::os::getch();
    return 0;
}

