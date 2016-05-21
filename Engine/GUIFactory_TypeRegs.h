/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "GUIFactory_Variables.h"
#include <sstream>
#include "TypeRegs_ConversionHelper.h"

//----

#define GVMspace TypeMVC::GUIView::GUIViewModel

#define REGTYPE(type) class GVM_##type : public GVMspace \
{\
public:\
	std::string Encode(const void* data);\
	void* Decode(std::string data);\
	GVM_##type(std::string str);\
};\
	GVM_##type::GVM_##type(std::string str) { Type = str; dataSize = sizeof(type); }


#define GENERICREG(type) std::string GVM_##type::Encode(const void* data) { \
	const type* var = reinterpret_cast<const type*>(data); \
	return Converter::Convert_##type(*var); } \
void* GVM_##type::Decode(std::string data) { \
	type * v = new type((type)Converter::Convert_##type(data)); \
	return v; }

#define ___END___ }
#define ___BEGIN_ENCODE_REG___(type) std::string GVM_##type::Encode(const void* data) { \
	const type* var = reinterpret_cast<const type*>(data);
#define ___BEGIN_DECODE_REG___(type) void* GVM_##type::Decode(std::string data) {

#define INSERTREG(type) GVMspace::GUIViewModels.insert(GVMspace::GUIViewModelsElement(#type, new GVM_##type(#type)));\

//CONVERTER'S NOTE: WHEN HANDLING THE DATA AT THE APPLICATION POINT IN THE PIPELINE REMEMBER THAT:
// ENCODE IS A NO-PROBLEM CONVERSION FROM DATA -> STRING AND HAS NO RAMIFICATIONS
// DECODE CONVERTS FROM STRING -> DATA AND REQUIRES MANUAL MEMORY MANAGEMENT, MAKING IT UNSUITABLE FOR FRAME-BY-FRAME USE

#define GENERICCONVERTERREG(type)\
	___BEGIN_ENCODE_REG___(type)\
		return Converter::Convert_##type(*var);\
	___END___;\
	___BEGIN_DECODE_REG___(type)\
		type* r = new type(Converter::Convert_##type(data));\
		if (!Converter::WasLastConversionSuccessful()) { return nullptr; }\
		return r;\
	___END___

//----
//To add new types:
//1. REGTYPE(class/var name)
//2. Provide implementations for Encode/Decode or use defaults (GENERICREG)
//3. Add INSERTREG(class/var name) to MakeListOfRegs
//4. Make a CONVERTER over at ConversionHelper
//----


REGTYPE(float);
GENERICREG(float);
REGTYPE(int);
GENERICREG(int);
REGTYPE(unsigned);
GENERICREG(unsigned);
REGTYPE(short);
GENERICREG(short);
REGTYPE(char);
GENERICREG(char);
REGTYPE(double);
GENERICREG(double);
REGTYPE(bool);
GENERICREG(bool);
REGTYPE(Vector2)
GENERICCONVERTERREG(Vector2);
REGTYPE(Vector4)
GENERICCONVERTERREG(Vector4);
REGTYPE(Color)
GENERICCONVERTERREG(Color);
REGTYPE(Vector3);
GENERICCONVERTERREG(Vector3);
REGTYPE(Point3);
GENERICCONVERTERREG(Point3);
REGTYPE(MochaString);		//typedef'ed std::string
GENERICCONVERTERREG(MochaString);
//REGTYPE(XMVECTOR);
//GENERICCONVERTERREG(XMVECTOR);

/* Post compile time dictionary manager */
/* Singleton */
void MakeListOfRegs()
{
	static bool generated = false;
	if (generated) { return; }

	INSERTREG(float);
	INSERTREG(int);
	INSERTREG(unsigned);
	INSERTREG(char);
	INSERTREG(double);
	INSERTREG(short);
	INSERTREG(Vector2);
	INSERTREG(Vector3);
	INSERTREG(Vector4);
  INSERTREG(Point3);
	INSERTREG(MochaString);
	INSERTREG(bool);
	//INSERTREG(XMVECTOR);
	INSERTREG(Color);
	
	generated = true;
}