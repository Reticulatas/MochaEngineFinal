/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include "IRegisterable.h"
#include "MochaStringImpl.h"

/* For registration purposes only
	Abstracts std::string so that it can be exposed and rendered to the editor
	Only for use in between ___BEGIN/END_REGISTERED___ blocks
	When doing operations, use Str() to get string and use that
*/

class MochaString
{
public:
	MochaString(void);
	MochaString(const std::string& s);
	MochaString(const char* buf);
	MochaString(const MochaString& s);
	~MochaString();

	MochaString& operator=(const MochaString& s);

	void FromStr(const std::string s);
	std::string Str(void) const;
	const char* CStr(void) const;
	unsigned	Length(void) const;
	char GetCharAt(unsigned i);
	
private:
	MochaStringImpl impl;

	___SERIALIZE_SAVE_BEGIN___
		std::string str = this->Str();
		unsigned length = impl.Length();
		unsigned capacity = impl.capacity;
		___DEFSER(str, 1)
		___DEFSER(length, 1)
		___DEFSER(capacity, 2)
	___SERIALIZE_END___
	___SERIALIZE_LOAD_BEGIN___
		std::string str;
		unsigned length,capacity;
		___DEFSER(str, 1)
		___DEFSER(length, 1)
		impl.Set(str.c_str(), length);
		___DEFSER(capacity, 2)
	___SERIALIZE_END___
	___SERIALIZE_SAVE_LOAD___
};
___SERIALIZE_CLASS(MochaString, 2);

void MochaString_ScriptConstructor(void* memory);
void MochaString_ScriptDestructor(void* memory);

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
