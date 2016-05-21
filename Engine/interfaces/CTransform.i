
%{
#include "CTransform.h"
%}

%typemap(cscode) CTransform %{
	[MochaDoNotShow]
  public MochaVec3 position {
    get {
		return GetPosition(TransformSpace.GLOBAL);
    }
  }
%}

%csattributes CTransform::rotation "[MochaHint(\"RadToDegConverter\")]"
%template(CTransformVector)std::vector<CTransform*>;
%csattributes CTransform::children "[MochaDoNotShow]"

class CTransform : public Component
{
public:
	enum TransformSpace {
		LOCAL,
		GLOBAL
	};

	Math::Vector3 localPosition;
	Math::Vector3 scale;
	Math::EulerAngles rotation;

	Math::Vector3 GetForwardVector(void) ;
	Math::Vector3 GetUpVector(void) ;
	Math::Vector3 GetRightVector(void) ;
	Math::Vector3 GetGlobalPosition(void);
	void LookAt(const Math::Vector3* point, Math::Vector3 worldUp = Math::Vector3::cYAxis);
	
	// Rotation
	void Rotate(float x, float y, float z, TransformSpace space = TransformSpace::GLOBAL);
	void Rotate(const Math::Vector3& vec, TransformSpace space = TransformSpace::GLOBAL);
	void SetRotation(const Math::Vector3& rot, float angle, TransformSpace space = TransformSpace::GLOBAL);
	void SetRotation(const float x, const float y, const float z, float angle, TransformSpace space = TransformSpace::GLOBAL);
	Math::Vector3 GetConstrainedRotation() ; // Based on global forward vector. Assumes 0 roll (rotation about z axis), and [-90,90] about x axis
	Math::Vector3 GetRotation(TransformSpace space = TransformSpace::GLOBAL) ;
	void RotateAround(const float x, const float y, const float z, const float angle, TransformSpace space = TransformSpace::GLOBAL);
	void RotateAround(const Math::Vector3& vec, float angle, TransformSpace space = TransformSpace::GLOBAL);

	// Translation
	Math::Vector3 GetPosition(TransformSpace space);
	void SetPosition(const Math::Vector3& pos, TransformSpace space = TransformSpace::GLOBAL);
	void SetPosition(const float x, const float y, const float z, TransformSpace space = TransformSpace::GLOBAL);
	void SetPositionX(const float x, TransformSpace space = TransformSpace::GLOBAL);
	void SetPositionY(const float y, TransformSpace space = TransformSpace::GLOBAL);
	void SetPositionZ(const float z, TransformSpace space = TransformSpace::GLOBAL);
	void Translate(const float x, const float y, const float z);
	void Translate(const Math::Vector3& v);

	// Scale
	void SetScale(const Math::Vector3& scale, TransformSpace space = TransformSpace::GLOBAL);
	void SetScale(float x, float y, float z, TransformSpace space = TransformSpace::GLOBAL);
	Math::Vector3 GetScale(TransformSpace space = TransformSpace::GLOBAL);

	std::vector<CTransform*> children;
	unsigned	GetNumberOfChildren();
	CTransform* GetChild(unsigned index);
	void		SetParent(CTransform* parent);
	CTransform* GetParent() const;
};