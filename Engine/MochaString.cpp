/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "MathUtilities.h"

MochaString::MochaString( void )
{
}

MochaString::MochaString( const std::string& s ) : impl(s)
{
}

MochaString::MochaString( const char* buf ) : impl(buf)
{
}

MochaString::MochaString( const MochaString& s ) : impl(s.Str())
{
}

std::string MochaString::Str( void ) const
{
	return impl.Str();
}

unsigned MochaString::Length( void ) const
{
	return impl.Length();
}

MochaString::~MochaString()
{
}

MochaString& MochaString::operator=( const MochaString& s )
{
	FromStr(s.Str());
	return *this;
}

void MochaString::FromStr( const std::string s )
{
	impl.Set(s.c_str(),s.length());
}

char MochaString::GetCharAt( unsigned i )
{
	return impl.GetCharAt(i);
}

const char* MochaString::CStr( void ) const
{
	return impl.cBuf;
}

void MochaString_ScriptConstructor( void* memory )
{
	new(memory) MochaString();
}

void MochaString_ScriptDestructor( void* memory )
{
	((MochaString*)memory)->~MochaString();
}

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
