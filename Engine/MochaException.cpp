#include "stdafx.h"
#include "MochaException.h"
#include "Log.h"

MochaException::MochaException( std::string _msg, bool doLog ) : std::exception()
{
	msg = _msg;	
	if (doLog)
		Log(msg, LogEntry::Exception);
}

MochaException::MochaException( const MochaException& o )
{
	msg = o.msg;
}

MochaException& MochaException::operator=( const MochaException& o ) throw()
{
	msg = o.msg;
	return *this;
}

MochaException::~MochaException()
{
}

const char* MochaException::what() const throw()
{
	return msg.c_str();
}
