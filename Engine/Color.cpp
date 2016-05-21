/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 

Color::Color(float _r, float _g, float _b, float _a): r(_r),g(_g),b(_b),a(_a)
{
	//Ctor
}

Color::Color(const Vector4 &color)
{
	*this = color;
}

Color::Color(const Color& clr)
{
	*this = clr;
}

Color Color::operator + (const Color& rhs)
{
	Color ret;

	ret.r = r + rhs.r;
	ret.g = g + rhs.g;
	ret.b = b + rhs.b;
	ret.a = a + rhs.a;

	return ret;
}

Color& Color::operator += (const Color& rhs)
{
	*this = *this + rhs;

	return *this;
}

Color Color::operator - (const Color& rhs)
{
	Color ret;

	ret.r = r - rhs.r;
	ret.g = g - rhs.g;
	ret.b = b - rhs.b;
	ret.a = a - rhs.a;

	return ret;
}

Color& Color::operator -= (const Color& rhs)
{
	*this = *this + rhs;

	return *this;
}

Color Color::operator * (const float& rhs)
{
	Color col =  *this;

	col.r *= rhs;
	col.g *= rhs;
	col.b *= rhs;
	col.a *= rhs;

	return col;
}

Color& Color::operator *= (const float& rhs)
{
	*this = *this * rhs;

	return *this;
}

Color& Color::operator=(const Vector4& rhs)
{
	this->r = rhs.x;
	this->g = rhs.y;
	this->b = rhs.z;
	this->a = rhs.w;

	return *this;
}

bool Color::operator == (const Color& clr)
{
	return (r == clr.r && g == clr.g && b == clr.b && a == clr.a);
}

bool Color::operator != (const Color& clr)
{
	return !((*this) == clr);
}

/* Color Operations */
///////////////////////////////IMPLEMENNNNTTTTTTTT/////////////////////////////////
Color Color::Blend(const Color& col, float blendFactor)
{
	return *this;
}			
Color Color::AlphaBlend(const Color& col)
{
	return *this;
}

Vector4 Color::ToVector4() const
{
  Vector4 convertedColor;

	convertedColor.x = this->r;
	convertedColor.y = this->g;
	convertedColor.z = this->b;
	convertedColor.w = this->a;

	return convertedColor;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change