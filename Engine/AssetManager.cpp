/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "AssetManager.h"
#include "MeshFactory.h"
#include <fstream>
#include "AudioManager.h"
#include "AvailableAssets.h"
#include "RoasterUtility.h"
#include "ModelLoader.h"
#include "Barrista.h"
#include "TextureLoader.h"

// BASE RAW ASSETHANDLE CLASS //

AssetHandle::AssetHandle(Asset* rawPtr)
{
  rawAssetPtr = rawPtr;
  mAssetPtr = rawPtr;
  if (rawAssetPtr)
  {
    rawAssetPtr->AddUseCounter();
	mAssetName = rawAssetPtr->GetName();
  }
}

AssetHandle::AssetHandle( const AssetHandle& rhs )
{
  *this = rhs;
}

AssetHandle::AssetHandle()
{
  mAssetName = AssetHandleFee;
	rawAssetPtr = 0;
	mAssetPtr = 0;
}

std::string AssetHandle::GetName()const
{
	return mAssetName;
}

unsigned AssetHandle::GetId()const
{
	if (!GetRawAssetPtr())
		return 0;
  return GetRawAssetPtr()->GetId();
}

Asset* AssetHandle::GetRawAssetPtr() const
{
  return rawAssetPtr;
}

void AssetHandle::SetAssetPtr( void* assetPtr )
{
  this->mAssetPtr = assetPtr;
  this->rawAssetPtr = reinterpret_cast<Asset*>(assetPtr);
  if (assetPtr)
	  this->mAssetName = rawAssetPtr->GetName();
  else
	  mAssetName = "[unassigned by user]";
}

AssetHandle::~AssetHandle()
{
  //Release();
}

void AssetHandle::Release( void )
{
  if(GetId())
  {
    AssetManager::getInstance()->ReleaseAsset(this);
  }
}

bool AssetHandle::Valid() const
{
	return (rawAssetPtr != 0 && mAssetPtr != 0);
}

AssetHandle& AssetHandle::operator=(const AssetHandle& rhs)
{
  mAssetName = rhs.mAssetName;
  rawAssetPtr = rhs.rawAssetPtr;
  mAssetPtr = rhs.mAssetPtr;
  return *this;
}

std::string AssetHandle::GetUnderlyingAssetType() const
{
	if (!GetRawAssetPtr())
		return "[inv]";
	return GetRawAssetPtr()->GetType().c_str();
}

void AssetHandle::ResetHandle()
{
  mAssetName = AssetHandleFee;
	rawAssetPtr = 0;
	mAssetPtr = 0;
}

bool AssetHandle::operator!=(const AssetHandle& rhs)
{
	return (rhs.GetRawAssetPtr() != GetRawAssetPtr());
}

bool AssetHandle::operator==(const AssetHandle& rhs)
{
	return (rhs.GetRawAssetPtr() == GetRawAssetPtr());
}

void AssetManager::ReleaseAsset( AssetHandle* handle)
{
  AssetIter iter = mAssetContainer.find(handle->GetId());
  Asset* pAsset = iter->second;

  if(pAsset->GetUseCounter())
  {
    pAsset->SubtractUseConter();
    if (pAsset->GetUseCounter() == 0 && pAsset->IsAutoUnloading())
    {
      IdIter idIterator = mAssetNameReferece.find(pAsset->GetName());
      mAssetNameReferece.erase(idIterator);
      mAssetContainer.erase(iter);
      delete(pAsset);
      numOfAssets--;
    }
    handle->SetAssetPtr(0);
  }


}

/* Singleton */
AssetManager* AssetManager::getInstance()
{
static AssetManager* instance  = 0;

  if(!instance)
    instance = new AssetManager();

  return instance;
}

AssetManager::AssetManager() :mCurrentId(0), numOfAssets(0)
{
}

