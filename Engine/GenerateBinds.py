import sys
import os, os.path
from time import gmtime, strftime
from difflib import SequenceMatcher
from shutil import copy2

outfile = open('bindingfunctors_TEMP.h','w')
tobind = []

def DoDiffCheckAndFinalWrite():
    global outfile
    outfile.close()
    outfileTH = open('bindingfunctors_TEMP.h','r')
    if os.path.isfile('bindingfunctors.h'):
        finalH = open('bindingfunctors.h','r')
        resultH_SM = SequenceMatcher(None, outfileTH.read(), finalH.read())
        resultH = resultH_SM.ratio()
        finalH.close()
    if resultH == 1:
        # nothing changed, just exit
        print '>>> No changes since last build\n>>> Generation skipped'
        return;
    outfileTH.close()
    # kill the originals
    if resultH != 1:
        if os.path.isfile('bindingfunctors.h'):
            os.remove('bindingfunctors.h')
        copy2('bindingfunctors_TEMP.h', 'bindingfunctors.h')
        print '     Applied new binding function'
    print '>>> New generation applied'
    return

def Parse(f):
    print '\t>' + f + '...'
    fil = open(f, 'r')
    cl = 'CURRENTLINE'
    while cl:
        cl = fil.readline()
        curLine = cl.strip()
        if "bindstart" in curLine:
            junk, curLine = curLine.split('(',1)
            curLine, junk = curLine.split(')',1)
            print '\t\tbound: ' + curLine
            name,ext = f.split('.',1)
            outfile.write('#include "' + name + '.h"\n')
            tobind.append(curLine)
    return

def main():
    print ' ...Generating binds... '
    curPath = '.'

    outfile.write('/* AUTO GENERATED FILE FROM GENERATEBINDS.EXE */\n')
    outfile.write('#include <boost/python.hpp>\n')


    for f in os.listdir(curPath):
        fullpath = os.path.join(curPath, f)
        if os.path.isfile(fullpath):
            compName, fileext = os.path.splitext(f)
            if fileext == '.h' and compName != 'def' and compName != 'def_bind':
                Parse(f)

    outfile.write('\nvoid CreateBinds() {\n')
    for b in tobind:
        outfile.write(b + '::bindor_BIND' + b + '();\n')

    outfile.write('}\n')

    
    DoDiffCheckAndFinalWrite()

    # remove temp files
    if os.path.isfile('bindingfunctors_TEMP.h'):
        os.remove('bindingfunctors_TEMP.h')

    print '...Done!'

if __name__ == "__main__":
    main()
