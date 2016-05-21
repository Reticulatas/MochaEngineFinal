#include "stdafx.h"
#include "CFontRenderer.h"
#include "GraphicsInclude.h"
#include <cstdlib>

CFontRenderer::CFontRenderer() : CDiffuseRenderer()
{

}

void CFontRenderer::OnFree( void )
{
}

void CFontRenderer::OnInit( void )
{
  CDiffuseRenderer::OnInit();

	mMesh = AssetManager::getInstance()->GetAsset("FontQuad.bean").GetAssetPtr<Mesh>();
	mFontSize = 48;
	mFontScale = 1.f;
	mFontData = 0;
	mText = "Test123";
	mColor = Color(1.f, 1.f, 1.f, 1.f); // White
	mJustification = LEFT_JUSTIFIED;
	mPerspective = true;
	mNewlineSpacing = 0.f;

	mPrevFontSize = 0; // update all properties
	mPrevFontScale = 0.0f;
	mPrevText = std::string(); // update all properties
	mPrevColor = Color(0.f, 0.f, 0.f, 1.f); // update all string properties
	mPrevJustification = NUM_FONT_JUSTIFICATION; // update only justification
	mPrevNewlineSpacing = 1.0f;
	// mPrevIsPerspective = !mPerspective;

	mOffset = Vector2(0.0f, 0.0f);
	mXAxis = Vector3(0.0f);
	mYAxis = Vector3(0.0f);
	mCenterPos = Vector3(0.0f);
	mMaxHeight = 0.0f;
}

void CFontRenderer::OnStart( void )
{
	CDiffuseRenderer::OnStart();

	if (!mFontAsset.Valid())
		mFontAsset = AssetManager::getInstance()->GetAsset("verdana.ttf");
	
	ForceRegeneration();
}

void CFontRenderer::OnUpdate( void )
{
	/*
	Vector2 scaleToGameObject(gameObject->transform->scale.x / mSentenceDimensions.x,
		gameObject->transform->scale.y / mSentenceDimensions.y);
	// Use the smaller scale to fit the gameObject. If ortho, then don't scale
	mActualScale = mIsPerspective ? min(scaleToGameObject.x, scaleToGameObject.y) : 1.f;
	*/

	// Check if any properties changed
	CheckChanges();
}

void CFontRenderer::OnEditorUpdate()
{
	//if (EditorSS::GetHierarchyTreeSelectedGameObject() == gameObject)
	OnUpdate();
}

void CFontRenderer::OnDraw(void)
{
	if (gameObject->GetSelected())
	{
		Vector3 tl(mCenterPos - mXAxis + mYAxis); // top left
		Vector3 tr(mCenterPos + mXAxis + mYAxis);
		Vector3 bl(mCenterPos - mXAxis - mYAxis);
		Vector3 br(mCenterPos + mXAxis - mYAxis); // btm right

		Renderer::getInstance()->DrawLine(tl, tr, COLOR::LIME_GREEN);
		Renderer::getInstance()->DrawLine(br, tr, COLOR::LIME_GREEN);
		Renderer::getInstance()->DrawLine(br, bl, COLOR::LIME_GREEN);
		Renderer::getInstance()->DrawLine(tl, bl, COLOR::LIME_GREEN);
	}
}

