import sys
import os, os.path
from time import gmtime, strftime
from difflib import SequenceMatcher
from shutil import copy2

# ------------HISTORY DEFINITION -----------
#regHistory = open('SerializationData\RegistrationHistory.txt','r+')
regComps = dict()
outfileH = open('Serializations_TEMP.h','w')
outfileC = open('Serializations_TEMP.cpp','w')
#outfileDoc = open('Docs/Binding_GeneratedDocs.h','w')
exitCode = 0
verbose = True


#def DefineHistory():
 #   if regHistory.readline().strip() != ">COMPS": # begin comps registered history
  #      print 'Corrupt History, ignoring...'
   # else:
    #    # read in list of component already registered
     #   while True:
      #      compName = regHistory.readline().strip()
      #  if compName == '>ENDCOMPS':
       #         break
        #    regVarsForComp = []
         #   # get list of all variables
          #  while True:
           #     varDef = regHistory.readline().strip()
            #    if varDef == 'End' + compName:
             #       break
              #  regVarsForComp.append(varDef)
       #     # final registration in dictionary
        #    regComps[compName] = regVarsForComp

class BindType:
    GLOBALMETHOD = 1
    CLASSMETHOD = 2
    SINGLETONMETHOD = 3
    STATICCLASSMETHOD = 4
    PROPERTY = 5
    TYPE = 6
    ENUMDEF = 7

class DefaultData(object):
    def __init__(self, typename, varname, default):
        self.typename = typename
        self.varname = varname
        self.default = default
class BindInfo(object):
    def __init__(self):
        # global
        self.line = ''
        # method or type
        self.structname = ''
        self.classname = ''
        self.filename = ''
        self.basename = 'NONE'
        self.raw = ''
        self.bindtype = BindType.GLOBALMETHOD
        self.rawfunc = ''
        self.rawParams = []
        self.rawReturn = ''
        self.isConstMethod = False
        self.docs = []
        # property
        self.varname = ''
        self.vartype = ''
        # enum
        self.enumname = ''
        self.enumvalues = []
        self.enumtruename = ''

# ----------END HISTORY DEFINITION --------


# is file f a component?
def IsComponent(f):
    for line in open(f).readlines():
        if '___BEGIN_REGISTERED___' in line:
            return True
    return False

# Return True is nothing has changed between the comp lists and False if there are new comps
def CheckHistoryWithCurrent_Comps(listComps):
    for comp in listComps:
        if not comp in regComps:
            return False
    return True;

# dictionary in form: { 'compKey' , [ DefaultDataList ] }
varDefaults = dict()
varSerDefaults = dict()

def WriteNewHistory():
    return

class ScopeInfo(object):
    def __init__(self):
        self.classname = 'NONE'
        self.basename = 'NONE'

scriptBinds = []
typesBound = []
enumTypesBound = []
lastComment_lines = []

def DoScriptBinds_ParseHeader_FullClean(raw):
    raw = raw.replace("static ", " ")
    raw = raw.replace("*","@")
    raw = raw.replace("char","uint8")
    raw = raw.replace("unsigned int", "uint")
    raw = raw.replace("unsigned", "uint")
    raw = raw.replace("std::string","string")
    return raw

def DoScriptBinds_ParseHeader_CleanReturnRaw(raw):
    raw = DoScriptBinds_ParseHeader_FullClean(raw)
    raw = raw.replace("float&","float")
    raw = raw.replace("int&","int")
    raw = raw.replace("&","@")
    raw = raw.replace("const","")
    # replace enum definitions with their global equivalent
    for enumTrueBind in enumTypesBound:
        raw = raw.replace(enumTrueBind.enumtruename,enumTrueBind.enumname)
    raw = raw.strip()
    return raw

def DoScriptBinds_ParseHeader_CleanRaw(raw, fullASClean):
    raw = raw.replace("static ", " ")
    if fullASClean:
        raw = DoScriptBinds_ParseHeader_FullClean(raw)
        raw = raw.replace("&"," &in ")
        raw = raw.replace("const","")
        # replace enum definitions with their global equivalent
        for enumTrueBind in enumTypesBound:
            raw = raw.replace(enumTrueBind.enumtruename,enumTrueBind.enumname)
    raw = raw.strip()
    return raw

