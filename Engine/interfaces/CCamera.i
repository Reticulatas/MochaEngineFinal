
%{
#include "CCamera.h"
	%}

class CCamera : public Component
{
public:

  enum CamType
  {
    MAIN,
    VR,
    GENERIC
  };

	enum CamProj
	{
		PERSP,
		ORTHO,
		UNASSIGNED
	};

	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	float GetNearWindowWidth()  const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	Math::Vector3 ScreenToWorldPosition(int x, int y);

  CamType GetCamType();

	float FoVmodifier() const;
	void FoVmodifier(float val);

	void Strafe(float d);
	void Walk(float d);
	void Pitch(float angle);
	void RotateY(float angle);

  void SetRenderTarget(std::string name);

  CamType mCamType;
	CamProj mActiveProj;
	float mFoVmodifier;
	bool DispatchClickMessages;
  AssetHandle mCameraRT;
  float mNearZ;				
  float mFarZ;				
};