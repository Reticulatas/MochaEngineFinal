#pragma once

#include <string>
#include <Windows.h>
#include "boost/filesystem.hpp"

const unsigned FileMark = 'moca';
const unsigned MarkAnimation = 'anim';
const unsigned MarkMesh = 'mesh';
const unsigned MarkSkel = 'skel';
const unsigned MarkType = 'type';
const std::string BeanExt = ".bean";
const std::string MetaExt = ".meta";
const std::string FbxExt = ".fbx";
const std::string AnimExt = ".anim";
const std::string FModGUIDFile = "GUIDs.txt";
const std::string VenteExt = ".vente";

typedef boost::filesystem::path BoostPath;

// Gets Time Stamp of specified file
inline FILETIME GetTimeStamp(BoostPath filePath)
{
  std::string s = filePath.string();
  std::wstring ws(s.begin(), s.end());

  HANDLE file = CreateFile(ws.c_str(),
                           0,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           0,
                           OPEN_EXISTING,
                           0,
                           0);

  FILETIME createTime, accessTime, writeTime;

  if (!GetFileTime(file, &createTime, &accessTime, &writeTime))
    std::cout << "Failed to get time stamp for " << filePath.filename().string() << std::endl;

  return writeTime;
}

inline FILETIME GetMetaTimeStamp(BoostPath metaPath)
{
  FILETIME metaTime;

  // Get time stamp from meta file
  std::ifstream fileRead(metaPath.string());
  if (fileRead.is_open())
  {    
    fileRead >> metaTime.dwLowDateTime;
    fileRead >> metaTime.dwHighDateTime;
    fileRead.close();
  }
  else
    std::cout << "Failed to open " << metaPath.filename().string() << std::endl;

  return metaTime;
}

// Writes Time Stamp to specified file
inline void WriteTimeStamp(BoostPath filePath, FILETIME time)
{
  std::ofstream file(filePath.string());

  if (file.is_open())
  {
    file << time.dwLowDateTime << std::endl;
    file << time.dwHighDateTime << std::endl;
    file.close();
  }
  else
    std::cout << "Failed to open " << filePath.filename().string() << std::endl;
}

inline void WriteAnimStamp(BoostPath filePath)
{
  // Writes animation name to .bean.meta so model file knows which animation it contains
  std::string modelName = filePath.filename().string();
  modelName = "\\" + modelName.substr(0, modelName.find_first_of("_")) + BeanExt + MetaExt;
  BoostPath beanFile = filePath.parent_path();
  beanFile += modelName;

  std::ofstream file(beanFile.string(), std::ios_base::app);

  if (file.is_open())
  {
    file << filePath.filename().string() << std::endl;
    file.close();
  }
  else
    std::cout << "Failed to open " << beanFile.filename().string() << std::endl;
}