# helper for DoScriptBinds_ParseHeader
def DoScriptBinds_ParseHeader_ParseMethod(line, linenum, scopeStack, filename, currentScopeIsSingleton):
    # parse bound function tokens
    firstparen = line.find('(')
    lastparen = line.rfind(')')
    raw = line[firstparen+1:lastparen]
    rawFunc = raw[:raw.find('(')]
    rawTokens = rawFunc.split(' ')
    rawFunc = rawTokens[len(rawTokens)-1]
    # const function check
    constCheckTemp = raw[raw.rfind(')'):]
    isConst = False
    if 'const' in constCheckTemp:
         isConst = True
    # token out the params
    rawParamsRaw = raw[raw.find('(')+1:raw.rfind(')')]
    rawParams = rawParamsRaw.split(',')
    # strip out var names from param list while keeping modifiers
    for i in range(len(rawParams)):
        param = rawParams[i].strip()
        param = (param.strip())[:param.rfind(' ')+1].strip()
        if param == '':
            param = 'void'
        rawParams[i] = param
    isStatic = False
    if 'static ' in raw:
        # this is a static function, bind it globally
        isStatic = True   
    # cleanup the raw removing trivials
    raw = DoScriptBinds_ParseHeader_CleanRaw(raw, False)
    # token out return val from the fixed up raw
    rawTokens = raw.split(' ')
    if rawTokens[0] == 'const':
        rawReturn = rawTokens[0] + ' ' + rawTokens[1]
    else:
        rawReturn = rawTokens[0]
    # cleanup the raw (full clean)
    # split the raw, we don't want cleanup to happen on the returnval
    rawTempParams = raw[raw.find('('):]
    raw = raw[:raw.find('(')]
    rawTempParams = DoScriptBinds_ParseHeader_CleanRaw(rawTempParams, True)
    raw = DoScriptBinds_ParseHeader_CleanReturnRaw(raw)
    raw = raw + rawTempParams
    # generate bind information
    bi = BindInfo()
    bi.line = line
    bi.structname = '___BINDINFO___' + str(linenum)
    bi.classname = scopeStack[0].classname
    bi.baseclass = scopeStack[0].basename
    bi.filename = filename
    bi.raw = raw
    bi.rawfunc = rawFunc
    bi.isConstMethod = isConst
    if len(lastComment_lines) == 0:
        print '\tWARNING: NO DOCUMENTATION DECLARED FOR:\n\t' + raw + '\n\tIN FILE: ' + filename + '.h('+str(linenum)+')'
    for comment in lastComment_lines:
        bi.docs.append(comment)
    for p in rawParams:
        bi.rawParams.append(p)
    bi.rawReturn = rawReturn
    # if this isn't global and is currently in a scope
    if scopeStack[0].classname != 'NONE':
        if currentScopeIsSingleton:
            bi.bindtype = BindType.SINGLETONMETHOD
        elif not isStatic:
            bi.bindtype = BindType.CLASSMETHOD
        else:
            bi.bindtype = BindType.STATICCLASSMETHOD
    scriptBinds.append(bi)
    if verbose:
        print '\t\t'+line.strip()
    return

# helper for DoScriptBinds_ParseHeader
def DoScriptBinds_ParseHeader_ParseProperty(line, linenum, scopeStack, filename, currentScopeIsSingleton):
    bi = BindInfo()
    bi.bindtype = BindType.PROPERTY
    firstparen = line.find('(')
    lastparen = line.rfind(')')
    raw = line[firstparen+1:lastparen]
    tokens = raw.strip().split(' ')
    bi.vartype = DoScriptBinds_ParseHeader_CleanRaw(tokens[0], True)
    bi.varname = tokens[1]
    bi.classname = scopeStack[0].classname
    bi.basename = scopeStack[0].basename
    bi.structname = '___BINDINFO___' + str(linenum)
    bi.filename = filename
    bi.line = line
    if ';' in bi.varname:
        bi.varname = bi.varname[:-1]
    scriptBinds.append(bi)
    if verbose:
        print '\t\t'+line.strip()
    return