void CFontRenderer::UpdateMinMaxDimensions()
{
	const TextCharData& charData = mFontData->m_charData[(numChar-1)];
	mMaxHeight = charData.m_bitmapHeight * mFontScale;
}
void CFontRenderer::CheckChanges()
{
	bool needToUpdateStrings = false;
	needToUpdateStrings = (mPrevFontSize != mFontSize) ||
		(mPrevText != mText) ||
		(mPrevColor != mColor) || 
		(mPrevFontScale != mFontScale);
	bool needToUpdateJustification = (mPrevJustification != mJustification) ||
		(mPrevNewlineSpacing != mNewlineSpacing);

	FontData* fData = mFontAsset.GetAssetPtr<Font>()->GetFontData(mFontSize);
	if (mFontData != fData)
	{
		mFontData = fData;
		ForceRegeneration();

		needToUpdateStrings = true;
	}

	if (needToUpdateStrings) // Update the entire cache string
	{
		mPrevText = mText;
		mPrevFontSize = mFontSize;
		mPrevFontScale = mFontScale;
		mPrevColor = mColor;

		UpdateMinMaxDimensions();
		CheckStringForTags();

		// Update sentence size for every line of colored string
		for (auto& lineColorStr : mCachedLinesOfStrings)
		{
			float maxYPos = -FLT_MAX;
			float minYPos = FLT_MAX;
			lineColorStr.mSentenceDimensions = Vector2(0.f, 0.f);
			for (const auto& colorStr : lineColorStr.mText)
			{
				unsigned textSize = colorStr.mText.size();
				if (textSize == 0)
				{
					maxYPos = 0.4f/mFontScale;
					minYPos = 0.f;
				}
				// Gather total sentence's width for justification
				for (unsigned int i = 0; i < textSize; ++i)
				{
					char currChar = colorStr.mText[i];
					unsigned int index = currChar - startChar; // Index to array of char data

					const TextCharData& charData = mFontData->m_charData[index];
					lineColorStr.mSentenceDimensions.x += charData.m_advanceX;
					// Store max y dimensions in sentence (maybe for newline functionality)
					// mSentenceDimensions.y = std::max((float)charData.m_bitmapHeight, mSentenceDimensions.y);

					maxYPos = max((float)charData.m_posTop, maxYPos);
					minYPos = min((float)charData.m_posTop - (float)charData.m_bitmapHeight, minYPos);
				}
			}
			// lineColorStr.mSentenceOffsetToYCenter = (maxYPos + minYPos) * 0.5f * mFontScale; // Exact offset to get to the center
			lineColorStr.maxYPos = maxYPos * mFontScale;
			lineColorStr.minYPos = minYPos * mFontScale;
			lineColorStr.mSentenceDimensions.y = lineColorStr.maxYPos - lineColorStr.minYPos;
			lineColorStr.mSentenceDimensions.x *= mFontScale;
		}
	}
	
	if (needToUpdateStrings || needToUpdateJustification)
	{
		mPrevJustification = mJustification;
		mPrevNewlineSpacing = mNewlineSpacing;
		//mPrevIsPerspective = mPerspective;

		mAmountOfYGapBetweenLines = 0.f;
		Vector2 totalScale(0.f, 0.f);
		for (auto& lineColorStr : mCachedLinesOfStrings)
		{
			lineColorStr.mJustificationOffset = 0.f;
			switch (mJustification)
			{
			case LEFT_JUSTIFIED: break; // Do nothing
			case RIGHT_JUSTIFIED: lineColorStr.mJustificationOffset = lineColorStr.mSentenceDimensions.x;  break; // Use whole sentence width
			case CENTER_JUSTIFIED: lineColorStr.mJustificationOffset = lineColorStr.mSentenceDimensions.x * 0.5f; // Use half sentence width to shift
			}

			// mOffset.x = -lineColorStr.mJustificationOffset + lineColorStr.mSentenceDimensions.x * 0.5f;
			// mOffset.y = lineColorStr.mSentenceOffsetToYCenter;

			// non-justification data gathering
			mAmountOfYGapBetweenLines = Max<float>(mAmountOfYGapBetweenLines, lineColorStr.mSentenceDimensions.y); // Get the max y dimensions of all lines
			totalScale.x = Max<float>(totalScale.x, lineColorStr.mSentenceDimensions.x);
			// totalScale.y += lineColorStr.mSentenceDimensions.y;
		}
		totalScale *= 0.5f; // half width/height
		// Use specified newline spacing if it is >= 0
		if (mNewlineSpacing > 0.f)
			mAmountOfYGapBetweenLines = mMaxHeight * mNewlineSpacing;

		// Update the frustum culling data, center and x and y widths
		Vector3 right = gameObject->transform->GetRightVector();
		Vector3 up = gameObject->transform->GetUpVector();

		switch (mJustification)
		{
		case LEFT_JUSTIFIED: mOffset.x = totalScale.x; break; // +x/2
		case RIGHT_JUSTIFIED: mOffset.x = -totalScale.x;  break; // -x/2
		case CENTER_JUSTIFIED: mOffset.x = 0.f;
		}
		float maxY = (mCachedLinesOfStrings.empty()) ? 0.f : mCachedLinesOfStrings[0].maxYPos; // First line's max y
		float minY = (mCachedLinesOfStrings.empty()) ? 0.f :
			((mCachedLinesOfStrings.size() - 1) * -mAmountOfYGapBetweenLines) + mCachedLinesOfStrings.back().minYPos;
		mOffset.y = (maxY + minY) * 0.5f;
		totalScale.y = (maxY - minY) * 0.5f;
		
		mCenterPos = gameObject->transform->GetPosition() +
			mOffset.x * right + mOffset.y * up;
		mXAxis = right * totalScale.x;
		mYAxis = up * totalScale.y;
	}
}

