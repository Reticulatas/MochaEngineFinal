///////////////////////////////////////////////////////////////////////////////
//
//	Conversion.h
//	Converts homgeneous transform matrices between coordinate systems.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#ifndef FBXCONVERTER_H_
#define FBXCONVERTER_H_

#include "fbxsdk.h"

class Converter
{
public:	
	void Initialize(FbxScene* pScene);
	void ConvertMatrix( FbxAMatrix& sourceMatX);
	void ConvertMeshMatrix( FbxAMatrix& m);
	FbxMatrix ConversionMatrix;
	bool NeedToChangedWinding;
};

#endif