/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CTransform.h"


CTransform::CTransform(void) : Component(), mParent(0)
{
  mScale.SetIdentity();
  mRotation.SetIdentity();
  mTranslation.SetIdentity();
  mWorld.SetIdentity();
}

void CTransform::OnInit( void )
{
  scale = Vector3(1, 1, 1);
  localPosition = Vector3(0, 0, 0);
  mDirection = Vector3(0, 0, 0);
}
void CTransform::OnStart(void) {
}

void CTransform::OnDraw()
{
  //Vector3 p = gameObject->transform->GetGlobalPosition();
  //Renderer::getInstance()->DrawLine(p, p + GetForwardVector(), Vector4(0, 0, 1, 1));
  //Renderer::getInstance()->DrawLine(p, p + GetRightVector(), Vector4(1, 0, 0, 1));
  //Renderer::getInstance()->DrawLine(p, p + GetUpVector(), Vector4(0, 1.f, 0, 1));
}

void CTransform::OnUpdate(void)
{
  // TRS is correct - goes into the shader in this order PV(TRS)
  UpdateWorldTransform();
}

void CTransform::UpdateWorldTransform()
{
  mWorld = GetLocalTranslationMatrix() * GetLocalRotationMatrix() * GetLocalScaleMatrix();
  mIsUpdateWorldTransform = false;
}

Matrix4 CTransform::GetWorldMatrix(void) 
{
	//if (mIsUpdateWorldTransform)
		UpdateWorldTransform();
	if (!gameObject->transform->GetParent())
	{
		return mWorld;
	}
	else
	{
		Matrix4 parentMatrix = gameObject->transform->GetParent()->GetWorldMatrix();
		return  parentMatrix * mWorld;
	}
}

void CTransform::OnFree(void) { }
 
Matrix4 CTransform::GetLocalTranslationMatrix(void) 
{
  Matrix4 trans;
  trans.Translate(localPosition);

  return trans;
}

Matrix4 CTransform::GetLocalRotationMatrix( void ) 
{
  return ToMatrix4(EulerAnglesRadian(rotation));
}

Matrix4 CTransform::GetLocalScaleMatrix(void) 
{
  Matrix4 S;
  S.Scale(scale);
  return S;
}

Matrix4 CTransform::GetGlobalTranslationMatrix( void ) 
{
  if(!gameObject->transform->GetParent())
  {
    return GetLocalTranslationMatrix();
  }
  else
  {
    //Matrix4 parentMatrix = gameObject->transform->GetParent()->GetGlobalTranslationMatrix();
    //return  parentMatrix*GetLocalTranslationMatrix();
    Matrix4 globalTrans = GetWorldMatrix();
    Matrix4 transMat; Vector3 pos;
    globalTrans.Decompose(&Vector3(), &Matrix3(), &pos);
    transMat.Translate(pos);
    return transMat;
  }
}

Matrix4 CTransform::GetGlobalRotationMatrix( void ) 
{
  if (!gameObject->transform->GetParent())
  {
    return GetLocalRotationMatrix();
  }
  else
  {
    //Matrix4 parentMatrix = gameObject->transform->GetParent()->GetGlobalRotationMatrix();
    //return  parentMatrix*GetLocalRotationMatrix();
    Matrix4 globalTrans = GetWorldMatrix();
    Matrix3 rot;
    globalTrans.Decompose(&Vector3(), &rot, &Vector3());
    return ToMatrix4(rot);
  }
}

Matrix4 CTransform::GetGlobalScaleMatrix( void ) 
{
  if(!gameObject->transform->GetParent())
  {
    return GetLocalScaleMatrix();
  }
  else
  {
    //Matrix4 parentMatrix = gameObject->transform->GetParent()->GetGlobalScaleMatrix();
    //return  parentMatrix*GetLocalScaleMatrix();
    Matrix4 globalTrans = GetWorldMatrix();
    Matrix4 scaleMat; Vector3 s;
    globalTrans.Decompose(&s, &Matrix3(), &Vector3());
    scaleMat.Scale(s);
    return scaleMat;
  }
}

Vector3 CTransform::GetWorldPosition()
{
  Vector3 worldPosition;
  Matrix4 worldTransMatrix = GetWorldMatrix();

  worldPosition.x = worldTransMatrix(0, 3);
  worldPosition.y = worldTransMatrix(1, 3);
  worldPosition.z = worldTransMatrix(2, 3);

  return worldPosition;
}

Vector3 CTransform::GetRightVector(void) 
{
  Vector3 right = ToQuaternion(GetGlobalRotationMatrix()).RotatedVector(Vector3::cXAxis);
  //Vector3 right = Transform(ToMatrix3(GetGlobalRotationMatrix()), Vector3::cXAxis);
  right.Normalize();

  return right;
}

