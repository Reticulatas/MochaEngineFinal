%{
#include "CReflectionProbe.h"
%}

class CReflectionProbe : public Component
{
	CReflectionProbe(void);
	virtual ~CReflectionProbe();
public:

	bool mStatic;
	Math::Vector3 mProbeBoxDim;
};