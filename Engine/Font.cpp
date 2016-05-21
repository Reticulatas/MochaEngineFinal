#include "stdafx.h"

#include "Font.h"

#include <fstream>

#include <ft2build.h>
#include FT_FREETYPE_H

const unsigned int DEFAULT_FONT_SIZE = 12;

void Font::OnAssetFileChanged()
{

}

void Font::Init(const std::string& filename)
{
	FT_Error error = FT_New_Face(FONT_MGR->GetFreeTypeLibrary(), 
		filename.c_str()/*"verdana.ttf"*/, 0, &m_faceObj);
	/* or use FT_New_Memory_Face(m_library,
							buffer,    // first byte in memory
							size,      // size in bytes
							0,         // face_index
							&face );
	*/
	if (error == FT_Err_Unknown_File_Format)
	{
		Log("Error in Font::Init, font file read but unsupported\n");
	}
	else if (error)
	{
		Log("Error in Font::Init, font file could not be read\n");
	}
}
Font::~Font()
{
	FT_Done_Face(m_faceObj);

	for (auto& elem : m_fontDataList)
	{
    if (elem.second->m_texture)
      delete elem.second->m_texture;
		delete elem.second;
		elem.second = nullptr;
	}
	m_fontDataList.clear();
}

FontData* Font::AddFont(unsigned int fontSize)
{
	FT_Error error = FT_Set_Pixel_Sizes(
		m_faceObj, // handle to face object
		0, // pixel_width
		fontSize); // pixel_height
	// eg for setting up 16pt font for 300x300 dpi device
	/*error = FT_Set_Char_Size(
	m_face[0], // handle to face object
	0, // char_width in 1/64th of points, 0 means same as height
	16*64, // char_height in 1/64th of points, 0 means same as width
	300, // horizontal device resolution
	300); // vertical device resolution
	*/
	if (error)
	{
		Log("Error in AddFont, could not set font char size\n");
	}

	if (m_fontDataList.find(fontSize) != m_fontDataList.end())
	{
		Log("Font size already exists!\n");
		return nullptr;
	}
	FontData* fontData = new FontData(fontSize);
	m_fontDataList[fontSize] = fontData;

	unsigned int totalWidth = 0;
	unsigned int maxWidth = 0;
	unsigned int maxHeight = 0;
	int maxYValue = -INT32_MAX;
	int minYValue = INT32_MAX;
	unsigned int currIndex = 0;
	FT_GlyphSlot glyph = m_faceObj->glyph;
	for (char c = startChar; c <= endChar; ++c)
	{
		FT_Error error = FT_Load_Char(m_faceObj, c, FT_LOAD_RENDER);
		if (error)
		{
			std::cout << "Error in Text Render, cannot load char\n";
			continue;
		}

		currIndex = c - startChar;
		fontData->m_charData[currIndex].m_bitmapWidth = glyph->bitmap.width;
		fontData->m_charData[currIndex].m_bitmapHeight = glyph->bitmap.rows;
		fontData->m_charData[currIndex].m_bitmapStartU = totalWidth;
		fontData->m_charData[currIndex].m_bitmapStartV = 0; // Always top of texture (read downwards by height)
		fontData->m_charData[currIndex].m_posLeft = glyph->bitmap_left;
		fontData->m_charData[currIndex].m_posTop = glyph->bitmap_top;
		fontData->m_charData[currIndex].m_advanceX = glyph->advance.x >> 6;
		fontData->m_charData[currIndex].m_advanceY = glyph->advance.y >> 6;

		totalWidth += glyph->bitmap.width + 1; // char are appended side by side in the texture with 1 pixel padding
		maxWidth = std::max(maxWidth, glyph->bitmap.width);
		maxHeight = std::max(maxHeight, glyph->bitmap.rows);

		maxYValue = max((int)glyph->bitmap_top, maxYValue);
		minYValue = min((int)glyph->bitmap_top - (int)glyph->bitmap.rows, minYValue);
	}
	maxHeight = (maxYValue - minYValue);
	// Add a ASCII char (219) at the end
	++currIndex;
	fontData->m_charData[currIndex].m_bitmapWidth = maxWidth; // actual pixel width in texture is maxWidth + 2
	fontData->m_charData[currIndex].m_bitmapHeight = maxHeight;
	fontData->m_charData[currIndex].m_bitmapStartU = totalWidth + 1;
	fontData->m_charData[currIndex].m_bitmapStartV = 0; // Always top of texture (read downwards by height)
	fontData->m_charData[currIndex].m_posLeft = 0;
	fontData->m_charData[currIndex].m_posTop = maxYValue;
	fontData->m_charData[currIndex].m_advanceX = maxWidth;
	fontData->m_charData[currIndex].m_advanceY = 0;

	totalWidth += maxWidth+2 + 1; // This white block is padded by 1 pixel on left and right

	unsigned int imageWidth = NextPowerOfTwo(totalWidth);
	unsigned int imageHeight = NextPowerOfTwo(maxHeight);
	fontData->m_fontTextureWidth = imageWidth;
	fontData->m_fontTextureHeight = imageHeight;
	unsigned int totalDim = imageWidth * imageHeight;
	int numColor = 4;
	unsigned short* pixels = new unsigned short[numColor*totalDim];
	// Can set all pixels to black first, then start copying over for safety
	for (unsigned int i = 0; i < numColor*totalDim; ++i)
	{
		pixels[i] = 0;
	}

	// Copy all character images into 1 pixel buffer
	for (char c = startChar; c <= endChar; ++c)
	{
		// std::cout << "current char: " << c << std::endl;
		FT_Load_Char(m_faceObj, c, FT_LOAD_RENDER);

		currIndex = c - startChar;
		unsigned int indexToSrcBuffer = 0;
		unsigned int indexToDestBufferStart = fontData->m_charData[currIndex].m_bitmapStartU * numColor; // (u, 0)
		// For every row of pixels
		for (unsigned int row = 0; row < fontData->m_charData[currIndex].m_bitmapHeight; ++row)
		{
			unsigned int currIndexToDestBuffer = indexToDestBufferStart; // start of row, (0, v)

			// For each col of pixels, copy it to the raw pixel buffer
			for (unsigned int col = 0; col < fontData->m_charData[currIndex].m_bitmapWidth; ++col)
			{
				unsigned char colorValue = glyph->bitmap.buffer[indexToSrcBuffer++];
				unsigned short actualColor = unsigned short((float)colorValue / 255.f * 65535.f);
				for (int i = 0; i < numColor; ++i)
				{
					pixels[currIndexToDestBuffer++] = actualColor;
				}
			}
			for (int i = 0; i < numColor; ++i)
			{
				pixels[currIndexToDestBuffer++] = 0; // 1 pixel padding after this char image
			}

			indexToDestBufferStart += imageWidth*numColor; // increment to next row
		}
	} // End of copying every char's bitmap buffer data
	
	// Add a ASCII char (219) pixels at the end
	++currIndex;
	unsigned int indexToDestBufferStart = (fontData->m_charData[currIndex].m_bitmapStartU-1) * numColor; // (u, 0)
	for (unsigned int row = 0; row < fontData->m_charData[currIndex].m_bitmapHeight; ++row)
	{
		unsigned int currIndexToDestBuffer = indexToDestBufferStart; // start of row, (0, v)
		unsigned short colorValue = 65535;

		for (int i = 0; i < numColor; ++i)
		{
			pixels[currIndexToDestBuffer++] = colorValue; // 1 pixel padding before this char image (left) to prevent bilinear filtering with black
		}
		// For each col of pixels, copy it to the raw pixel buffer
		for (unsigned int col = 0; col < fontData->m_charData[currIndex].m_bitmapWidth; ++col)
		{
			for (int i = 0; i < numColor; ++i)
			{
				pixels[currIndexToDestBuffer++] = colorValue;
			}
		}
		for (int i = 0; i < numColor; ++i)
		{
			pixels[currIndexToDestBuffer++] = colorValue; // 1 pixel padding after this char image (right)
		}
		for (int i = 0; i < numColor; ++i)
		{
			pixels[currIndexToDestBuffer++] = 0; // second pixel padding after this char image
		}

		indexToDestBufferStart += imageWidth*numColor; // increment to next row
	}

#define FLIP_FONT_TEXTURE 0
#if FLIP_FONT_TEXTURE
	// So I need to do it internally first before sending it to OpenGL
	for (unsigned int row = 0; row < imageHeight / 2; ++row)
	{
		unsigned int indexForRow = row * imageWidth;
		unsigned int indexForLastRow = (imageHeight - row - 1) * imageWidth;

		// Copy each row with the last row (with the two rows moving towards the center)
		for (unsigned int col = 0; col < imageWidth; ++col)
		{
			std::swap(pixels[indexForRow + col], pixels[indexForLastRow + col]);
		}
	}
#endif

  fontData->m_texture = new RenderTarget;
  fontData->m_texture->InitRenderTargetFromMemory(imageWidth, imageHeight, Renderer::getInstance()->GetFrameBufferFormat(), pixels);
	
#if 0
	// Load into OpenGL
	glGenTextures(1, &fontData->m_fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontData->m_fontTexture);

	// glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Freetype bitmaps are 1 B per pixel
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imageWidth, imageHeight, 0,
		GL_RED, GL_UNSIGNED_BYTE, pixels);
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Freetype bitmaps are 1 B per pixel
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
#endif

	delete[] pixels;
	pixels = nullptr;

	return fontData;
}

