/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "TypeRegs_ConversionHelper.h"

 
 
 

//----NON-GENERICS--------------------------------------------

//REMEBER THE INCLUDE THE CORRECT HEADERS FOR TYPES
//ALL CONVERSIONS FROM STRING MUST ALLOW FOR "0" INPUT
//REMEMBER! CLEAR THE MWASSUCCESSFUL FLAG BEFORE EVERY CONVERSION

//Color
Color Converter::Convert_Color( std::string s )
{
	mLastConversionSuccessful = true;
	if (s == "0")
		return Color();
	std::vector<std::string> tokens = split(s, ',');
	if (tokens.size() != 4) { mLastConversionSuccessful = false; return Color(); }
	float r = (float)atof(trim(tokens[0]).c_str());
	float g = (float)atof(trim(tokens[1]).c_str());
	float b = (float)atof(trim(tokens[2]).c_str());
	float a = (float)atof(trim(tokens[3]).c_str());
	Color v(r,g,b,a);
	return v;
}

//5,4,3,2
std::string Converter::Convert_Color( const Color& v )
{
	mLastConversionSuccessful = true;
	return std::string(ToString(v.r) + ", " + ToString(v.g) + ", " + ToString(v.b) + ", ") + ToString(v.a);
}
//Vector2
Vector2 Converter::Convert_Vector2(std::string s)
{
	mLastConversionSuccessful = true;
	if (s == "0")
    return Vector2(0, 0);
	std::vector<std::string> tokens = split(s, ',');
  if (tokens.size() != 2) { mLastConversionSuccessful = false; return Vector2(0, 0); }
	float x = (float)atof(trim(tokens[0]).c_str());
	float y = (float)atof(trim(tokens[1]).c_str());
  Vector2 v(x, y);
	return v;
}

//5,4,3,2
std::string Converter::Convert_Vector2(const Vector2& v)
{
	mLastConversionSuccessful = true;
	return std::string(ToString(v.x) + ", " + ToString(v.y));
}
//Vector4
Vector4 Converter::Convert_Vector4(std::string s)
{
	mLastConversionSuccessful = true;
	if (s == "0")
    return Vector4(0, 0, 0, 0);
	std::vector<std::string> tokens = split(s, ',');
  if (tokens.size() != 4) { mLastConversionSuccessful = false; return Vector4(0, 0, 0, 0); }
	float x = (float)atof(trim(tokens[0]).c_str());
	float y = (float)atof(trim(tokens[1]).c_str());
	float z = (float)atof(trim(tokens[2]).c_str());
	float w = (float)atof(trim(tokens[3]).c_str());
  Vector4 v(x, y, z, w);
	return v;
}

//5,4,3,2
std::string Converter::Convert_Vector4(const Vector4& v)
{
	mLastConversionSuccessful = true;
	return std::string(ToString(v.x) + ", " + ToString(v.y) + ", " + ToString(v.z) + ", ") + ToString(v.w);
}
//Vector3
Vector3 Converter::Convert_Vector3(std::string s)
{
	mLastConversionSuccessful = true;
	if (s == "0")
    return Vector3(0, 0, 0);
	std::vector<std::string> tokens = split(s, ',');
  if (tokens.size() != 3) { mLastConversionSuccessful = false; return Vector3(0, 0, 0); }
	float x = (float)atof(trim(tokens[0]).c_str());
	float y = (float)atof(trim(tokens[1]).c_str());
	float z = (float)atof(trim(tokens[2]).c_str());
  Vector3 v(x, y, z);
	return v;
}

//5,4,3
std::string Converter::Convert_Vector3(const Vector3& v)
{
	mLastConversionSuccessful = true;
	return std::string(ToString(v.x) + ", " + ToString(v.y) + ", " + ToString(v.z));
}

//5,4,3
std::string Converter::Convert_Point3( const Point3& v )
{
  mLastConversionSuccessful = true;
  return std::string(ToString(v.x) + ", " + ToString(v.y) + ", " + ToString(v.z));
}

//Vector4
Point3 Converter::Convert_Point3( std::string s )
{
  mLastConversionSuccessful = true;
  if (s == "0")
    return Point3(0,0,0);
  std::vector<std::string> tokens = split(s, ',');
  if (tokens.size() != 3) { mLastConversionSuccessful = false; return Point3(0,0,0); }
  float x = (float)atof(trim(tokens[0]).c_str());
  float y = (float)atof(trim(tokens[1]).c_str());
  float z = (float)atof(trim(tokens[2]).c_str());
  Point3 v(x,y,z);
  return v;
}
bool Converter::Convert_bool(std::string v)
{
	mLastConversionSuccessful = true;
	if (v == "0")
		return false;
	return true;
}
std::string Converter::Convert_bool(const bool& v)
{
	mLastConversionSuccessful = true;
	if (v == false)
		return "0";
	return "1";
}

MochaString Converter::Convert_MochaString(std::string v)
{
	mLastConversionSuccessful = true;
	return MochaString(v.c_str());
}
std::string Converter::Convert_MochaString(const MochaString& v)
{
	mLastConversionSuccessful = true;
	return v.Str();
}

Math::Range Converter::Convert_Range(std::string v)
{
	mLastConversionSuccessful = true;
	if (v == "0")
		return Math::Range();
	std::vector<std::string> tokens = split(v, ',');
	if (tokens.size() != 2) { mLastConversionSuccessful = false; return Math::Range(); }
	float start = (float)atof(trim(tokens[0]).c_str());
	float end = (float)atof(trim(tokens[1]).c_str());
	return Math::Range(start,end);

}
std::string Converter::Convert_Range(const Math::Range& v)
{
	mLastConversionSuccessful = true;
	return std::string(ToString(v.GetStart()) + ", " + ToString(v.GetEnd()));
}

bool Converter::mLastConversionSuccessful = true;
bool Converter::WasLastConversionSuccessful( void )
{
	return mLastConversionSuccessful;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//GENERICS--------------------------------------------------------------------------------
#define ___GENERICCONVERTER(type) 	std::string Converter::Convert_##type(const type& t) {\
	mLastConversionSuccessful = true; std::stringstream ss;\
	ss << t; std::string r = ss.str();\
	ss.clear();	return r; }\
	type Converter::Convert_##type(std::string str) {\
	mLastConversionSuccessful = true; type val;	std::stringstream ss(str);\
	ss >> val; if (ss.fail()) {	mLastConversionSuccessful = false; return 0;	}\
	return val; }

#define ___ASSETCONVERTER(type) \
	AssetHandle##type Converter::Convert_AssetHandle##type(std::string v)\
{\
	mLastConversionSuccessful = true;\
	unsigned id = atoi(v.c_str());\
	return AssetManager::getInstance()->GetAsset<type>(id);\
}\
	std::string Converter::Convert_AssetHandle##type(const AssetHandle##type& v)\
{\
	mLastConversionSuccessful = true;\
	return ToString(v.GetId());\
}

___GENERICCONVERTER(int);
___GENERICCONVERTER(short);
___GENERICCONVERTER(char);
___GENERICCONVERTER(double);
___GENERICCONVERTER(float);
___GENERICCONVERTER(unsigned);