void CFontRenderer::CheckStringForTags()
{
	std::vector<ColouredString> cachedColorStrings;
	mCachedLinesOfStrings.clear();

	// Go through the entire text, and find \\b, replace it with endChar+1
	const char* blankTag = "\\b";
	const unsigned int lengthOfBlankTag = std::strlen(blankTag);
	unsigned int startIndex = 0;
	unsigned index = 0;
	while ((index = mText.find(blankTag, startIndex)) != std::string::npos)
	{
		mText.replace(index, lengthOfBlankTag, 1, (char)(endChar + 1));

		startIndex = index + 1;
	}

	// Get a vector of string separated by color first
	Color currColor = mColor;
	unsigned int startStrIndex = 0;
	unsigned int colorTagIndex = mText.find("<#", 0);
	while (colorTagIndex != std::string::npos)
	{
		if (colorTagIndex > 0)
		{
			std::string currStr = mText.substr(startStrIndex, colorTagIndex - startStrIndex);
			cachedColorStrings.push_back(ColouredString(currStr, currColor));
		}

		// Grab the RGBA data between the '<#' '#>' tags
		colorTagIndex += 2; // position to right after <#
		if (colorTagIndex >= mText.size())
		{
			startStrIndex = std::string::npos;
			break;
		}

		unsigned int endIndex = mText.find(',', colorTagIndex);
		float r = (float)std::atof(mText.substr(colorTagIndex, endIndex - colorTagIndex).c_str());

		colorTagIndex = endIndex + 1; // position after ,
		endIndex = mText.find(',', colorTagIndex);
		float g = (float)std::atof(mText.substr(colorTagIndex, endIndex - colorTagIndex).c_str());

		colorTagIndex = endIndex + 1; // position after ,
		endIndex = mText.find(',', colorTagIndex);
		float b = (float)std::atof(mText.substr(colorTagIndex, endIndex - colorTagIndex).c_str());

		colorTagIndex = endIndex + 1; // position after ,
		endIndex = mText.find("#>", colorTagIndex);
		float a = (float)std::atof(mText.substr(colorTagIndex, endIndex - colorTagIndex).c_str());

		startStrIndex = endIndex + 2;
		currColor = Color(r, g, b, a);
		colorTagIndex = mText.find("<#", startStrIndex);
	}
	
	if (startStrIndex < mText.size())
	{
		std::string currStr = mText.substr(startStrIndex);
		if (!currStr.empty())
		{
			cachedColorStrings.push_back(ColouredString(currStr, currColor));
		}
	}

	// Go through each colored strings, and check if they contain a newline, "\n"
	NewLineOfString currLineOfStrings;
	unsigned int currNewlineCount = 0;
	const char* newlineTag = "\\n";
	for (const auto& colorStr : cachedColorStrings)
	{
		unsigned int startIndex = 0;
		unsigned index = colorStr.mText.find(newlineTag, startIndex);
		while (index != std::string::npos)
		{
			// When found, pull them out into a NewLineOfString, and add it to the vector
			ColouredString firstHalf(colorStr.mText.substr(startIndex, index - startIndex), colorStr.mColor);
			currLineOfStrings.mText.push_back(firstHalf);
			currLineOfStrings.mNumNewlines = currNewlineCount++;
			mCachedLinesOfStrings.push_back(currLineOfStrings);
			currLineOfStrings.mText.clear();

			startIndex = index + 2; // start from after the "\n"
			index = colorStr.mText.find(newlineTag, startIndex);
		}

		if (startIndex < colorStr.mText.size())
		{
			std::string remainingStr(colorStr.mText.substr(startIndex));
			if (!remainingStr.empty()) // Accumulate the strings that do not have newlines
			{
				ColouredString remainingColorStr(remainingStr, colorStr.mColor);
				currLineOfStrings.mText.push_back(remainingColorStr);
			}
		}
	}
	if (!currLineOfStrings.mText.empty())
	{
		currLineOfStrings.mNumNewlines = currNewlineCount; // Set the correct num of newlines
		mCachedLinesOfStrings.push_back(currLineOfStrings); // Add it to the cached vector
	}
}

