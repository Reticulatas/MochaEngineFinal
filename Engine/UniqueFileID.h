/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 
#include <windows.h>
#include <string>

class UniqueFileID
{
	DWORD indexLow;
	DWORD indexHigh;
	DWORD volume;
	std::string filename;
public:
	UniqueFileID(std::string _filename);
	UniqueFileID(void);

	bool operator==(const UniqueFileID& rhs) const;
	void operator=(const UniqueFileID& rhs);
	bool operator<(const UniqueFileID& rhs) const;

	std::string Filename() const { return filename; }
	bool Valid() const;
};

struct FileIDException : public std::exception
{
	std::string s;
	FileIDException(std::string ss) : s(ss) {}
	~FileIDException() throw () {} // Updated
	const char* what() const throw() { return s.c_str(); }
};