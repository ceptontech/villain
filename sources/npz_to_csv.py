import numpy as np


FRAME_PREFIX = "C:\\Users\Michael\\repos\\data\\set1\\reco\\frame_"
IN_TYPE = ".npz"
OUT_TYPE = ".csv"
NUM_FILES = 5693
def main():
    for i in range(NUM_FILES):
        frame_file = np.load(FRAME_PREFIX + str(i) + IN_TYPE)
        frame_data = frame_file.files
        temp = []
        labs = ["x","y","z","r","t"]
        # print(frame_data)
        mat = frame_file[frame_data[0]]
        # print(mat.shape)
        for elem in frame_data[1:]:
            labs.append(elem)
            col = np.atleast_2d(frame_file[elem]).T
            mat = np.hstack((mat, col))
        # print(mat.shape)
        np.savetxt("../data/frame_" + str(i) + ".csv", mat, header=str(labs), delimiter=",",newline="\n")
        if i % 100 == 0:
            print(f"[INFO] frame_{i}.csv translated")
    return 0

if __name__ == '__main__':
    main()