Vector3 CTransform::GetForwardVector(void) 
{
  Vector3 forward = ToQuaternion(GetGlobalRotationMatrix()).RotatedVector(Vector3::cZAxis);
  //Vector3 forward = Transform(ToMatrix3(GetGlobalRotationMatrix()), Vector3::cZAxis);
  forward.Normalize();

  return forward;
}

Vector3 CTransform::GetUpVector(void) 
{
  Vector3 up = ToQuaternion(GetGlobalRotationMatrix()).RotatedVector(Vector3::cYAxis);
  //Vector3 up = Transform(ToMatrix3(GetGlobalRotationMatrix()), Vector3::cYAxis);
  up.Normalize();

  return up;
}

Vector3 CTransform::GetLocalPosition( void )
{
  return localPosition;
}

Vector3 CTransform::GetGlobalPosition( void )
{
  if (!gameObject->transform->GetParent())
    return localPosition;

  return GetWorldPosition();
}

CTransform::~CTransform()
{

}

void CTransform::LookAt(const Vector3* point, Vector3 worldUp)
{
  Vector3 newForward = *point - position();
  if (newForward == Vector3::cZero)
    return;

  if (newForward == worldUp || newForward == -worldUp)
	  worldUp = Vector3::cXAxis;

  Vector3 newRight = worldUp.Cross(newForward);
  Vector3 newUp = newForward.Cross(newRight);

  newForward.Normalize();
  newRight.Normalize();
  newUp.Normalize();

  Matrix3 m;
  m.SetBasis(0, newRight);
  m.SetBasis(1, newUp);
  m.SetBasis(2, newForward);

  //rotation = ToEulerAngles(m, rotation.Order.Order);
  Vector4 axisAngle = ToAxisAngle(ToQuaternion(m));
  SetRotation(ToVector3(axisAngle), RadToDeg(axisAngle.w));
}

void CTransform::Rotate(float x, float y, float z, TransformSpace space)
{
  Rotate(Vector3(x, y, z), space);
}

void CTransform::Rotate(const Vector3& vec, TransformSpace space)
{
	mIsUpdateWorldTransform = true;
  // Do calculations in radians
  EulerAngles ea(vec);
  ea = EulerAnglesRadian(ea);

  if (gameObject->transform->GetParent() && space == GLOBAL)
  {
    // Decompose to get global scale and pos
    Matrix4 globalTrans = GetWorldMatrix();
    Matrix4 transMat, scaleMat;
    Matrix3 rot; Vector3 s, pos;
    globalTrans.Decompose(&s, &rot, &pos);

    // Recompute new global transform with input euler angle
    transMat.Translate(pos);
    scaleMat.Scale(s);
    globalTrans = transMat * ToMatrix4(ea) * scaleMat;

    // Calculate local transform
    Matrix4 m = gameObject->transform->GetParent()->GetWorldMatrix().Inverted();
    m = m * globalTrans;

    m.Decompose(&Vector3(), &rot, &Vector3());
    SetRotationFromnMatrix3(rot);
  }

  else
  {
    ClampEulerAngles(ea, 2 * cPi);
    rotation = EulerAnglesDegree(ea);
  }

  UpdateWorldTransform();
}

Math::Quaternion CTransform::GetRotationQuaternion(void) 
{
  return ToQuaternion(GetGlobalRotationMatrix());
}

void CTransform::SetRotationFromQuaternion(const Quaternion& q)
{
	mIsUpdateWorldTransform = true;
  EulerAngles ea = ToEulerAngles(q, rotation.Order.Order);
  ClampEulerAngles(ea, 2 * cPi);
  rotation = EulerAnglesDegree(ea);
}

Matrix3 CTransform::GetRotationMatrix3(void) 
{
  return ToMatrix3(GetGlobalRotationMatrix());
}

void CTransform::SetRotationFromnMatrix3(const Matrix3& q)
{
  SetRotationFromQuaternion(ToQuaternion(q));
}

void CTransform::SetPosition(const Vector3& pos, TransformSpace space /*= TransformSpace::GLOBAL*/)
{
	mIsUpdateWorldTransform = true;
	if (space == TransformSpace::GLOBAL && gameObject->transform->GetParent())
	{
		Matrix4 trans;
		trans.Translate(pos);
		Matrix4 m = gameObject->transform->GetParent()->GetWorldMatrix().Inverted();
		m = m * trans * GetLocalRotationMatrix() * GetLocalScaleMatrix();

    m.Decompose(&Vector3(), &Matrix3(), &localPosition);
	}
	else
		localPosition = pos;
}

void CTransform::SetPosition(const float x, const float y, const float z, TransformSpace space /*= TransformSpace::GLOBAL*/)
{
	mIsUpdateWorldTransform = true;
	SetPosition(Math::Vector3(x, y, z), space);
}

