#include "MochaMath.h"

float Math::Cerp(float start, float end, float percent)
{
	double a = static_cast<double>(start);
	double b = static_cast<double>(end);
	double mu = static_cast<double>(percent);
	double mu2;
	mu2 = (1 - std::cos(mu*PI)) / 2;
	return static_cast<float>((a*(1 - mu2) + b*mu2));
}

float Math::Lerp(float start, float end, float percent)
{
	return (start + percent*(end - start));
}

float Math::Clamp(float num, float min, float max)
{
	if (num < min)
		num = min;
	else if (num > max)
		num = max;
	return num;
}
