import numpy as np

BKG_FILE_LOCATION = "C:\\Users\\\Michael\\repos\\raylib-viewer\\npz\\bkg_0.npz"

def main():
    bkgf = np.load(BKG_FILE_LOCATION)
    bkgh = bkgf.files
    temp = []
    labs = []
    for elem in bkgh:
        labs.append(elem)
        temp.append(bkgf[elem])
        mat =  np.stack(temp).T
    np.savetxt("../data/bkg.csv", mat, header=str(labs), delimiter=",",newline="\n")

if __name__ == "__main__":
    main()