void AssetManager::Init()
{
  MeshFactory* meshFactory = MeshFactory::getInstance();
  /* Add the box */

  Mesh* box          = meshFactory->CreateBoxMesh();
  Mesh* quad         = meshFactory->CreateNewQuadMesh();
  Mesh* debugMesh    = meshFactory->CreateDebugMesh();
  Mesh* particleMesh = meshFactory->CreateParticleMesh(MAXPARTICLEBUFFER);
  Mesh* textMesh     = meshFactory->CreateTextMesh(5000);
  Mesh* ppsQuad      = meshFactory->CreatePPSQuad();

  //box->SetAutoUnload(false);
  quad->SetAutoUnload(false);
  debugMesh->SetAutoUnload(false);
  particleMesh->SetAutoUnload(false);
  textMesh->SetAutoUnload(false);
  ppsQuad->SetAutoUnload(false);

  //InsertAssetInMap(box);
  InsertAssetInMap(quad);
  InsertAssetInMap(debugMesh);
  InsertAssetInMap(particleMesh);
  InsertAssetInMap(textMesh);
  InsertAssetInMap(ppsQuad);

  assChecker = 0;  

  std::cout << "Asset Manager Initialized" << std::endl;
}

void AssetManager::PrintLoadedAssets()
{
  std::cout << std::endl << "Assets Loaded : " << std::endl << std::endl;

  AssetIter iter;
  for(iter = mAssetContainer.begin(); iter != mAssetContainer.end(); ++iter)
  {
      std::cout << iter->second->GetName()  << "(" << iter->second->GetUseCounter() << ")" << std::endl;
  }

  std::cout << std::endl << "-------------" << std::endl;
}

Mesh* AssetManager::LoadMeshes( const std::string& meshName )
{
  Mesh* newMesh = 0;
  std::string fullPath = GetAbsolutePathOfFile(meshName);
  ModelLoader::getInstance()->LoadBean(&newMesh, fullPath);
  return newMesh;
}

Animation* AssetManager::LoadAnimations(const std::string& animName)
{
  BoostPath fullPath = GetAbsolutePathOfFile(animName);
  return ModelLoader::getInstance()->LoadAnim(fullPath);
}

RenderTarget* AssetManager::LoadTextures( const std::string& textName )
{
  std::string fullPath = GetAbsolutePathOfFile(textName);
  //check if file exists
  if (!boost::filesystem::exists(boost::filesystem::path(fullPath)))
  {
    //if it does not exist then load a default file
    if (boost::filesystem::exists(boost::filesystem::path("Assets/Textures/missingTexture.png")))
  	  return LoadTextures("missingTexture.png");
    else
      throw std::exception("Missing Texture does not exist");
  }

  std::wstring extension = boost::filesystem::path(fullPath).extension().c_str();

  // DDS file
  if (extension == DDS_Ext)
    return TextureLoader::getInstance()->LoadDDSTexture(fullPath);

  // Normal textures
  else
    return TextureLoader::getInstance()->LoadTexture(fullPath, D3D11_BIND_SHADER_RESOURCE);
}

RenderTarget* AssetManager::LoadRenderTargets(const std::string& textName)
{
  std::string fullPath = GetAbsolutePathOfFile(textName);
  //check if file exists
  if (!boost::filesystem::exists(boost::filesystem::path(fullPath)))
      throw std::exception("Render Target does not exist");

  RenderTarget* pRenderTarget = new RenderTarget();

  // Default values - will be resized later
  unsigned textureWidth = Engine::getInstance()->GetVRWnd()->GetWindowWidth();
  unsigned textureHeight = Engine::getInstance()->GetVRWnd()->GetWindowHeight();
  pRenderTarget->InitRenderTarget(textureWidth, textureHeight, Renderer::getInstance()->GetFrameBufferFormat());

  return pRenderTarget;
}

Sound* AssetManager::LoadSounds( const std::string& soundName )
{
  std::string fullPath = GetAbsolutePathOfFile(soundName);
  Sound* pSound = nullptr; // AudioManager::getInstance()->GetAudio(fullPath);
  //pSound->SetName(soundName);
  throw AssetException(AssetException::ASSETINVALID);

  return pSound;
}