FontData* Font::GetFontData(unsigned int fontSize)
{
	auto it = m_fontDataList.find(fontSize);
	if (it == m_fontDataList.end())
	{
		FontData* newlyCreatedData = AddFont(fontSize);
		return newlyCreatedData;
	}

	return it->second;
}

// This can be done once and saved at load time
Matrix3 Font::GetCharTextureMat(char c, const FontData* fontData)
{
	unsigned int index = c - startChar;
	if (index < 0 || index >= numChar) // If out of bounds
		return Matrix3();

	// mapping tl,tr,br,bl {(0,1), (1,1), (1,0), (0,0)} -> {(x1,y1), (x2,y1), (x2,y2), (x1,y2)}
	// where x2 > x1, y2 > y1, width = x2 - x1, height = y2 - y1
	float width = (float)fontData->m_charData[index].m_bitmapWidth / (float)fontData->m_fontTextureWidth;
	float height = (float)fontData->m_charData[index].m_bitmapHeight / (float)fontData->m_fontTextureHeight;
	Matrix3 textureMat;
	textureMat[0][0] = width; // width
	textureMat[0][2] = fontData->m_charData[index].m_bitmapStartU / (float)fontData->m_fontTextureWidth; // x1
	textureMat[1][1] = height; // height
	textureMat[1][2] = 1.f - (fontData->m_charData[index].m_bitmapStartV / (float)fontData->m_fontTextureHeight + height); // 1.f - (y1 + height)
	return textureMat;
}

