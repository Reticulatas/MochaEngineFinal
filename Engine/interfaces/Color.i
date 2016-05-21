%{
#include "Color.h"
	%}

%typemap(cscode) Color %{
	public override string ToString()
	{
		return "(" + r.ToString() + ", " + g.ToString() + ", " 
			+ b.ToString() + ", " + a.ToString() + ")";
	}
%}

struct Color {

	float r, g, b, a;

	Color(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _a = 1.0f);
	Color(const Vector4 &color);
	Color(const Color& clr);

	%rename(Mul) operator*;
	%rename(MulAssign) operator*=;
	%rename(Comp) operator==;
	%rename(NegComp) operator!=;

	Color operator * (const float& rhs);
	Color& operator *= (const float& rhs);
	bool operator == (const Color& clr);
	bool operator != (const Color& clr);
};

/* Common colors */
namespace COLOR
{
	const struct Color WHITE;
	const struct Color BLACK;
	const struct Color RED;
	const struct Color GREEN;
	const struct Color BLUE;
	const struct Color YELLOW;
	const struct Color SILVER;
	const struct Color LIGHT_STEEL_BLUE;
	const struct Color CORNFLOWER_BLUE;
  const struct Color LIME_GREEN;
  const struct Color GREY;
};