void CFontRenderer::Logic(CCamera* cam)
{
	//XMMATRIX orthoMatXM = XMMatrixOrthographicLH((float)Engine::getInstance()->GetGameWnd()->GetWindowWidth(),
	//	(float)Engine::getInstance()->GetGameWnd()->GetWindowHeight(), -1.f, 1.f);
	//Matrix4 orthoMat(&orthoMatXM.m[0][0]);

	Color baseColor = mColor;
	Vector3 gameObjPos = gameObject->transform->GetPosition();
	if (!mPerspective)
		gameObjPos.z = 0.f;

	Matrix4 projMat = mPerspective ? cam->ProjView() : cam->Ortho();

	Vector2 charBtmLeftPos(0.f, 0.f);
	Vector2 bitmapDimensions((float)mFontData->m_fontTextureWidth, 
		(float)mFontData->m_fontTextureHeight);

	for (const auto& newlineStr : mCachedLinesOfStrings)
	{
		charBtmLeftPos.x = 0.f;
		charBtmLeftPos.y = newlineStr.mNumNewlines * -mAmountOfYGapBetweenLines;

		for (auto& colorStr : newlineStr.mText)
		{
			Color currColor = colorStr.mColor;
			currColor.r *= mColor.r;
			currColor.g *= mColor.g;
			currColor.b *= mColor.b;
			currColor.a *= mColor.a;

			unsigned textSize = colorStr.mText.size();
			for (unsigned int i = 0; i < textSize; ++i)
			{
				char currChar = colorStr.mText[i];
				unsigned int index = currChar - startChar; // Index to array of char data

				const TextCharData& charData = mFontData->m_charData[index];
				// Move position from btm left to center
				Vector2 currPos(charBtmLeftPos.x + charData.m_posLeft*mFontScale,
					charBtmLeftPos.y + charData.m_posTop*mFontScale);
				currPos.x -= newlineStr.mJustificationOffset; // Shift left if needed based on justification

				Matrix4 posTransMat;
				posTransMat.Translate(gameObjPos);
				Matrix4 rotMat = gameObject->transform->GetGlobalRotationMatrix();
				Matrix4 transMat;
				transMat.Translate(Vector3(currPos.x, currPos.y, 0.f));
				Matrix scaleMat;
				scaleMat.Scale(Vector3((float)charData.m_bitmapWidth * mFontScale,
					(float)charData.m_bitmapHeight * mFontScale, 1.f));

				// Use Rotation ON translation to offset the characters
				Matrix4 mvpMat = projMat * posTransMat * rotMat * transMat * scaleMat;

				Vector2 texScale(charData.m_bitmapWidth / bitmapDimensions.x,
					charData.m_bitmapHeight / bitmapDimensions.y);
				Vector2 texOffset(charData.m_bitmapStartU / bitmapDimensions.x,
					charData.m_bitmapStartV / bitmapDimensions.y);

				// Bind Transforms and draw
				BindTransforms(mvpMat, texScale, texOffset, currColor);
				GetMesh()->DrawMesh(Renderer::getInstance()->GetD3D11DeviceContext());

				// Update new btm left position for next char
				charBtmLeftPos.x += charData.m_advanceX*mFontScale;
				charBtmLeftPos.y -= charData.m_advanceY*mFontScale;
			}
		}
	}
}

