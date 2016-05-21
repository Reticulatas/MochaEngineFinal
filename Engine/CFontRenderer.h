/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Edaurdo Chaves
-* See Footer for Revisions                                      */


#pragma once

#include "AssetManager.h"
#include "Material.h"
#include "CMeshRenderer.h"
#include "MochaString.h"
#include "AssetManager.h"
#include "CTransform.h"
#include"ColorNode.h"
#include "TextureNode.h"
#include "UVTransformNode.h"
#include "EditorSubsystem_Interface.h"
#include "CDiffuseRenderer.h"

 
class CFontRenderer : public CDiffuseRenderer
{
public:
	enum FONT_JUSTIFICATION
	{
		LEFT_JUSTIFIED = 0,
		RIGHT_JUSTIFIED,
		CENTER_JUSTIFIED,

		NUM_FONT_JUSTIFICATION
	};
	struct ColouredString
	{
		ColouredString(const std::string& str, const Color& clr)
			: mText(str), mColor(clr)
		{ }
		std::string mText;
		Color mColor;
	};
	struct NewLineOfString
	{
		NewLineOfString() : mNumNewlines(0), mSentenceDimensions(0.f, 0.f), 
			mJustificationOffset(0.f), maxYPos(0.f), minYPos(0.f)
		{ }
		std::vector<ColouredString> mText;
		int mNumNewlines;
		Vector2 mSentenceDimensions; // For justification (scaling)
		float mJustificationOffset; // position offset for each character when rendering to justification
		// float mSentenceOffsetToYCenter; // different from mSentenceDimensions, this is translation offset
		float maxYPos; // max and min y positions for this sentence
		float minYPos;
	};

	CFontRenderer(void);
	virtual ~CFontRenderer();

	void OnFree(void) override;
	void OnInit(void) override;
	void OnStart(void) override;
	void OnUpdate(void) override;
	void OnEditorUpdate(void) override;
	void OnDraw(void) override;

	void Logic(CCamera* cam);
	void BindTransforms(const Matrix4& WorldViewProj, 
		const Vector2& texScale, const Vector2& texOffset, const Color& clr);

	void ClampTextWithinWidth(float lineWidthLimit);
	float GetLineWidth(unsigned int lineIndex) const;
	unsigned int GetNumberOfLines() const;

	// Returns center position and half width/height. Check if !mIsPerspective, then don't do culling
	virtual void GetFrustumCullData(Vector3& pos, Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

	Mesh* GetMesh();
	Material* GetMaterial();
	
	unsigned int mFontSize;
	float mFontScale;
	std::string mText; // use <#1,0.5,0.5,0.1>#Hello World <#0,0,0,1#>BlackText, to color code the string in RGBA
	Color mColor;
	FONT_JUSTIFICATION mJustification;
	AssetHandle mFontAsset;
	float mNewlineSpacing;
	DEPRECATED bool mIsPerspective;

	virtual void CopyFrom(const ::CFontRenderer* c);
	COMPONENTCLONE(CFontRenderer);

private:
	AssetHandle mFontTexure;
	FontData* mFontData;
	std::vector<NewLineOfString> mCachedLinesOfStrings; // Each element is a line of colored text in a new line
	float mAmountOfYGapBetweenLines; // y distance between newlines

	// For detecting changes
	unsigned mPrevFontSize; // update all properties
	float mPrevFontScale;
	std::string mPrevText; // update all properties
	Color mPrevColor; // update all string properties
	FONT_JUSTIFICATION mPrevJustification; // update only justification
	float mPrevNewlineSpacing;
	DEPRECATED bool mPrevIsPerspective;

	// For Frustum culling corner positions
	Vector2 mOffset; // get text's actual center position from gameObject.position and this offset
	Vector3 mXAxis;
	Vector3 mYAxis;
	Vector3 mCenterPos;

	float mMaxHeight; // stores the max height of all char in font
	void UpdateMinMaxDimensions();
	void CheckChanges();
	void CheckStringForTags(); // Called when mText changes, looks for "<#" "#>" tags

	void ForceRegeneration();

	___SERIALIZE_BEGIN___
		___DEFBASE(CDiffuseRenderer)
		___DEFSER(mFontSize, 1)
		___DEFSER(mText, 1)
		___DEFSER(mColor, 1)
		___DEFSER(mJustification, 1)
		___DEFSER(mIsPerspective, 1)
		___DEFSER(mFontAsset, 2);
		___DEFSER(mFontScale, 2);
		___DEFSER(mNewlineSpacing, 4);
    if (ver < 3)
      mPerspective = mIsPerspective;
	___SERIALIZE_END___

public:
	metadef(CFontRenderer)
    m_addbase(CDiffuseRenderer)
	endmetadef;
};
 
___SERIALIZE_CLASS(CFontRenderer, 4);

/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change