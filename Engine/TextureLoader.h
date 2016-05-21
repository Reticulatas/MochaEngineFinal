#pragma once

#include "RenderTarget.h"

const std::wstring DDS_Ext = L".dds";

class TextureLoader
{
public:

  // PNG, BMP, JPEG, TIFF, GIF, etc
  RenderTarget* LoadTexture(const std::string& filePath, unsigned bindFlags);
  // DDS format only - cube maps
  RenderTarget* LoadDDSTexture(const std::string& filePath);

  RenderTarget* LoadTextureFromMemory(const unsigned char* buffer, const unsigned& bufferSize, unsigned bindFlags);

  static TextureLoader* getInstance();

private:
  
};