# helper for DoScriptBinds
def DoScriptBinds_ParseHeader(curPath, filename):
    fil = open(curPath)
    line = 'CURRENT LINE'
    scopeStack = [ScopeInfo()]
    linenum = 0
    currentScopeIsSingleton = False
    previousLine = ""
    for line in fil:
        line = line.strip()
        linenum += 1
        if '#define' in line:
            continue
        if 'BINDENUMSTART' in line:
            # specific definition for enum
            enumline = fil.next()
            toks = enumline.strip().split(' ')
            linenum += 1
            bi = BindInfo()
            bi.bindtype = BindType.ENUMDEF
            bi.enumname = toks[len(toks)-1]
            bi.filename = filename
            bi.classname = scopeStack[0].classname
            bi.line = line
            if verbose:
                print '\t\tBOUND ENUM: '+bi.enumname
            # add the current scope tree to the enum definition
            if len(scopeStack) > 1:
                for scope in scopeStack:
                    if scope.classname != 'NONE':
                        bi.enumtruename = scope.classname + '::' + bi.enumname
            # keep registering values until we hit the end
            while True:
                enumline = fil.next()
                if 'BINDENUMEND' in enumline:
                    break
                linenum += 1
                if '{' in enumline or '}' in enumline:
                    continue
                enumvalue = enumline.strip().replace(",","")
                if '=' in enumvalue:
                    enumvalue = enumvalue[:enumvalue.find('=')].strip()
                bi.enumvalues.append(enumvalue)
            scriptBinds.append(bi)
            enumTypesBound.append(bi)
        elif 'BINDSTART' in line:
            # get next line and parse the class name out of it
            classline = fil.next()
            toks = classline.split()
            linenum += 1
            if len(toks) < 2:
                print '\tERROR: Line after BINDSTART in '+curPath+' is not class declaration'
            if 'class' not in toks and 'struct' not in toks:
                print '\tERROR: Line after BINDSTART in '+curPath+' is not class declaration'
            for i in range(len(toks)):
                if 'class' in toks[i] or 'struct' in toks[i]:
                    if i + 1 == len(toks):
                        print '\tERROR: Line after BINDSTART in '+curPath+' is not class declaration'
                    scopeStack.insert(0, ScopeInfo())
                    scopeStack[0].classname = toks[i+1]
                    print '\tSTART Bind Scope: '+toks[i+1]
                if 'public' in toks[i]:
                    print '\t\t' + scopeStack[0].classname + ' has base: ' + toks[i+1]
                    scopeStack[0].basename = toks[i+1]
                if 'ISingleton' in toks[i]:
                    currentScopeIsSingleton = True
        elif 'BINDEND' in line:
            # end the current scope
            deadScope = scopeStack.pop(0)
            currentScopeIsSingleton = False
            print '\tEND Bind Scope: '+deadScope.classname
        elif 'BIND(' in line:
            # skip empty lines or comment lines
            if line.strip() == '' or '//' in line.strip()[:5] or '/*' in line.strip()[:5]:
                continue
            # is this a function?
            if '(' in line[line.find('(')+1:]:
                DoScriptBinds_ParseHeader_ParseMethod(line, linenum, scopeStack, filename, currentScopeIsSingleton)
            else:
                DoScriptBinds_ParseHeader_ParseProperty(line, linenum, scopeStack, filename, currentScopeIsSingleton)
        if '/*!' in line:
            # begin searching for the comment for this script bind
            lastComment_lines.append(line.strip() + '\n')
            while not '*/' in line:
                line = fil.next()
                linenum += 1
                lastComment_lines.append(line.strip() + '\n')
        else:
            del lastComment_lines[:] #clear previous comment
        previousLine = line;
    fil.close()
    return

def DoScriptBinds_WriteParams(bind, doClean):
    for i in range(len(bind.rawParams)):
        if doClean:
            bind.rawParams[i] = DoScriptBinds_ParseHeader_CleanRaw(bind.rawParams[i], True)
        outfileC.write(bind.rawParams[i])
        if i != len(bind.rawParams) - 1:
                outfileC.write(', ')
        
    return

boundClasses = []

