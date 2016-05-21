#pragma once

#include "Asset.h"
#include "TextureLoader.h"

#include <string>
#include <map>

struct FT_FaceRec_;
struct FT_LibraryRec_;

const char startChar = ' ';
const char endChar = '~';
const unsigned int numChar = endChar - startChar + 1 + 1; // 1 additional for ASCII(219)

struct TextCharData
{
	unsigned int m_bitmapWidth; // char image's width and height in pixels
	unsigned int m_bitmapHeight;
	unsigned int m_bitmapStartU; // starting pixel position in u axis (left to right)
	unsigned int m_bitmapStartV; // starting pixel position in v axis (top to btm)
	int m_posLeft; // x position to place char (left to right)
	int m_posTop; // y position to place char (btm to top)
	unsigned int m_advanceX; // pixels to offset for spacing for next char (left to right)
	unsigned int m_advanceY; // usually 0, pixels to offset for spacing for next char (top to btm)
};

struct FontData
{
	FontData(unsigned int fontSize)
    : m_fontTextureWidth(0), m_fontTextureHeight(0), m_fontTexture(0), m_fontSize(fontSize), m_texture(0)
	{ }

	TextCharData m_charData[numChar]; // 1 for each font, for each unique size
	unsigned int m_fontTextureWidth;
	unsigned int m_fontTextureHeight;
	unsigned int m_fontTexture; // for OpenGL, a buffered texture id
	// int m_fontType;
	unsigned int m_fontSize;
  RenderTarget* m_texture;
};

// Per font type (per .ttf), eg Arial
class Font : public Asset
{
public:
	virtual void OnAssetFileChanged() override;

	virtual ~Font();
	void Init(const std::string& filename);
	
	FontData* GetFontData(unsigned int fontSize);
	Matrix3 GetCharTextureMat(char c, const FontData* fontData);

	___SERIALIZE_BEGIN___
		___DEFBASE(Asset)
	___SERIALIZE_END___

private:
	FT_FaceRec_* m_faceObj; // face obj for each font type
	std::map<unsigned int, FontData*> m_fontDataList;	

	// Called when a user wants to get a fontSize that does not exist yet
	FontData* AddFont(unsigned int fontSize);

public:
	metadef(Font)
	endmetadef;
};
___SERIALIZE_CLASS(Font, 1);

class FontManager
{
public:
	FontManager();
	~FontManager();

	FT_LibraryRec_* GetFreeTypeLibrary() { return m_library; }

	static FontManager* GetInstance();

private:
	FT_LibraryRec_* m_library;
};
#define FONT_MGR FontManager::GetInstance()