void CFontRenderer::ClampTextWithinWidth(float lineWidthLimit)
{
	// Add up word widths from the start, until the current word add exceeds the limit
	float currWidth = 0.f;
	const char* newspaceChars = " ";
	const char* blankTag = "\\b";
	const char* newlineTag = "\\n";

	unsigned indexOfBlank = mText.find(blankTag, 0);
	unsigned indexOfNewline = mText.find(newlineTag, 0);
	unsigned indexOfColorStart = mText.find("<#", 0);
	unsigned indexOfColorEnd = mText.find("#>", 0);

	for (unsigned int i = 0; i < mText.size(); ++i)
	{
		char currChar = mText[i];
		unsigned int index = currChar - startChar; // Index to array of char data
		
		// Parsing of special characters
		if (indexOfBlank == i)
		{
			index = endChar + 1 - startChar; // index is 1 past last char
			indexOfBlank = mText.find(blankTag, i + 1);
		}
		else if (indexOfNewline == i)
		{
			currWidth = 0.f; // Reset the width
			indexOfNewline = mText.find(newlineTag, i + 1);
			continue; // Skip adding newline to width
		}
		else if (indexOfColorStart == i)
		{
			i = indexOfColorEnd + 1;
			indexOfColorStart = mText.find("<#", i+1);
			indexOfColorEnd = mText.find("#>", i + 1);
			continue; // Skip color tag part
		}

		const TextCharData& charData = mFontData->m_charData[index];
		currWidth += charData.m_advanceX * mFontScale; // Accumulate the word widths
		if (currWidth > lineWidthLimit) // This is the char that exceeded the limit
		{
			// Find the whitespace closest to this char
			unsigned int indexNewspace = mText.find_last_of(newspaceChars, i);
			if (indexNewspace == std::string::npos) // no whitespace char before this char
			{
				// Find a whitespace after
				indexNewspace = mText.find_first_of(newspaceChars, i);
				if (indexNewspace == std::string::npos) // If no whitespace anywhere, just leave it be (don't newline truncate)
				{
					return;
				}
			}
			// Here, I have the position of the whitespace
			mText.insert(indexNewspace + 1, newlineTag); // add a newline after the whitespace
			currWidth = 0.f;
			i = indexNewspace + 1; // Continue after the newline char
		}
	}

	CheckChanges(); // Make sure vectors of text are updated after adding newlines (if any)
}
float CFontRenderer::GetLineWidth(unsigned int lineIndex) const
{
	if (mCachedLinesOfStrings.empty())
		return 0.f;

	lineIndex = Minimum(lineIndex, mCachedLinesOfStrings.size() - 1);
	return mCachedLinesOfStrings[lineIndex].mSentenceDimensions.x;
}
unsigned int CFontRenderer::GetNumberOfLines() const
{
	return mCachedLinesOfStrings.size();
}

