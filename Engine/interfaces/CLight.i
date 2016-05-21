%{
#include "CLight.h"
	%}

class CLight : public Component
{
public:
	enum LIGHTTYPE
	{
		DIRECTIONAL,
		POINT,
		SPOTLIGHT
	};

	Color GetDiffuse() const;
	void SetDiffuse(Color val);
	float GetRange() const;
	void SetRange(float val);
	float GetSpot() const;
	void SetSpot(float val);
	float GetAtt() const;
	void SetAtt(float val);
	int GetLightType() const;
	void SetLightType(LIGHTTYPE val);

	Color mDiffuse;
	float mRange;        
	float mAtt;
	float mInnerAngle;  
	float mOuterAngle; 
	float mSpot;
	float mIntensity;
	LIGHTTYPE mLightType;
	Math::Vector3 mLightDir;
};