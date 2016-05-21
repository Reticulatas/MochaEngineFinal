#pragma once
#include <string>
#ifdef MOCHAEDITOR
#include "Engine/UniqueFileID.h"
#include "Engine/AssetTypes.h"
#else
#include "UniqueFileID.h"
#endif
#ifdef MOCHAEDITOR
#include "JuceHeader.h"
#endif

typedef std::string AssetType;
class AssetHandle;

//Asset Watcher contains observables related to file system and asset watching
class IObservable;
class AvailableAssetWatcher
{
	IObservable* observer;
	AvailableAssetWatcher(void);
	~AvailableAssetWatcher(void);

	void ObserverCallback(void);
public:
	static AvailableAssetWatcher* getInstance(void);
};

bool isExtensionOfType(std::string ext, AssetType type);
AssetType GetTypeFromExtension(std::string ext);

//Available asset is synced with the assets folder and represents all the assets that 'could' be loaded
//This class can also control reloading of specific assets when their files change and thus must adhere to:
//Specifications:
//Non-transient - class should exist for lifetime of file it monitors
//No data		- class should not contain data that can be invalidated by clients
//Hard linked	- any external references to this struct must be done loosely through another source than the container in which it resides
//Synced		- data in this class should match that of the data in AssetHandle and the subsequent RAW Asset

class AvailableAsset
{
	std::string filename;	//name with ext
	std::string filepath;	//relative file path
	std::string name;		//name w/o ext
	UniqueFileID fileID;
	std::string type;		//type derived from ext
public:
	AssetType Type() const { return type; }
	UniqueFileID FileID() const { return fileID; }
	std::string Filename() const { return filename; }
	std::string Filepath() const { return filepath; }
	std::string Name() const { return name; }

	/* Gets the Asset Handle tied to this available asset 
	   if there isn't one, it loads the asset				*/
	AssetHandle GetAssetHandle(void);

	AvailableAsset(std::string _filename);
	~AvailableAsset();

	typedef std::vector<AvailableAsset*>::iterator AvailableAsset_iter;
	static std::vector<AvailableAsset*> GetAvailableAssetsFromType(AssetType type);
	static std::vector<AvailableAsset*> GetAvailableAssetsFromExtension(std::string& ext);
	static AvailableAsset* GetAvailableAssetFromUFID(UniqueFileID id, bool createIfNotFound = true);
	static AvailableAsset* GetAvailableAssetFromFileName(std::string filename_, bool createIfNotFound = true);
	static AssetType GetTypeFromExtension(std::string ext);
	static bool isExtensionOfType(std::string ext, AssetType type);

	static void Refresh(void);
};

//Available Asset threaded refresh for startup - task system
#ifdef MOCHAEDITOR
#include "Engine/Task.h"
#else
#include "Task.h"
#endif
struct Task_AvailableAsset_Refresh : public Task
{
public:
	bool Tick(void);
    
	metadef(Task_AvailableAsset_Refresh)
		endmetadef;
};