void CFontRenderer::GetFrustumCullData(Vector3& pos, Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
{
	pos = mCenterPos;
	xAxis = mXAxis; // Length is not 1, it is exactly the half width of text
	yAxis = mYAxis;
	zAxis.ZeroOut();

	// For debug drawing
#if 0
	Vector3 fontmax = pos + Vector3(abs(xAxis.x), abs(xAxis.y), abs(xAxis.z)) +
		Vector3(abs(yAxis.x), abs(yAxis.y), abs(yAxis.z));
	Vector3 fontmin = pos - Vector3(abs(xAxis.x), abs(xAxis.y), abs(xAxis.z)) -
		Vector3(abs(yAxis.x), abs(yAxis.y), abs(yAxis.z));

	Vector3 p1(fontmin.x, fontmin.y, fontmin.z);
	Vector3 p2(fontmax.x, fontmin.y, fontmin.z);
	Vector3 p3(fontmin.x, fontmin.y, fontmax.z);
	Vector3 p4(fontmax.x, fontmin.y, fontmax.z);

	Vector3 p5(fontmin.x, fontmax.y, fontmin.z);
	Vector3 p6(fontmax.x, fontmax.y, fontmin.z);
	Vector3 p7(fontmin.x, fontmax.y, fontmax.z);
	Vector3 p8(fontmax.x, fontmax.y, fontmax.z);

	Renderer::getInstance()->DrawLinePerma(p1, p2, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p1, p3, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p4, p2, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p4, p3, COLOR::RED);

	Renderer::getInstance()->DrawLinePerma(p5, p6, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p5, p7, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p8, p6, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p8, p7, COLOR::RED);

	Renderer::getInstance()->DrawLinePerma(p1, p5, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p2, p6, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p3, p7, COLOR::RED);
	Renderer::getInstance()->DrawLinePerma(p4, p8, COLOR::RED);
#endif
}

Mesh* CFontRenderer::GetMesh()
{
	return mMesh.GetAssetPtr<Mesh>();
}

Material* CFontRenderer::GetMaterial()
{
	return &mMaterial;
}

void CFontRenderer::BindTransforms(const Matrix4& WorldViewProj, 
	const Vector2& texScale, const Vector2& texOffset, const Color& clr)
{
  InputLayouts::SetInputLayout(InputLayouts::InputLayout_Vert);
  Renderer::getInstance()->SetBlendState(mBlendState);

  ConstantBufferManager::BindCameraBuffer(WorldViewProj);
  mMaterial.SetTextureScale(texScale);
  mMaterial.SetTextureOffset(texOffset);
  mMaterial.SetColor(clr);
  mMaterial.BindAttributes();

  if (mDrawOnTop)
    Renderer::getInstance()->EnableDepthWriting(false);
  else
    Renderer::getInstance()->EnableDepthWriting(true);
}

CFontRenderer::~CFontRenderer()
{

}

void CFontRenderer::CopyFrom(const ::CFontRenderer* c)
{
	mFontSize = c->mFontSize;
	mFontScale = c->mFontScale;
	mText = c->mText;
	mColor = c->mColor;
	mJustification = c->mJustification;
	mIsPerspective = c->mIsPerspective;
	mNewlineSpacing = c->mNewlineSpacing;

	mFontAsset = c->mFontAsset;
	mFontData = c->mFontData;
	mCachedLinesOfStrings = c->mCachedLinesOfStrings;
	mPrevFontSize = mFontSize - 1; // to force update
	mPrevFontScale = c->mPrevFontScale;
	mPrevText = c->mPrevText;
	mPrevColor = c->mPrevColor;
	mPrevNewlineSpacing = c->mPrevNewlineSpacing;
	mPrevIsPerspective = c->mPrevIsPerspective;
	mPrevJustification = c->mPrevJustification;

	mOffset = c->mOffset;
	mXAxis = c->mXAxis;
	mYAxis = c->mYAxis;
	mCenterPos = c->mCenterPos;
	
	CDiffuseRenderer::CopyFrom(c);
}

void CFontRenderer::ForceRegeneration()
{
	if (mFontData) {
		mFontTexure.SetAssetPtr(mFontData->m_texture);
		mMaterial.SetTexture(mFontTexure);
		mFontTexure.GetRawAssetPtr()->IsVirtual(true);
	}
}

meta_define(CFontRenderer)
___COMP_REGISTER(CFontRenderer);
/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change
