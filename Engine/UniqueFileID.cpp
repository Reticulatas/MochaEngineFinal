/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


// Generates a unique id based on a filename or filepath
// Uses Windows NTFS data to collect ID
#include "stdafx.h"
 
#include <windows.h>

UniqueFileID::UniqueFileID( std::string _filename )
{
	filename = _filename;

	//get ntfs id and volume for this filename
	HANDLE hfile = CreateFile(std::wstring(_filename.begin(),_filename.end()).c_str(),
		0, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile == INVALID_HANDLE_VALUE)
		throw FileIDException(filename + " could not be found!");

	BY_HANDLE_FILE_INFORMATION FileInfo;
	GetFileInformationByHandle(hfile, &FileInfo);
	indexLow	= FileInfo.nFileIndexLow;
	indexHigh	= FileInfo.nFileIndexHigh;
	volume		= FileInfo.dwVolumeSerialNumber;

	CloseHandle(hfile);
}

UniqueFileID::UniqueFileID( void )
{
	filename = "UNASSIGNED";
	indexHigh = indexLow = volume = (DWORD)0;
}

bool UniqueFileID::operator==( const UniqueFileID& rhs ) const
{
	if (rhs.indexHigh != indexHigh)
		return false;
	if (rhs.indexLow != indexLow)
		return false;
	if (rhs.volume != volume)
		return false;
	return true;
}

void UniqueFileID::operator=( const UniqueFileID& rhs )
{
	indexLow = rhs.indexLow;
	indexHigh = rhs.indexHigh;
	volume = rhs.volume;
	filename = rhs.filename;
}

bool UniqueFileID::operator<(const UniqueFileID& rhs) const
{
	return (indexHigh | indexLow) < (rhs.indexHigh | rhs.indexLow);
}

bool UniqueFileID::Valid() const
{
	return !(indexLow == 0 && indexHigh == 0 && volume == 0);
}