# search through all headers and generate the structs for their bindinfo's
def DoScriptBinds():
    print '\n...Generating Script Bindings...'
    curPath = '.'
    for f in os.listdir(curPath):
        fullpath = os.path.join(curPath, f)
        if os.path.isfile(fullpath):
            filename, fileext = os.path.splitext(f)
            if fileext == '.h':
                DoScriptBinds_ParseHeader(fullpath, filename)
    print '...Done'
    print '...Writing Script Binds to Serializations...'
    outfileC.write('\n')
    classesincluded = []
    for bind in scriptBinds:
        if bind.filename not in classesincluded:
            if bind.filename != 'NONE' and bind.filename != '':
                classesincluded.append(bind.filename)
    for cl in classesincluded:
        outfileC.write('#include "'+cl+'.h"\n')
    outfileC.write('void GenerateBindInfo(asIScriptEngine* engine) {\nint r;\n')
    # write type binds first to avoid dependency issues
    for bind in scriptBinds:
        # write the static bind info initializer if there needs to be one
        if bind.bindtype != BindType.ENUMDEF:
            if bind.classname != 'NONE':
                outfileC.write('static '+bind.classname+'::'+bind.structname+' '+bind.classname+bind.structname+'inst;\n')
            else:
                # global scope
                outfileC.write('static '+bind.structname+' '+bind.classname+bind.structname+'inst;\n')
        # if we haven't bound the class this is contained in, do so
        if bind.classname.strip() != '':
            if bind.classname.strip() not in typesBound:
                outfileC.write('___AS_REG('+bind.classname.strip()+');\n')
                if bind.basename != 'NONE':
                    outfileC.write('___AS_REG_HIERARCHY('+bind.basename.strip()+','+bind.classname.strip()+');\n')
                typesBound.append(bind.classname.strip())
    for bind in scriptBinds:
        # write all the regs
        if bind.bindtype == BindType.GLOBALMETHOD:
            outfileC.write('r = engine->RegisterGlobalFunction("'+bind.raw+'",asFUNCTIONPR('+bind.rawfunc+',(')
            paramsStr = ''
            for param in bind.rawParams:
                paramsStr = paramsStr + param + ','
            paramsStr = paramsStr[:-1]
            outfileC.write(paramsStr+'),'+bind.rawReturn+'), asCALL_CDECL); assert(r >= 0);\n')
        if bind.bindtype == BindType.SINGLETONMETHOD:
            outfileC.write('{ '+bind.classname+'* inst = '+bind.classname+'::getInstance();\n')
            outfileC.write('r = engine->RegisterGlobalFunction("'+bind.raw+'", asMETHOD('+bind.classname+', '+bind.rawfunc+'), asCALL_THISCALL_ASGLOBAL, inst); assert(r>=0); }\n')
        if bind.bindtype == BindType.CLASSMETHOD:
            outfileC.write('_REG_OBJ_METHOD('+bind.classname+',"'+bind.raw+'",'+bind.rawfunc+', (')
            DoScriptBinds_WriteParams(bind, False)
            outfileC.write(')')
            # hack: inject the const keyword into the macro by putting it with the params
            if bind.isConstMethod:
                outfileC.write(' const')
            outfileC.write(', ' + bind.rawReturn + ');\n')
        if bind.bindtype == BindType.STATICCLASSMETHOD:
            # returns for statics should be cleaned
            bind.rawReturn = DoScriptBinds_ParseHeader_CleanReturnRaw(bind.rawReturn)
            outfileC.write('_REG_STATIC_METHOD('+bind.rawReturn+','+bind.classname+','+bind.rawfunc+',"')
            DoScriptBinds_WriteParams(bind, True)
            outfileC.write('");\n')
        if bind.bindtype == BindType.PROPERTY:
            outfileC.write('___AS_REG_PROP('+bind.classname+','+bind.vartype+','+bind.varname+');\n')
        if bind.bindtype == BindType.ENUMDEF:            
            outfileC.write('_REG_ENUMDEF("'+bind.enumname+'");\n')
            for i in range(len(bind.enumvalues)):
                outfileC.write('_REG_ENUMVALUE("'+bind.enumname+'","'+bind.enumvalues[i]+'",'+str(i)+');\n') 
    # write hierarchies
    for compKey in varDefaults:        
        outfileC.write('___AS_REG_HIERARCHY(Component,'+compKey+');\n')
    outfileC.write('}\n')

    # write documentation
    print 'Writing documentation...'
    currentWritingClass = ''
    for bind in scriptBinds:
        if bind.classname != currentWritingClass and bind.classname != '':
            #if currentWritingClass != '':
                # end last scope
              #  outfileDoc.write('};\n')
            #start new scope
            #outfileDoc.write('class ' + bind.classname + '\n{\npublic:\n')
            currentWritingClass = bind.classname
        if len(bind.docs) != 0:
            for comment in bind.docs:
                if '/*!' in comment:
                    #outfileDoc.write('/*!\n')
                    comment = comment.replace("/*!","")
             #   outfileDoc.write(comment)
            #outfileDoc.write(bind.line.strip()[5:-2] + ';\n')
    print '...Done'
    return

