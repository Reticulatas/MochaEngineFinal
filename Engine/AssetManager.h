/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once
 
#include <map>
#include "UniqueFileID.h"
#include "Material.h" 
#include "Sound.h"
#include "Mesh.h"
#include "Animation.h"
#include "Font.h"
#include "RenderTarget.h"
#ifndef MOCHAEDITOR
#include "Shader.h"
#endif
#include "metadef.h"
#include "ConvexHullPhysics.h"

class ScriptAsset;
class Asset;
/************************************************************************/
/*							  ASSET HANDLE                              */
/************************************************************************/

class AssetHandle
{
protected:
  std::string mAssetName;
  Asset* rawAssetPtr;
  void* mAssetPtr;		//Pointer to the actual Asset
public:
  AssetHandle(Asset* rawPtr);
  AssetHandle(const AssetHandle& rhs);
  AssetHandle& operator=(const AssetHandle& rhs);
  AssetHandle();

  virtual ~AssetHandle();

  bool operator==(const AssetHandle& rhs);
  bool operator!=(const AssetHandle& rhs);

  bool Valid() const;

  unsigned GetId()const;
  std::string GetName()const;
  void SetAssetPtr(void* assetPtr);

  void Release(void);
  void ResetHandle();

  std::string GetUnderlyingAssetType() const;
  Asset* GetRawAssetPtr() const;
  template <typename T>
  T* GetAssetPtr() const {
	  static_assert(std::is_base_of<Asset, T>::value, "Lookup asset must be derived from Asset");
	  return dynamic_cast<T*>(GetRawAssetPtr());
  }

  ___SERIALIZE_BEGIN___
	  ___DEFSER(mAssetName, 1)
	  if (mAssetName != AssetHandleFee)
	  {
	  try {
		  *this = AssetManager::getInstance()->GetAsset(mAssetName);
	  }
	  catch (AssetException)
	  {
		  Log(mAssetName + " was not found. Objects containing this asset will be invalid.");
		  ResetHandle();
	  }
	  }
    ___SERIALIZE_END___


};
___SERIALIZE_CLASS(AssetHandle, 1);

/************************************************************************/
/*             				ASSET MANAGER                               */
/************************************************************************/

struct AssetException {
	enum AssetExceptionType { ASSETNOTFOUND, ASSETINVALID, ASSETFAILEDTOLOAD, ASSETFILENOTFOUND };
	AssetExceptionType _type;
	AssetException(AssetExceptionType e) : _type(e) {}
};

class AssetManager : public IMeta
{

private:

  // private member variables
  unsigned mCurrentId;
  unsigned numOfAssets;
  std::map<unsigned, Asset*> mAssetContainer;       // Hold ALL Assets : Materials, Textures, Sounds, etc...
  std::map<std::string, unsigned> mAssetNameReferece;       // Get ID from String for by name access
  std::map<UniqueFileID, unsigned> mAssetFIDReference;		// Get ID from Unique File ID

  typedef std::map<unsigned,Asset*>::iterator AssetIter;
  typedef std::map<std::string, unsigned>::iterator IdIter;
  typedef std::map<UniqueFileID, unsigned>::iterator AssetFileIDIter;

  void RelinquishHandleToPool(AssetHandle* ptr);

  // private methods
  Mesh*			LoadMeshes(const std::string& meshName);					/* If the mesh exits, returns a pointer to it, of not it will load and return the pointer */
  RenderTarget*	LoadTextures(const std::string& textName);
  RenderTarget*	LoadRenderTargets(const std::string& textName);
  //Material* LoadMaterial(std::string matName);
  Sound*		LoadSounds(const std::string& soundFile);
  Sound*		LoadMusics(const std::string& musicFile);
  Shader*   LoadShaders(const std::string& shaderFile);
  Asset*		LoadSubShaders(const std::string& shaderFile);
  Animation*	LoadAnimations(const std::string& animName);
  ScriptAsset* LoadScripts(const std::string& scriptFile);
  Font*			LoadFonts(const std::string& fontFile);
  ConvexHullMesh*	LoadHulls(const std::string& hullFile);

  void AssignId(Asset* asset, unsigned id = 0);
  int GetIdFromName(const std::string& assetName);
  SUBSHADERTYPE GetSubShaderTypeFromFile(boost::filesystem::path path);
  
public:
  std::string GetAbsolutePathOfFile(const std::string& filename, bool createIfNotFound = true);
  void CheckForChangedAssets();

   /* Constructors */
  AssetManager();
  ~AssetManager();

  meta::metafunction_method* assChecker;

  static AssetManager* getInstance();	/* singleton */
  void Init();

  /// TEMPLTES LOAD FUNCTION ////
  Asset* LoadAsset(const std::string& fileName);

  AssetHandle GetAsset(unsigned assetId);
  AssetHandle GetAsset(std::string assetId);

  void ReleaseAsset(AssetHandle* handle);

  void PrintLoadedAssets();

  void InsertAssetInMap(Asset* asset);

  unsigned GetAssetId();
  unsigned NumOfAssets() const { return numOfAssets; }

  friend class AssetViewportComponent;
  friend class AssetIcon;
  friend class AssetManager;
  friend struct Task_AssetManager_CheckForChangedAssets;
  friend class AssetHandle;

public:
	metadef(AssetManager)
		m_addfunc(LoadShaders)
		m_addfunc(LoadSubShaders)
		m_addfunc(LoadScripts)
		m_addfunc(LoadMeshes)
		m_addfunc(LoadTextures)
    m_addfunc(LoadRenderTargets)
    m_addfunc(LoadSounds)
		m_addfunc(LoadMusics)
		m_addfunc(LoadAnimations)
		m_addfunc(LoadFonts)
		m_addfunc(LoadHulls)
		endmetadef;
};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change