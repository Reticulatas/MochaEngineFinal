/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include "IRegisterable.h"

class MochaStringImpl
{
public:
	MochaStringImpl(void);
	MochaStringImpl(const std::string& s);
	MochaStringImpl(const char* buf);
	~MochaStringImpl();

	MochaStringImpl& operator=(const MochaStringImpl& s);

	void FromStr(const std::string s);
	std::string Str(void) const;
	unsigned	Length(void) const;
	void Set(const char* buf, unsigned size);
	char GetCharAt(unsigned i);
	
	char*		cBuf;
	unsigned	length;
	unsigned	capacity;
private:
};

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change

