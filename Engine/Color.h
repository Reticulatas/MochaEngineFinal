/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#include "MathIncludes.h"
#include "IRegisterable.h"

struct Color
{
	/* Color variables */
	float r,g,b,a;				

	/* Constructors */
	Color(float _r = 0.0f,float _g = 0.0f,float _b = 0.0f,float _a = 1.0f);
	Color(const Vector4 &color);
	Color(const Color& clr);
	
	/* Color Operators */
	Color operator + (const Color& rhs);
	Color& operator += (const Color& rhs);
	Color operator - (const Color& rhs);
	Color& operator -= (const Color& rhs);
	Color operator * (const float& rhs);
	Color& operator *= (const float& rhs);
	Color& operator =(const Vector4& rhs);
	bool operator == (const Color& clr);
	bool operator != (const Color& clr);
	/* Color Operations */
	Color Blend(const Color& col, float blendFactor);			/* Blend two colors based on a blend factor for the first one */
	Color AlphaBlend(const Color& col);

	/*Access & conversions*/
  Vector4 ToVector4() const;

private:
	___SERIALIZE_BEGIN___
		___DEFSER(r, 1)
		___DEFSER(g, 1)
		___DEFSER(b, 1)
		___DEFSER(a, 1)
	___SERIALIZE_END___
};
___SERIALIZE_CLASS(::Color, 1);

/* Common colors */
namespace COLOR
{
	const struct Color WHITE(1.0f,1.0f,1.0f);
	const struct Color BLACK(0.0f);
	const struct Color RED(1.0f);
	const struct Color GREEN(0.0f,1.0f);
	const struct Color BLUE(0.0f,0.0f,1.0f);
	const struct Color YELLOW(1.0f,1.0f,0.0f);
	const struct Color SILVER(0.75f,0.75f,0.75f);
	const struct Color LIGHT_STEEL_BLUE(0.69f,0.77f,0.87f);
	const struct Color CORNFLOWER_BLUE(0.39f,0.60f,0.93f);
	const struct Color LIME_GREEN(0.5f, 1.f, 0.5f, .5f);
	const struct Color GREY(0.4f, 0.4f, 0.4f, 1);
};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change