# Register single variable as existing in class compName
def Register(line, compName):
    trimline = line.strip()
    if trimline.startswith('//') or trimline.startswith('/*'):
        return
    linesanscomments, comments = trimline.split(';')
    typename, varname = linesanscomments.split(' ',1)
    if "::" in typename:
        print '\tWARNING: No namespaces allowed in registrations'
        print '\t         Make ONLY SERIALIZED to allow any type'
        print '\t         ' + typename + ' ' + varname + ' was not registered'
        return
    if "const" in typename or "static" in typename:
        print '\tWARNING: No modifiers in definition (ex: const, static, etc)'
        print '\t         ' + typename + ' ' + varname + ' was not registered'
        return
    if varname.endswith(';'):
        varname = varname[:-1]
    # account for special cases
    # templates in this case
    if 'AssetHandle' in typename:
        typename = typename.replace("<","")
        typename = typename.replace(">","")
    print('     +R ' + typename + ' ' + varname)
    # insert this into the current list of defaults for this class
    if not compName in varDefaults:
        varDefaults[compName] = []
    varDefaults[compName].append(DefaultData(typename, varname, '0'))
    return

# Register single variable as being only serialized - not exposed to typeregs
def RegisterSerialized(line, compName):
    trimline = line.strip()
    if trimline.startswith('//') or trimline.startswith('/*'):
        return
    linesanscomments, comments = trimline.split(';')
    typename, varname = linesanscomments.split(' ',1)
    if "const" in typename or "static" in typename:
        print '\tWARNING: No modifiers in definition (ex: const, static, etc)'
        print '\t         ' + typename + ' ' + varname + ' was not registered'
        return
    if varname.endswith(';'):
        varname = varname[:-1]
    print('     +S ' + typename + ' ' + varname)
    # insert this into the current list of serialized defaults for this class
    if not compName in varSerDefaults:
        varSerDefaults[compName] = []
    varSerDefaults[compName].append(DefaultData(typename, varname, '0'))
    return

# write the defaults array after the SetDefaults method has been registered
# this is how we go from name -> string and back using Python as a middle man
def MakeDefaultsArray(compName):
    outfileC.write('Serializer::DefaultData defaults_' + compName + '[] = {\n')
    lastCounter = 0
    # if there is no variable registered, make one so the compiler doesn't flip out
    if len(varDefaults[compName]) == 0:
        varDefaults[compName].append(DefaultData('NONE', 'NONE', '"NONE"'))
    # generate DefaultData c++ structs for each registered variable (from Register)
    for var in varDefaults[compName]:
        outfileC.write('{ "' + var.typename + '","' + var.varname + '", ' + var.default + '}')
        lastCounter += 1
        if lastCounter != len(varDefaults[compName]):
            outfileC.write(',\n')
    outfileC.write('};\n\n')
    return

# component registrations must have this header in order to compile correctly
def MakeCompIniterHeader(compName):
    outfileC.write('#include "' + compName.strip() + '.h"\n')
    outfileC.write('#ifndef NOSERIALIZE\nBOOST_CLASS_EXPORT(' + compName.strip()+');\n#endif\n');
    outfileH.write('#include "' + compName.strip() + '.h"\n')
    return

