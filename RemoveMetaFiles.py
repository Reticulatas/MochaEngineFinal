import os,sys
import shutil
import glob
import msvcrt as m

def gen_files(dir, pattern):
   for dirname, subdirs, files in os.walk(dir):
      for f in files:
         if f.endswith(pattern):
            yield os.path.join(dirname, f)

print "Removing Meta Files..."
for f in gen_files('Assets/', '.meta'):
    print " Removed: " + f 
    os.remove(f)
