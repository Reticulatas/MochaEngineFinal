%{
#include "CShadowProbe.h"
%}

class CShadowProbe : public Component
{
	CShadowProbe(void);
	virtual ~CShadowProbe();
public:

	bool mStatic;
	float mStrength;
	float mBias;
	bool mUseSoftShadows;
	float mNearZ;
	float mFarZ;
};