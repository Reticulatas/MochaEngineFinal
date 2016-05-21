
%{
#include "CFilmGrain.h"
%}


class CFilmGrain : public CPostProcess
{
public:
	float mBarHeight;
	float mBarSpeed;
	float mBarOverflow;
	float mNoiseDensity;
	float mPixelDensity;
   private:
    CFilmGrain();
	~CFilmGrain();
};