Sound* AssetManager::LoadMusics( const std::string& musicName )
{
  std::string fullPath = GetAbsolutePathOfFile(musicName);
  Sound* pSound = nullptr; // AudioManager::getInstance()->GetAudio(fullPath);
  //pSound->SetName(musicName);
  throw AssetException(AssetException::ASSETINVALID);

  return pSound;
}

Shader* AssetManager::LoadShaders(const std::string& shaderFile)
{
  std::ifstream file(GetAbsolutePathOfFile(shaderFile));
  Shader* pShader = new Shader();

  if (file.is_open())
  {
    while (!file.eof())
    {
      std::string subShader;
      file >> subShader;
      pShader->AddSubShader(subShader);
    }
  }

  file.close();

  return pShader;
}

Asset* AssetManager::LoadSubShaders(const std::string& subshaderFile)
{
  std::string fullPath = GetAbsolutePathOfFile(subshaderFile);
  std::wstring filepath = std::wstring(fullPath.begin(),fullPath.end());
  SUBSHADERTYPE sstype = GetSubShaderTypeFromFile(fullPath);

  Asset* pSubShader = 0;;

  switch (sstype)
  {
  case VERTEX_SHADER:

    pSubShader = new SubShader<ID3D11VertexShader>(filepath, sstype);
    break;

  case PIXEL_SHADER:

    pSubShader = new SubShader<ID3D11PixelShader>(filepath, sstype);
    break;
  case COMPUTE_SHADER:

	  pSubShader = new SubShader<ID3D11ComputeShader>(filepath, sstype);
	  break;

  case GEOMETRY_SHADER:

	  pSubShader = new SubShader<ID3D11GeometryShader>(filepath, sstype);
	  break;
  }

  pSubShader->SetAutoUnload(false);
  pSubShader->AddUseCounter();

  return pSubShader;
}  

void AssetManager::AssignId( Asset* asset, unsigned id /* = 0 */ )
{
  if(!asset)
    return;

  if(!id)
  {
    mCurrentId++;
    asset->SetId(mCurrentId);
    try
    {
      asset->fileID = UniqueFileID(GetAbsolutePathOfFile(asset->GetName()));
    }
    catch (FileIDException e)
    {
      Log("Warning: No file found for asset: " + asset->GetName());
    }
  }
  else
  {
    if(mCurrentId < id)
      mCurrentId = id + 1;
    asset->SetId(id);
  }
}

void AssetManager::InsertAssetInMap( Asset* asset )
{
  AssignId(asset,asset->GetId());
  mAssetContainer.insert(std::pair<unsigned, Asset*>(asset->GetId(), asset));
  mAssetNameReferece.insert(std::pair<std::string, unsigned>(asset->GetName(), asset->GetId()));
  mAssetFIDReference.emplace(asset->FileID(), asset->GetId());
  numOfAssets = mAssetContainer.size();
}

std::string AssetManager::GetAbsolutePathOfFile(const std::string& filename, bool createIfNotFound)
{
	AvailableAsset* aa = AvailableAsset::GetAvailableAssetFromFileName(filename, createIfNotFound);
	if (!aa)
		throw AssetException(AssetException::ASSETINVALID);
	return aa->FileID().Filename();
}

int AssetManager::GetIdFromName( const std::string& assetName )
{
  IdIter iter = mAssetNameReferece.find(assetName);

  // Cannot find Asset
  if(iter == mAssetNameReferece.end())
    return -1;
  
  return iter->second;
}

AssetHandle AssetManager::GetAsset( unsigned assetId )
{
	AssetIter iter = mAssetContainer.find(assetId);
	if (iter == mAssetContainer.end())
		throw AssetException(AssetException::ASSETNOTFOUND);
	return AssetHandle(iter->second);
}

AssetHandle AssetManager::GetAsset(std::string assetId)
{
	int id = GetIdFromName(assetId);
	if (id == -1)
	{
		Asset* a = LoadAsset(assetId);
		if (!a)
			throw AssetException(AssetException::ASSETFAILEDTOLOAD);
		return a;
	}
	return GetAsset((unsigned)id);
}

