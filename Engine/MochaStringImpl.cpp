/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "MathUtilities.h"
#include "MochaStringImpl.h"

MochaStringImpl::MochaStringImpl( void )
{
	cBuf = 0;
	length = 0;
	capacity = 0;
}

MochaStringImpl::MochaStringImpl( const std::string& s )
{
	cBuf = 0;
	Set(s.c_str(), s.length());
}

MochaStringImpl::MochaStringImpl( const char* buf )
{
	cBuf = 0;
	Set(buf, strlen(buf));
}

std::string MochaStringImpl::Str( void ) const
{
	if (cBuf == 0)
		return std::string("");
	return std::string(cBuf, length);
}

unsigned MochaStringImpl::Length( void ) const
{
	return length;
}

MochaStringImpl::~MochaStringImpl()
{
	if (cBuf)
		delete[] cBuf;
}

void MochaStringImpl::Set( const char* buf, unsigned size )
{
	if (cBuf)
		delete[] cBuf;
	unsigned long nextPowerTwo = NextPowerOfTwo(size);
	if (nextPowerTwo == size)
		nextPowerTwo += 1;
	cBuf = new char[nextPowerTwo]();
	memset(cBuf, 0x00, nextPowerTwo);
	memcpy(cBuf, buf, size);
	length = size;
	capacity = nextPowerTwo;
}

MochaStringImpl& MochaStringImpl::operator=( const MochaStringImpl& s )
{
	Set(s.cBuf, s.Length());
	return *this;
}

void MochaStringImpl::FromStr( const std::string s )
{
	Set(s.c_str(),s.length());
}

char MochaStringImpl::GetCharAt( unsigned i )
{
	if (i >= length)
		return 0;
	return cBuf[i];
}

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change

