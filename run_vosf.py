
import os, sys

from os.path import join, abspath

# The executable, change if you have a different build folder
exe = './Joint-VO-SF/build/VO-SF-SINTEL'
# set your SINTEL training data-set
sintel_data_dir = '../SINTEL/training'

scenes_test = {
    'ambush_6': 20,
    'bandage_1': 50,
    'market_6': 50,
    'shaman_2': 50,
    'temple_2': 50
}

# scenes = {
#     'alley_1': [10, 20, 30, 40],
#     'alley_2': [10, 20, 30, 40],
#     "ambush_2":[10, 20],
# 	"ambush_4":[10, 20, 30],
#     "ambush_5":[10, 20, 30, 40 ],
#     "ambush_7":[10, 20, 30, 40],
#     "bamboo_1":[10, 20, 30, 40],
#     "bamboo_2":[10, 20, 30, 40],
#     "bandage_2": [10, 20, 30, 40],
# 	"cave_2": [10, 20, 30, 40],
#     "cave_4": [10, 20, 30, 40],
#     "market_2": [10, 20, 30, 40],
#     "market_5": [10, 20, 30, 40],
# 	"mountain_1": [10, 20, 30],
#     "shaman_2": [10, 20, 30, 40],
#     "shaman_3": [10, 20, 30, 40],
# 	"sleeping_1": [10, 20, 30, 40],
#     "sleeping_2": [10, 20, 30, 40],
#     "temple_2": [10, 20, 30, 40],
#     "temple_3": [10, 20, 30, 40]
# }

def check_directory(filename):
    target_dir = os.path.dirname(filename)
    if not os.path.isdir(target_dir):
        os.makedirs(target_dir)

for key in scenes_test:
    frames = range(scenes_test[key])
    for idx in frames:
        frame0 = 'frame_{:04d}'.format(idx)
        frame1 = 'frame_{:04d}'.format(idx+1)

        img0_path = abspath(join(sintel_data_dir, 'clean', key, frame0 + '.png'))
        img1_path = abspath(join(sintel_data_dir, 'clean', key, frame1 + '.png'))
        depth0_path = abspath(join(sintel_data_dir, 'depth', key, frame0 + '.dpt'))
        depth1_path = abspath(join(sintel_data_dir, 'depth', key, frame1 + '.dpt'))
        intrinsic_path = abspath(join(sintel_data_dir, 'camdata_left', key, frame0 + '.cam'))
        results_path = abspath(join('result', key, frame0, 'output'))
        check_directory(results_path)

        if not os.path.isfile(img0_path):
            print('file does not exist: {:}'.format(img0_path))
        if not os.path.isfile(img1_path):
            print('file does not exist: {:}'.format(img1_path))
        if not os.path.isfile(depth0_path):
            print('file does not exist: {:}'.format(depth0_path))
        if not os.path.isfile(depth1_path):
            print('file does not exist: {:}'.format(depth1_path))
        if not os.path.isfile(intrinsic_path):
            print('file does not exist: {:}'.format(intrinsic_path))

        cmd = '{:} {:} {:} {:} {:} {:} {:}'.format(exe, intrinsic_path, img0_path, img1_path, depth0_path, depth1_path, results_path)
        print(cmd)
        os.system(cmd)
