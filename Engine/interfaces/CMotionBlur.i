
%{
#include "CMotionBlur.h"
%}


class CMotionBlur : public CPostProcess
{
public:

    int mBlurFactor;
   private:
    CMotionBlur();
	~CMotionBlur();
};
