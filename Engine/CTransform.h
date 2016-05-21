/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once

#include "Component.h"
#include "..\MochaMath\Math\Quat.h"
class CTransform : public Component
{
public:
	CTransform(void);
	~CTransform();

	enum TransformSpace {
		LOCAL,
		GLOBAL
	};

	void OnFree(void);
	void OnStart(void);
	void OnInit(void);
	void OnUpdate(void);
	void OnDraw();

  void UpdateWorldTransform();

	/* Gettor for world transform */
  Vector3 GetGlobalPosition();
	Vector3 GetLocalPosition();
	Vector2 GetScreenSpaceCoord(const CCamera* cam);

	Matrix4 GetWorldMatrix(void) ;
  Matrix4 GetLocalTranslationMatrix(void) ;
  Matrix4 GetLocalRotationMatrix(void) ;
  Matrix4 GetLocalScaleMatrix(void) ;

	/*! Returns an Vector3 that represents the direction this object is facing */
	Vector3 GetForwardVector(void) ;
	/*! Returns an Vector3 that represents the up direction for this object */
	Vector3 GetUpVector(void) ;
	/*! Returns an Vector3 that represents the right direction for this object */
	Vector3 GetRightVector(void) ;

	Matrix4 GetGlobalTranslationMatrix(void) ;
	Matrix4 GetGlobalRotationMatrix(void) ;
	Matrix4 GetGlobalScaleMatrix(void) ;

	void LookAt(const Vector3* point, Vector3 worldUp = Vector3::cYAxis);
	Vector3 GetConstrainedRotation() ; // Based on global forward vector. Assumes 0 roll (rotation about z axis), and [-90,90] about x axis
  // Return euler angles - CAREFUL: not guaranteed to be a unique angle
	Vector3 GetRotation(TransformSpace space = TransformSpace::GLOBAL) ;
  // Internal Rotation usage
	Quaternion GetRotationQuaternion(void) ;
  Matrix3 GetRotationMatrix3(void) ;
  void SetRotationFromQuaternion(const Quaternion& q);
  void SetRotationFromnMatrix3(const Matrix3& q);
  void SetRotation(const Quaternion& q);

  // Exposed to scripts
  // Sets rotation once given an axis angle pair
  void SetRotation(const Vector3& rot, float angle, TransformSpace space = TransformSpace::GLOBAL);
  void SetRotation(const float x, const float y, const float z, float angle, TransformSpace space = TransformSpace::GLOBAL);
  // Sets rotation similar to euler angles
  void Rotate(float x, float y, float z, TransformSpace space = TransformSpace::GLOBAL);
  void Rotate(const Vector3& vec, TransformSpace space = TransformSpace::GLOBAL);
  // Concatenates rotations given an axis angle pair
  void RotateAround(const float x, const float y, const float z, const float angle, TransformSpace space = TransformSpace::GLOBAL);
  void RotateAround(const Vector3& vec, float angle, TransformSpace space = TransformSpace::GLOBAL);

	//local position if parent, global otherwise - always the only position
	Vector3 localPosition;
	//position cannot be set directly anymore
	void SetPosition(const Vector3& pos, TransformSpace space = TransformSpace::GLOBAL);
	void SetPosition(const float x, const float y, const float z, TransformSpace space = TransformSpace::GLOBAL);
	void SetPositionX(const float x, TransformSpace space = TransformSpace::GLOBAL);
	void SetPositionY(const float y, TransformSpace space = TransformSpace::GLOBAL);
	void SetPositionZ(const float z, TransformSpace space = TransformSpace::GLOBAL);
	void Translate(const float x, const float y, const float z);
	void Translate(const Math::Vector3& v);
	//position cannot be set directly anymore
	Vector3 GetPosition(TransformSpace space = GLOBAL);
	Vector3 position();

  void SetScale(const Vector3& scale, TransformSpace space = TransformSpace::GLOBAL);
  void SetScale(float x, float y, float z, TransformSpace space = TransformSpace::GLOBAL);
  Vector3 GetScale(TransformSpace space = TransformSpace::GLOBAL);

	//parenting
	std::vector<CTransform*> children;
	unsigned	GetNumberOfChildren();
	CTransform* GetChild(unsigned index);
	void		SetParent(CTransform* parent);
	CTransform* GetParent() const;

	Vector3 scale;
	EulerAngles rotation;

	virtual void CopyFrom(const ::CTransform* c)
	{
		//position = c->position;
		localPosition = c->localPosition;
		scale = c->scale;
		rotation = c->rotation;
		mDirection = c->mDirection;
		mScale = c->mScale;
		mRotation = c->mRotation;
		mWorld = c->mWorld;
		//DO NOT COPY THESE
		//mParent = c->mParent;
		//children = c->children;
		Component::CopyFrom(c);
	}
	COMPONENTCLONE(CTransform);

	Vector3	mDirection;
	/* Transformations for Graphics Engine */
	Matrix4	mScale;
	Matrix4	mRotation;
	Matrix4	mTranslation;
	Matrix4	mWorld;
	bool mIsUpdateWorldTransform;

	CTransform	*mParent;			//object parent

	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		Vector3 position = localPosition;
	___DEFSER(position, 1)
		localPosition = position;
  ___DEFSER(scale, 1)
    ___DEFSER(rotation, 1)
    ___DEFSER(mDirection, 1)
    ___DEFSER(mScale, 1)
    ___DEFSER(mRotation, 1)
    ___DEFSER(mWorld, 1)
    ___DEFSER(children, 2)
    ___DEFSER(mParent, 2)
    if (ver < 3)
      rotation = EulerAnglesDegree(rotation);
		___SERIALIZE_END___

		metadef(CTransform)
		m_addbase(Component)
		m_tag(MINTBOARDTAG)
		endmetadef;

private:
	Vector3 GetWorldPosition();

};

___SERIALIZE_CLASS(CTransform, 3);

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
