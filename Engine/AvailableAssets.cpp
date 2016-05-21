#include "stdafx.h"
#include "Log.h"
#include "AvailableAssets.h"
#include "AssetManager.h"
#include "boost/filesystem.hpp"
#include "tinyxml2/tinyxml2.h"

std::list<AvailableAsset*> availableAssets;
typedef std::list<AvailableAsset*>::iterator availableAssets_iter;
typedef std::map<std::string, std::string> ExtToTypeMap;
ExtToTypeMap extToTypeMap;
bool hasBuiltExtToTypeMap = false;


//-----HELPER----------------------//

AssetType AvailableAsset::GetTypeFromExtension(std::string ext)
{
	ExtToTypeMap::iterator iter = extToTypeMap.find(ext);
	if (iter == extToTypeMap.end())
		return "";
	return iter->second;
}

bool AvailableAsset::isExtensionOfType(std::string ext, AssetType type)
{
	if (AvailableAsset::GetTypeFromExtension(ext) == type)
		return true;
	return false;
}

//-------------------------------//

AvailableAsset::AvailableAsset( std::string _filename)
{
	filepath = _filename;
	name = boost::filesystem::path(_filename).filename().stem().string();
	filename = name + boost::filesystem::path(_filename).extension().string();
	fileID = UniqueFileID(_filename);
	type = AvailableAsset::GetTypeFromExtension(boost::filesystem::path(_filename).extension().string().substr(1));

	availableAssets.push_back(this);
}

AvailableAsset::~AvailableAsset()
{
	//remove if this exists
	availableAssets_iter iter = availableAssets.begin();
	for (; iter != availableAssets.end(); ++iter)
	{
		if (*iter == this)
		{
			availableAssets.remove(*iter);
			return;
		}
	}
}

AvailableAsset* AvailableAsset::GetAvailableAssetFromFileName( std::string filename_, bool createIfNotFound /* = true */ )
{
	if (filename_ == "UNASSIGNED")
	{
		Log("Asset has no filename");
		return 0;
	}
	AvailableAsset* a = 0;
	availableAssets_iter iter = availableAssets.begin();
	for (; iter != availableAssets.end(); ++iter)
	{
		if ((*iter)->Filename() == filename_)
			return *iter;
	}
	if (createIfNotFound)
	{
		a = new AvailableAsset(filename_);
	}

	return a;
}
AvailableAsset* AvailableAsset::GetAvailableAssetFromUFID( UniqueFileID id, bool createIfNotFound /* = true */ )
{
	if (id.Filename() == "UNASSIGNED")
	{
		Log("Asset has no filename");
		return 0;
	}
	availableAssets_iter iter = availableAssets.begin();
	for (; iter != availableAssets.end(); ++iter)
	{
		if ((*iter)->FileID() == id)
			return *iter;
	}
	AvailableAsset* av = 0;
	if (createIfNotFound)
	{
		av = new AvailableAsset(id.Filename());
	}
	return av;
}

AssetHandle AvailableAsset::GetAssetHandle( void )
{
	return AssetManager::getInstance()->GetAsset(filename);
}

std::vector<AvailableAsset*> AvailableAsset::GetAvailableAssetsFromType( AssetType type )
{
	std::vector<AvailableAsset*> assets;
	for (availableAssets_iter iter = availableAssets.begin(); iter !=availableAssets.end(); ++iter)
	{
		if ((*iter)->Type() == type)
			assets.push_back(*iter);
	}
	return assets;
}

std::vector<AvailableAsset*> AvailableAsset::GetAvailableAssetsFromExtension( std::string& ext )
{
	AssetType t = GetTypeFromExtension(ext);
	return GetAvailableAssetsFromType(t);
}

bool Task_AvailableAsset_Refresh::Tick(void)
{
	if (!hasBuiltExtToTypeMap)
	{
		//first time generation of asset filters
		tinyxml2::XMLDocument doc;
		doc.LoadFile("Assets/Config/AssetFileSettings.xml");
		tinyxml2::XMLNode* root = doc.FirstChild();
		tinyxml2::XMLElement* n = root->FirstChildElement("AssetType");
		while (n)
		{
			const char* type_name = n->Attribute("type");
			tinyxml2::XMLNode* ext_node = n->FirstChild();
			while (ext_node)
			{
				const char* ext_t = ext_node->FirstChild()->Value();
				extToTypeMap[ext_t] = std::string(type_name);
				ext_node = ext_node->NextSibling();
			}
			n = n->NextSiblingElement();
		}
	}
	//Populate AssetModel with appropriate data
	for (boost::filesystem::recursive_directory_iterator end, dir("Assets/");
		dir != end; ++dir)
	{
		if (boost::filesystem::is_regular_file(*dir))
		{
			//this will create the available asset if it does not exist
			AvailableAsset::GetAvailableAssetFromFileName(dir->path().string());
		}
	}

	return false;
}

void AvailableAsset::Refresh( void )
{
	Task_AvailableAsset_Refresh t;
	t.Tick();
}

meta_define(Task_AvailableAsset_Refresh);