// Should be in CFontRenderer.cpp
void Render()
{
#if 0
	FontData* fontData = TEXT_MGR->GetFontData(fontType, fontSize); // Try to get the given font size
	if (!fontData)
	{
		fontData = TEXT_MGR->GetFontData(fontType, 0); // and if not possible get any size
		if (!fontData)
			return; // do not draw if unable to find a fontData
	}

	m_currShaderID = SHADER_ID::SHADER_FONT;
	Shader* currShader = m_shaders[m_currShaderID];
	currShader->Bind();

	glBindVertexArray(m_vao[FONT_QUAD]); // Must be inverted quad with y axis going downwards

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fontData->m_fontTexture);

	fVector2 charBtmLeftPos(pos.x, pos.y);
	while (*str)
	{
		char currChar = *str;
		unsigned int index = currChar - startChar;

		const TextCharData& charData = fontData->m_charData[index];
		// Move position from btm left to center
		fVector2 currPos(charBtmLeftPos.x + charData.m_posLeft, charBtmLeftPos.y + charData.m_posTop);

		Matrix4 modelMat = glm::translate<float>(glm::mat4(1.f), fVector3(currPos.x, currPos.y, pos.z)) *
			glm::scale(glm::mat4(1.f), //glm::vec3(1000.f, 60.f, 1.f));
			glm::vec3((float)charData.m_bitmapWidth, (float)charData.m_bitmapHeight, 1.f));
		Matrix4 mvpMat = (isPerspective ? m_matVPForSecondCamera : m_matOrtho) * modelMat;
		glUniformMatrix4fv(currShader->GetUniformLocations()[UL_MVP_MAT], 1, false, &mvpMat[0][0]);

		fVector4 color = ConvertRGBAColorToVec4(colorRGBA);
		glUniform4fv(currShader->GetUniformLocations()[UL_COLOR], 1, &color[0]);

		Matrix3 textureMat(TEXT_MGR->GetCharTextureMat(currChar, fontData));
		glUniformMatrix3fv(currShader->GetUniformLocations()[UL_TEXTURE_MAT], 1, true, &textureMat[0][0]);

		glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);

		// Update new btm left position for next char
		charBtmLeftPos.x += charData.m_advanceX;
		charBtmLeftPos.y -= charData.m_advanceY;
		++str;
	}

	glBindVertexArray(0); // Unbind vao
#endif
}

meta_define(Font);

// Font Manager Stuff
FontManager* FontManager::GetInstance()
{
	static FontManager inst;
	return &inst;
}

FontManager::FontManager()
{
	if (FT_Init_FreeType(&m_library))
	{
		Log("Unable to initialize freetype library\n");
	}
}
FontManager::~FontManager()
{
	FT_Done_FreeType(m_library); // somehow shows leak 0 B of memory
}

/////////////////////////////////////
//Original Author: Elroy Chan
//Modifications:
//	Date	-	Change