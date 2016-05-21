#pragma once
#include <exception>
#include <string>

class MochaException : public std::exception
{
public:
	MochaException(std::string _msg, bool doLog = true);
	MochaException(const MochaException& o);
	MochaException& operator=(const MochaException& o) throw();
	virtual ~MochaException();
	virtual const char* what() const throw(); 
private:
	std::string msg;
};