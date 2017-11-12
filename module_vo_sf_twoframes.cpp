#include <iostream>
#include <string>
#include "joint_vo_sf.h"

#include <boost/python.hpp>
#include <numpy/ndarrayobject.h>

using namespace std;
using namespace boost::python;

// transform the vector to a numpy array
static object array_to_nparray(double* data, npy_intp size) {
    npy_intp shape[1] = { size }; // array size
    PyObject* obj = PyArray_New(&PyArray_Type, 1, shape, NPY_DOUBLE, // data type
                                NULL, data, // data pointer
                                0, NPY_ARRAY_CARRAY, // NPY_ARRAY_CARRAY_RO for readonly
                                NULL);
    handle<> array( obj );
    return object(array);
}

void load_measurements(VO_SF& sf, float* np_intensity, float* np_depth, const unsigned int res_factor) {

    int height = sf.height;
    int width = sf.width;

    for (unsigned int v=0; v < height; v++) {
        for (unsigned int u=0; u < width; u++) {
            // the second term needs to multiply the number of rows
            sf.intensity_wf(height-1-v, u) = np_intensity[0, 0];
        }
    }

    for (unsigned int v=0; v < height; v++) {
        for (unsigned int u=0; u < width; u++) {
            sf.depth_wf(height-1-v, u) = np_depth[0, 0];
        }
    }

    sf.createImagePyramid();
}

void vo_sf_twoframes(PyObject* py_intensity0, PyObject* py_intensity1, 
    PyObject* py_depth0, PyObject* py_depth1) {

    PyArrayObject* np_intensity0 = (PyArrayObject*) py_intensity0;
    PyArrayObject* np_intensity1 = (PyArrayObject*) py_intensity1;
    PyArrayObject* np_depth0 = (PyArrayObject*) py_depth0;
    PyArrayObject* np_depth1 = (PyArrayObject*) py_depth1;
    
    // float* intensity0 = (float*) PyArray_GETPTR1(np_intensity0, 0);
    // float* intensity1 = (float*) PyArray_GETPTR1(np_intensity1, 0);
    // float* depth0 = (float*) PyArray_GETPTR1(np_depth0, 0);
    // float* depth1 = (float*) PyArray_GETPTR1(np_depth1, 0);    

    // const unsigned int res_factor = 2;
	// VO_SF cf(res_factor);

    // // load the first frame and create the first frame image Pyramid;
    // load_measurements(cf, intensity0, depth0, res_factor);
    // // load the first frame and create the second frame image Pyramid;
    // load_measurements(cf, intensity1, depth1, res_factor);

	// //Run the algorithm
	// cf.run_VO_SF(false);
    // cf.createImagesOfSegmentations();

    // save the results
    // cf.saveFlowAndSegmToFile(dir);
}

BOOST_PYTHON_MODULE(pyFlow2Pose) {
    
    // numpy requires this
    import_array();

    def("vo_sf_twoframes", &vo_sf_twoframes);
}



