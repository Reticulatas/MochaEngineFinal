/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include <sstream>
 

//Forward Declarations----
#include "MathIncludes.h"
using namespace std;
#include "AssetManager.h"
#include "MochaString.h"
#include "MochaMath.h"
//------------------------

//internal
#define ___DECLARE_CONVERTER_ENCODE(type)\
	static std::string	Convert_##type(const type& v);
#define ___DECLARE_CONVERTER_DECODE(type)\
	static type	Convert_##type(std::string s);
#define ___DECLARE_CONVERTER_ENCODE_N(type, namespac)\
	static std::string	Convert_##type(const namespac::type& v);
#define ___DECLARE_CONVERTER_DECODE_N(type, namespac)\
	static namespac::type	Convert_##type(std::string s);
//external
#define ___DECLARE_CONVERTER(type)\
	___DECLARE_CONVERTER_DECODE(type);\
	___DECLARE_CONVERTER_ENCODE(type);
#define ___DECLARE_CONVERTER_N(type, namespac)\
	___DECLARE_CONVERTER_DECODE_N(type, namespac);\
	___DECLARE_CONVERTER_ENCODE_N(type, namespac);

class Converter
{
	static bool mLastConversionSuccessful;
public:
	//Format: Convert_[TypeName]
	//Add forward declaration up top^^^

	___DECLARE_CONVERTER(int);
	___DECLARE_CONVERTER(short);
	___DECLARE_CONVERTER(double);
	___DECLARE_CONVERTER(char);
	___DECLARE_CONVERTER(bool);
	___DECLARE_CONVERTER(float);
	___DECLARE_CONVERTER(unsigned);

	//NON GENERICS---------------------------------

	___DECLARE_CONVERTER(Vector2);
	___DECLARE_CONVERTER(Vector3);
	___DECLARE_CONVERTER(MochaString);		
	___DECLARE_CONVERTER(Vector4);
	___DECLARE_CONVERTER(Point3);
	___DECLARE_CONVERTER(Color);

	___DECLARE_CONVERTER_N(Range, Math);

	/* Get whether last method call was a success
		this is due to returning copies of arbitrary items
		no error handling could be packaged into return data*/
	static bool WasLastConversionSuccessful(void);
};

/**
// ADD NEW SERIALIZATION TO SERIALIZATIONS_PERSISTENT
 */