Math::Vector3 CTransform::position()
{
	return GetPosition();
}

Math::Vector3 CTransform::GetPosition(TransformSpace space )
{
	if (space == GLOBAL)
		return GetGlobalPosition();
	return localPosition;
}

void CTransform::Translate(const Math::Vector3& v)
{
	localPosition += v;
	mIsUpdateWorldTransform = true;
}

void CTransform::Translate(const float x, const float y, const float z)
{
	Translate(Math::Vector3(x, y, z));
}

void CTransform::SetPositionZ(const float z, TransformSpace space /*= TransformSpace::GLOBAL*/)
{
	Math::Vector3 gpos = GetPosition(space);
	SetPosition(gpos.x, gpos.y, z, space);
}

void CTransform::SetPositionY(const float y, TransformSpace space /*= TransformSpace::GLOBAL*/)
{
	Math::Vector3 gpos = GetPosition(space);
	SetPosition(gpos.x, y, gpos.z, space);
}

void CTransform::SetPositionX(const float x, TransformSpace space /*= TransformSpace::GLOBAL*/)
{
	Math::Vector3 gpos = GetPosition(space);
	SetPosition(x, gpos.y, gpos.z, space);
}

void CTransform::RotateAround(const float x, const float y, const float z, const float angle, TransformSpace space)
{
  RotateAround(Vector3(x,y,z), angle, space);
}

void CTransform::RotateAround(const Vector3& vec, float angle, TransformSpace space)
{
  // Do calculations in radians
  angle = DegToRad(angle);
  Quaternion quatRot = ToQuaternion(vec.Normalized(), angle);

  if (gameObject->transform->GetParent())
  {
    // Decompose to get global scale and pos
    Matrix4 globalTrans = GetWorldMatrix();
    Matrix4 transMat, scaleMat;
    Matrix3 rot; Vector3 s, pos;
    globalTrans.Decompose(&s, &rot, &pos);

    // Recompute new global transform with input axis angle
    if (space == LOCAL)
    {
      Vector3 rotVec = Transform(rot, vec);
      quatRot = ToQuaternion(rotVec.Normalized(), angle);
    }

    quatRot *= ToQuaternion(rot);
    transMat.Translate(pos);
    scaleMat.Scale(s);
    globalTrans = transMat * ToMatrix4(quatRot) * scaleMat;

    // Calculate local transform
    Matrix4 m = gameObject->transform->GetParent()->GetWorldMatrix().Inverted();
    m = m * globalTrans;

    m.Decompose(&Vector3(), &rot, &Vector3());
    SetRotationFromnMatrix3(rot);
  }

  else
  {
    quatRot *= ToQuaternion(GetGlobalRotationMatrix());
    SetRotationFromQuaternion(quatRot);
  }

  UpdateWorldTransform();
}

void CTransform::SetRotation(const Vector3& vec, float angle, TransformSpace space)
{
  // Do calculations in radians
  angle = DegToRad(angle);
  Quaternion quatRot = ToQuaternion(vec.Normalized(), angle);

  if (gameObject->transform->GetParent() && space == GLOBAL)
  {
    // Decompose to get global scale and pos
    Matrix4 globalTrans = GetWorldMatrix();
    Matrix4 transMat, scaleMat;
    Matrix3 rot; Vector3 s, pos;
    globalTrans.Decompose(&s, &rot, &pos);

    // Recompute new global transform with input axis angle
    transMat.Translate(pos);
    scaleMat.Scale(s);
    globalTrans = transMat * ToMatrix4(quatRot) * scaleMat;

    // Calculate local transform
    Matrix4 m = gameObject->transform->GetParent()->GetWorldMatrix().Inverted();
    m = m * globalTrans;

    m.Decompose(&Vector3(), &rot, &Vector3());
    SetRotationFromnMatrix3(rot);
  }

  else
    SetRotationFromQuaternion(quatRot);

  UpdateWorldTransform();
}

void CTransform::SetRotation(const float x, const float y, const float z, float angle, TransformSpace space)
{
  SetRotation(Vector3(x, y, z), angle, space);
}

void CTransform::SetRotation(const Quaternion& q)
{
	Vector4 axisAngle = ToAxisAngle(q);
	SetRotation(ToVector3(axisAngle), RadToDeg(axisAngle.w));
}

