%{
#include "CFontRenderer.h"
%}

class CFontRenderer : public CDiffuseRenderer
{
	CFontRenderer();
	virtual ~CFontRenderer();

public:

	enum FONT_JUSTIFICATION
	{
		LEFT_JUSTIFIED = 0,
		RIGHT_JUSTIFIED,
		CENTER_JUSTIFIED,

		NUM_FONT_JUSTIFICATION
	};
	
	void ClampTextWithinWidth(float lineWidthLimit);
	float GetLineWidth(unsigned int lineIndex) const;
	unsigned int GetNumberOfLines() const;

	unsigned int mFontSize;
	float mFontScale;
	std::string mText;
	Color mColor;
	FONT_JUSTIFICATION mJustification;
	AssetHandle mFontAsset;
	float mNewlineSpacing;
};