void AssetManager::CheckForChangedAssets()
{
	// Iterate through all assets
	for (std::pair<const unsigned, Asset*>& asset : mAssetContainer)
	{
    if (asset.second->mName == AssetHandleFee)
			continue;
		if (!asset.second->FileID().Valid())
			continue;
		boost::filesystem::path filePath = asset.second->FileID().Filename();
		boost::filesystem::path metaPath = filePath;
		metaPath += MetaExt;

		// If meta file already created, check time stamp
		if (boost::filesystem::exists(metaPath))
		{
			// Check time stamp
			FILETIME writeTime = GetTimeStamp(filePath),
				metaTime = GetMetaTimeStamp(metaPath);

			// meta time is earlier than write time, reload asset
			if (CompareFileTime(&metaTime, &writeTime) < 0)
			{
				//UniqueFileID fileid(filePath.string());
				GetAsset(filePath.leaf().string()).GetRawAssetPtr()->OnAssetFileChanged();

				// Write time stamp to meta file
				WriteTimeStamp(metaPath, writeTime);
			}
		}

    else
		{
			// Write time stamp to meta file
			FILETIME writeTime = GetTimeStamp(filePath);
			WriteTimeStamp(metaPath, writeTime);
		}
	}
}

Asset* AssetManager::LoadAsset(const std::string& fileName)
{
	IdIter iter = mAssetNameReferece.find(fileName);
	if (iter == mAssetNameReferece.end())
	{
		//load asset
		AvailableAsset* asset = AvailableAsset::GetAvailableAssetFromFileName(fileName, false);
		if (!asset)
		{
			//asset never exists
			MessageBox(0, std::wstring(fileName.begin(), fileName.end()).c_str(), L"Asset not found", MB_OK);
			throw AssetException(AssetException::ASSETFILENOTFOUND);
		}
		meta::metafunction_method* loader = AssetManager::getInstance()->getmeta().getfunctionbyname("Load" + asset->Type());
		if (!loader)
		{
			//loader doesn't exist
			throw AssetException(AssetException::ASSETFAILEDTOLOAD);
		}
		Asset* newAsset = loader->invoke(AssetManager::getInstance(), fileName).GetAs<Asset*>();
		if (newAsset == 0)
		{
			Log("Asset failed to load for unknown reason: " + fileName);
			return 0;
		}
    newAsset->SetName(fileName);
	newAsset->SetType(asset->Type());
		//add to the lists
		if (newAsset)
			InsertAssetInMap(newAsset);
		return newAsset;
	}
	//asset exists
	return mAssetContainer[iter->second];
}

SUBSHADERTYPE AssetManager::GetSubShaderTypeFromFile(boost::filesystem::path path)
{
  std::string subshaderfile = path.relative_path().stem().string();
  subshaderfile = subshaderfile.substr(subshaderfile.find("_") + 1);

  std::transform(subshaderfile.begin(), subshaderfile.end(), subshaderfile.begin(), ::tolower);

  SUBSHADERTYPE sstype;
  if (subshaderfile == VSfilstr)
    sstype = VERTEX_SHADER;
  else if (subshaderfile == PSfilstr)
    sstype = PIXEL_SHADER;
  else if (subshaderfile == CSfilstr)
	sstype = COMPUTE_SHADER;
  else if (subshaderfile == GSfilstr)
	sstype = GEOMETRY_SHADER;

  return sstype;
}

ScriptAsset* AssetManager::LoadScripts(const std::string& scriptFile)
{
	//nada
	return new ScriptAsset();
}

Font* AssetManager::LoadFonts(const std::string& fontFile)
{
	std::string fontFilename = "Assets/Fonts/" + fontFile;
	Font* newFont = new Font;
	newFont->Init(fontFilename);
	return newFont;
}

ConvexHullMesh* AssetManager::LoadHulls(const std::string& hullFile)
{
	
	std::string fullPath = GetAbsolutePathOfFile(hullFile);
	ConvexHullMesh* ch = ModelLoader::getInstance()->LoadHull(fullPath);
	return ch;
}

meta_define(AssetManager);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