# write translation map from comp key string to defaults array size and address
def WriteDefaultStrMap():
    outfileC.write('\n//---Translation Map from CompKey string -> address of default array ---\n')
    outfileC.write('Serializer::KeyToDefs defaultStrMap[] = {\n')
    varCounter = 0
    for compKey in varDefaults:
        outfileC.write('{ "' + compKey + '", reinterpret_cast<Serializer::DefaultData*>(defaults_' + compKey + '), ')
        if len(varDefaults[compKey]) == 1 and (varDefaults[compKey])[0].varname == 'NONE':
            outfileC.write('0 }')
        else:
            outfileC.write(str(len(varDefaults[compKey])) + ' }')
        if varCounter < len(varDefaults):
            outfileC.write(',')
        outfileC.write('\n')
        varCounter = varCounter + 1
    outfileC.write('};unsigned defaultStrMapLen = ' + str(len(varDefaults)) + ';')

# write script bindings for each Component and their registered members
def WriteScriptRegistrations():
        outfileH.write('class asIScriptEngine;\nvoid RegisterASBinds(asIScriptEngine*);')
        outfileC.write('\nvoid GenerateBindInfo(asIScriptEngine*);\nvoid RegisterASBinds(asIScriptEngine* engine) {\n')
        outfileC.write('int r = 0;\n')
        for compKey in varDefaults:
                outfileC.write('___AS_REG('+compKey+');\n')
                typesBound.append(compKey)
                print compKey
                for var in varDefaults[compKey]:
                        if var.varname != 'NONE':
                                outfileC.write('___AS_REG_PROP('+compKey+','+var.typename+','+var.varname+');\n')
        outfileC.write('GenerateBindInfo(engine);\n}\n')


# write function that computes the offsets of every variable in every class
def WriteVariableOffsets():
    outfileC.write('\n\nvoid ComputeVariableOffsets(void) {\n\n')
    outfileC.write('static bool computed = false; if (computed) { return; }\n')
    for compKey in varDefaults:
        outfileC.write(compKey + '* inst_'+compKey+' = (' + compKey + '*)malloc(sizeof(' + compKey + '));\n')
        varCounter = 0
        for var in varDefaults[compKey]:
            if var.varname != 'NONE':
                outfileC.write('defaults_'+compKey+'['+str(varCounter)+'].memOffset = offsetof('+compKey+', '+var.varname+');\n')
                outfileC.write('defaults_'+compKey+'['+str(varCounter)+'].memSize = sizeof('+var.typename+');\n')
            varCounter+=1
        outfileC.write('free(inst_'+compKey+');\n')
    outfileC.write('\ncomputed = true; }\n')
    # header definition
    outfileH.write('\nvoid ComputeVariableOffsets(void);\n\n')

def WriteBOOSTSerializeRoutines():
    outfileH.write('#include "GameObject.h"\n#include "Component.h"\n')
    outfileH.write('\nnamespace boost { namespace serialization {\n')
    for compKey in varDefaults:
        outfileH.write('template <class Archive>\n')
        outfileH.write('void serialize(Archive &ar, '+compKey+'& t, const BOOST_PFTO unsigned int) {\n')
        outfileH.write('ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP_NONINTRUSIVE(Component, t);\n')
        for var in varDefaults[compKey]:
            if var.varname != 'NONE':
                outfileH.write('ar & BOOST_SERIALIZATION_NVP(t.' + var.varname + ');\n')
        if compKey in varSerDefaults:
            for var in varSerDefaults[compKey]:
                if var.varname != 'NONE':
                    outfileH.write('ar & BOOST_SERIALIZATION_NVP(t.' + var.varname + ');\n')
        outfileH.write('}\n')
    outfileH.write('} }\n');
    return

# Write all compiled data to the files Serializations.cpp/.h
def WriteSerializationFiles():
    # Initial headers
    outfileH.write('////AUTO-GENERATED HEADER FROM GENERATEREGISTRATIONS SCRIPT////\n')
    outfileH.write('#pragma once\n\n')
    outfileH.write('#include "SerializationIncludes.h"\n')
    outfileH.write('#include "Serializations_Persistent.h"\n')
    outfileC.write('////AUTO-GENERATED CODE FROM GENERATEREGISTRATIONS SCRIPT////\n')
    outfileC.write('#include "stdafx.h"\n')
    outfileC.write('#include "Serializations.h"\n')
    outfileC.write('#include "TypeRegs_ConversionHelper.h"\n')
    outfileC.write('#include <stdlib.h>\n')
    outfileC.write('#include "Binding_Macros.h"\n')

    #defaults and instantiation functions
    for compKey in varDefaults:
        MakeCompIniterHeader(compKey)
        MakeDefaultsArray(compKey)

    WriteDefaultStrMap()
    # WriteBOOSTSerializeRoutines()
    WriteVariableOffsets()
    WriteScriptRegistrations()
    DoScriptBinds()
    return

