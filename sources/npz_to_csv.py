from xml.dom.expatbuilder import FragmentBuilder
import numpy as np


FRAME_PREFIX = "C:\\Users\Michael\\repos\\data\\set1\\raw\\frame_"
IN_TYPE = ".npz"
OUT_TYPE = ".csv"
NUM_FILES = 5693

def main():
    for i in range(NUM_FILES):
        frame_file = np.load(FRAME_PREFIX + str(i) + IN_TYPE)
        frame_data = frame_file.files
        temp = []
        labs = []
        for elem in frame_data:
            labs.append(elem)
            temp.append(frame_file[elem])
            mat =  np.stack(temp).T
        np.savetxt("../data/frame_" + str(i) + ".csv", mat, delimiter=",",newline="\n")
        frame_file.close()
        if i % 100 == 0:
            print(i)
    return 0

if __name__ == '__main__':
    main()