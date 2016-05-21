import os,sys
import shutil
import glob
import msvcrt as m

def gen_files(dir, pattern):
   for dirname, subdirs, files in os.walk(dir):
      for f in files:
         if f.endswith(pattern):
            yield os.path.join(dirname, f)

outdir = 'C:/Users/Nicholas/Desktop/teammochagame/subjectREDACTED/'

for dllname in glob.glob("output/*.exe"):
    print("Copying " + dllname + "...")
    shutil.copy2(dllname, outdir)
for dllname in glob.glob("output/*.lib"):
    print("Copying " + dllname + "...")
    shutil.copy2(dllname, outdir)
for dllname in glob.glob("output/*.pdb"):
    print("Copying " + dllname + "...")
    shutil.copy2(dllname, outdir)
for dllname in glob.glob("output/*.dll"):
    print("Copying " + dllname + "...")
    shutil.copy2(dllname, outdir)
for dllname in glob.glob("output/*.ini"):
    print("Copying " + dllname + "...")
    shutil.copy2(dllname, outdir)
for dllname in glob.glob("output/*.csv"):
    print("Copying " + dllname + "...")
    shutil.copy2(dllname, outdir)

print "Removing Meta Files..."
for f in gen_files(outdir, '.meta'):
    print " Removed: " + f 
    os.remove(f)

print("\n\nDone.")
m.getch();