# Do single component parsing
def Parse(f, compName):
    print '>' + f + '...'
    fil = open(f, 'r')
    cl = 'CURRENTLINE'
    while cl:
        cl = fil.readline()
        curLine = cl.strip()
        # registrations for scripting/typeRegs
        if curLine == '___BEGIN_REGISTERED___':
            while curLine != '___END_REGISTERED___':
                cl = fil.readline()
                curLine = cl.strip()
                if curLine != '' and curLine != '___END_REGISTERED___':
                    Register(curLine, compName)
            if not compName in varDefaults:
                varDefaults[compName] = [DefaultData('NONE', 'NONE', '"NONE"')]
        # registrations for serialization ONLY
        if curLine == '___BEGIN_ONLY_SERIALIZED___':
            while not '___END_ONLY_SERIALIZED' in curLine:
                cl = fil.readline()
                curLine = cl.strip()
                if curLine != '' and not '___END_ONLY_SERIALIZED' in curLine:
                    RegisterSerialized(curLine, compName)
            if not compName in varSerDefaults:
                varSerDefaults[compName] = [DefaultData('NONE', 'NONE', '"NONE"')]
    return

# diffs the temp files to see if anything changed, if so, writes the temp to the production copy
def DoDiffCheckAndFinalWrite():
    global exitCode
    outfileH.close()
    outfileC.close()
    #outfileDoc.close()
    outfileTH = open('Serializations_TEMP.h','r')
    outfileTC = open('Serializations_TEMP.cpp','r')
    resultH, resultC = -1,-1
    if os.path.isfile('Serializations.h') and os.path.isfile('Serializations.cpp'):
        finalH = open('Serializations.h','r')
        finalC = open('Serializations.cpp','r')
        resultC_SM = SequenceMatcher(None, outfileTC.read(), finalC.read())
        resultH_SM = SequenceMatcher(None, outfileTH.read(), finalH.read())
        resultC = resultC_SM.ratio()
        resultH = resultH_SM.ratio()
        finalH.close()
        finalC.close()
    if resultH == 1 and resultC == 1:
        # nothing changed, just exit
        print '>>> No changes since last build\n>>> Generation skipped'
        return;
    outfileTH.close()
    outfileTC.close()
    # kill the originals
    if resultH != 1:
        if os.path.isfile('Serializations.h'):
            os.remove('Serializations.h')
        copy2('Serializations_TEMP.h', 'Serializations.h')
        print '     Applied header'
    if resultC != 1:
        if os.path.isfile('Serializations.cpp'):
            os.remove('Serializations.cpp')
        copy2('Serializations_TEMP.cpp', 'Serializations.cpp')
        print '     Applied cpp'
    print '>>> New generation applied'
    exitCode = 1
    return

# --------------------------------------------------------
# --------------------------------------------------------

def main():
    global exitCode
    print '--------------------------------'
    print ' ...Generating Registrations... '
    curPath = '.'

    if verbose:
        print '\tVERBOSE OUTPUT ENABLED\n'

    for f in os.listdir(curPath):
        fullpath = os.path.join(curPath, f)
        if os.path.isfile(fullpath):
            if f[0] == 'C':
                compName, fileext = os.path.splitext(f)
                if fileext == '.h':
                    if IsComponent(f) == True:
                        Parse(f, compName)

    WriteSerializationFiles()


    DoDiffCheckAndFinalWrite()

    # remove temp files
    if os.path.isfile('Serializations_TEMP.cpp'):
        os.remove('Serializations_TEMP.h')
        os.remove('Serializations_TEMP.cpp')

    print '             Done!             '
    print '-------------------------------'
    #sys.exit(exitCode)

if __name__ == "__main__":
    main()

