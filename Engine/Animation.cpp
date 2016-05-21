#include "stdafx.h"
#include "Animation.h"
#include "AssetManager.h"
#include "ModelLoader.h"

Animation::Animation(std::string name) : Asset(), mDuration(0)
{
  mName = name;
  SetType("Animation");
}

float Animation::GetDuration() const
{
  return mDuration;
}

std::vector<Track>& Animation::GetTracks()
{
  return mTracks;
}

std::string Animation::GetName()
{
  return mName;
}

void Animation::OnAssetFileChanged()
{
  std::string animPath = mName.substr(0, mName.find_first_of("_"));  

  Mesh* mesh = AssetManager::getInstance()->GetAsset(mName).GetAssetPtr<Mesh>();

  bool result = ModelLoader::getInstance()->LoadBean(&mesh, fileID.Filename());
  if (!result)
    std::cout << "Failed to reload " << mName << std::endl;
}
