import os
import sys
from difflib import SequenceMatcher
import shutil

def main():
    failed = False
    for file in os.listdir(sys.argv[1]):
        if file.endswith("." + sys.argv[2]):
            # print " checking file: " + file
            filename = file
            file = sys.argv[1] + file
            metafile = file + ".dmeta"
            if os.path.isfile(metafile):
                #file exists - check diff
                hosts0 = open(file,'r')
                hosts1 = open(metafile,'r')
                df = SequenceMatcher(None,hosts0.read(),hosts1.read())
                if df.ratio() != 1:
                    failed = True # changed file found, keep going
                    #print filename
                hosts0.close()
                hosts1.close()
            else:
                failed = True # new file found, keep going and update the rest
            # make the meta file
            shutil.copyfile(file, metafile)
            continue
    if failed == False:
        sys.exit(0) #no changes found
    sys.exit(1)

if __name__ == "__main__":
    main()