Math::Vector3 CTransform::GetConstrainedRotation() 
{
	// For Y axis rotation angle
	Vector3 forward = GetForwardVector(); // This works only if there is no roll
	forward.y = 0.f;
	forward.Normalize();
	float cosAngle = forward.x; // forward.Dot(Vector3::cXAxis);
	float angleY = acosf(cosAngle); // [0,180]
	if (forward.z > 0.f)
		angleY *= -1.f; // [-180, 180]
	angleY += Math::cPi * 0.5f; // [-90, 270]

	// For X axis rotation angle
	Vector3 forwardForX = GetForwardVector();
	cosAngle = forwardForX.y; // forward.Dot(Vector3::cYAxis);
	float angleX = acosf(cosAngle); // [0,180] in y axis
	//if (forwardForX.z < 0.f)
	//	angleX *= -1.f; // -a
	angleX += -Math::cPi * 0.5f; // a - 90 or -a - 90, [-90, 90] (assume no x axis rotation past 90)

	// For Z axis rotation angle
	float angleZ = 0.f; // Assumes no roll
	// Vector3 right = GetRightVector();
	// cosAngle = right.y; // right.Dot(Vector3::cYAxis); // if there is y component in right vector, then there is roll
	// float angleZ = acosf(cosAngle);
	// if (right.x > 0.f)
	// 	angleZ *= -1.f; // -a
	// angleZ += Math::cPi * 0.5f; // -a + 90 or a + 90

	return Vector3(Math::RadToDeg(angleX), Math::RadToDeg(angleY), Math::RadToDeg(angleZ));
}

Math::Vector3 CTransform::GetRotation(TransformSpace space) 
{
  // Returns euler angle but no guaranteed to be unique
  if (space == GLOBAL && gameObject->transform->GetParent())
  {
    EulerAngles ea = ToEulerAngles(GetGlobalRotationMatrix(), rotation.Order.Order);
    ClampEulerAngles(ea, 2 * cPi);
    ea = EulerAnglesDegree(ea);

    return ea.Angles;
  }

  return rotation.Angles;
}

void CTransform::SetParent(CTransform* parent)
{
	if (parent == this)
	{
		Log("ERROR: Cannot set object's parent to itself : " + gameObject->GetName());
		return;
	}
	
	CTransform* prevParent = mParent;
  // Attaching to a parent
	if (parent)
	{
		parent->children.push_back(this);
		if (!Math::IsZero(parent->GetWorldMatrix().Determinant()))
		{
			Matrix4 m = parent->GetWorldMatrix().Inverted();
			m = m * GetWorldMatrix();

			Matrix3 rot;
			m.Decompose(&scale, &rot, &localPosition);

      Matrix4 localTrans = GetLocalTranslationMatrix() * GetLocalRotationMatrix() * GetLocalScaleMatrix();
      Matrix4 globalTrans = parent->GetWorldMatrix() * localTrans;

      SetRotationFromnMatrix3(rot);
		}
	}
  // Detaching from a parent
	else
	{
		Matrix3 rot;
		Matrix4 m = GetWorldMatrix();
		m.Decompose(&scale, &rot, &localPosition);

    SetRotationFromnMatrix3(rot);
	}

	if (prevParent)
		prevParent->children.erase(std::remove(prevParent->children.begin(), prevParent->children.end(), this), prevParent->children.end());

	mParent = parent;
  UpdateWorldTransform();
}

void CTransform::SetScale(const Vector3& s, TransformSpace space)
{	
  if (gameObject->transform->GetParent() && space == GLOBAL)
  {
    // Decompose to get global scale and pos
    Matrix4 globalTrans = GetWorldMatrix();
    Matrix4 transMat, scaleMat;
    Matrix3 rot; Vector3 pos;
    globalTrans.Decompose(&Vector3(), &rot, &pos);

    // Recompute new global transform with input axis angle
    transMat.Translate(pos);
    scaleMat.Scale(s);
    globalTrans = transMat * ToMatrix4(rot) * scaleMat;

    // Calculate local transform
    Matrix4 m = gameObject->transform->GetParent()->GetWorldMatrix().Inverted();
    m = m * globalTrans;

    m.Decompose(&scale, &Matrix3(), &Vector3());
  }

  else
    scale = s;

  UpdateWorldTransform();
}

void CTransform::SetScale(float x, float y, float z, TransformSpace space)
{
  SetScale(Vector3(x, y, z), space);
}

Vector3 CTransform::GetScale(TransformSpace space /*= TransformSpace::GLOBAL*/)
{
  if (space == GLOBAL && gameObject->transform->GetParent())
  {
    Matrix4 s = GetGlobalScaleMatrix();
    return Vector3(s[0][0], s[1][1], s[2][2]);
  }

  return scale;
}

CTransform* CTransform::GetChild(unsigned index)
{
	if (children.size() < index)
		return 0;
	return children.at(index);
}

CTransform* CTransform::GetParent() const
{
	return mParent;
}

unsigned CTransform::GetNumberOfChildren()
{
	return children.size();
}

Vector2 CTransform::GetScreenSpaceCoord(const CCamera* cam)
{
	Matrix4 mvp = cam->ProjView() * mWorld;
	return Vector2(mvp(0, 3), mvp(1, 3));
}

___COMP_REGISTER(CTransform);
meta_define(CTransform);
/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change