/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CPhysics.h"
#include  "PhysicsEngine.h"
#include "CTransform.h"
#include "BoxShape.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "MeshShape.h"
#include "Collidar.h"
#include "EditorSubsystem.h"
#include "EditorSubsystem_Interface.h"
#define DT 0.01666666f
#define WAKEUP_EPSLON 0.1
CPhysics::CPhysics()
	:mPosition(), mAngAccel(),mAngVelocity(0),mAngPos(),mAcceleration(),
	mPrevPosition(), mVelocity(),mAccumulatedForce(), mMass(1.0f),
	mI_Mass(0), mInertia(0), mInveInertia(0), mFriction(1),
	mMotion(1.0f), mIsActive(true), mIsControl(false), 
	mIsCollision(false), mIsSleep(false), mColliderType(0),
	mIsForceActive(false),mCollisionData(),dtScale(1.0f),
	mDampingX(0.9f), mDampingY(0.99f), mRestitution(0),
	mIsJump(false), mDampingRot(0.99f), mIsTriggered(false),
	mWidthScale(1.0f), mHeightScale(1.0f), mCollisionShapeInt(1),
	mIsJoint(false), mOffsetCenterX(0), mOffsetCenterY(0),mIsGravityOn(true),
	mIsMovingStatic(false), mCollisionID(0), rigidCom(NULL), collidarCom(NULL),
	mIsRotation(false), mGravityFactor(Vector3(0,0,0)),
	mLockOnX(false), mLockOnY(false), mLockOnZ(false)
{}

CPhysics::~CPhysics()
{
	if(mCollisionData != NULL)
		delete mCollisionData;
}

void CPhysics::OnInit(void)
{
}
// Components template
void CPhysics::OnStart(void)
{
	if (EditorSS::IsTesting() || !Engine::getInstance()->GetIsEditorMode())
	{
		CreateRigidBodyComponent(mColliderType); // static or not
		rigidCom->SetPosition(gameObject->transform->position());

	}

}
void CPhysics::OnUpdate(void)
{

}
void CPhysics::Dispose()
{
	if (rigidCom)
	{
		rigidCom->Dispose();
		delete rigidCom;
		rigidCom = 0;
	}

}
void CPhysics::OnFree(void)
{
	Dispose();
}

// End
void CPhysics::OnDraw(void)
{
	
	/*
	if (Engine::getInstance()->GetIsEditorMode() && !EditorSS::IsTesting())
	{
		if (mCollisionData == NULL)
			return;
		if (!mCollisionData->IsUpdated)
		{
			CollisionSetup();
			mPosition = ToVector4(gameObject->transform->position(), 1);
			mPosition.x += mOffsetCenterX;
			mPosition.y += mOffsetCenterY;
			mCollisionData->SetUpValue();
		}
		if (mCollisionData->id == CollisionShape::IdOBB)
		{
			OBB* obb = reinterpret_cast<OBB*>(mCollisionData);
			Point3* mPoints = obb->GetVertices();

			Renderer::getInstance()->DrawLine(ToVector3(mPoints[0]), ToVector3(mPoints[1]));
			Renderer::getInstance()->DrawLine(ToVector3(mPoints[1]), ToVector3(mPoints[2]));
			Renderer::getInstance()->DrawLine(ToVector3(mPoints[2]), ToVector3(mPoints[3]));
			Renderer::getInstance()->DrawLine(ToVector3(mPoints[3]), ToVector3(mPoints[0]));
		}
		else if (mCollisionData->id == CollisionShape::IdGeneric)
		{
			GenericShape* gs = reinterpret_cast<GenericShape*>(mCollisionData);
			Matrix world = gameObject->transform->mWorld;
			vector<Vector>::iterator iterBegin = gs->mashes.begin();
			vector<Vector>::iterator iterEnd = gs->mashes.end();
			const unsigned size = gs->mashes.size();
			Vector3* mPointflo = new Vector3[size];
			unsigned i = 0;
			for (; iterBegin != iterEnd; ++iterBegin)
			{
				mPointflo[i] = ToVector3(world * (*iterBegin));
				++i;
			}
			for (unsigned i = 0; i < size - 1; ++i)
				Renderer::getInstance()->DrawLine(mPointflo[i], mPointflo[i + 1]);
			Renderer::getInstance()->DrawLine(mPointflo[0], mPointflo[size - 1]);
		}
	}*/
	/*
	static const value_type SPHERE = 0;
	static const value_type QUAD = 1;
	static const value_type MESH = 4;
	static const value_type BOX = 3;
	static const value_type POLYHEDRON = 2;
	static const value_type DEFAULT = 255;
	*/
	
	//if (EditorSS::IsTesting() || !Engine::getInstance()->GetIsEditorMode())
		//return;
	return;
	if (gameObject->GetName() == "PlayerBoxCollider")
		return;
	if (!collidarCom || !collidarCom->mCollider )
		return;
	if (collidarCom->GetCollider().mGeometry->mType == 3)
	{
		CBoxColliderComponent* box = static_cast<CBoxColliderComponent*>(collidarCom);
		if (!box)
			return;
		auto pos = rigidCom->GetPosition();
		pos = gameObject->transform->position();

		auto hx = box->GetHalfExtents();
		DrawBox(pos, hx);
	}
	else if (collidarCom->GetCollider().mGeometry->mType == 0)
	{
		// Draw Sphere
	}
	else if (collidarCom->GetCollider().mGeometry->mType == 4)
	{
		// Draw Mesh
		MeshColliderGeometry3D* drawPt = dynamic_cast<MeshColliderGeometry3D*>(collidarCom->GetCollider().mGeometry);
		std::string child = "Children1";
		//if (gameObject->GetName().compare(0, 9, child) == 0)
		Color c(1, 0, 0);
		//drawPt->DrawMesh(c);
			//drawPt->DrawMesh(PhysicEngine::getInstance()->mDebugColorManager.GetColor());
	}
}

void CPhysics::Integrate(float dt)
{
	GetPosFromTransform();
	if(!mIsActive)
		return;
	MassSetUp();
	CollisionSetup();
	if (mColliderType != 0 && !mIsMovingStatic)
	{
		mIsSleep = true;
		return;
	}
	UpdateAcceleration();
	UpdateVelocity(dt);
	UpdatePosition(dt);
	mAccumulatedForce.ZeroOut();
	mIsMovingStatic = false;
}

void CPhysics::GetPosFromTransform()
{

	if(ToVector3(mPosition) != (gameObject->transform->position()))
	{
		mPrevPosition = ToVector4(gameObject->transform->position(), 1);
		//mPreRotation  = gameObject->transform->rotation.z;
		mIsSleep = false;
	}

	else
	{
		mPrevPosition = mPosition;
		mPreRotation  = mRotation;
	}

	mPrevPosition.x += mOffsetCenterX;
	mPrevPosition.y += mOffsetCenterY;

  mPosition = ToVector4(gameObject->transform->position(),1);
	mPosition.x += mOffsetCenterX;
	mPosition.y += mOffsetCenterY;

	//mRotation = gameObject->transform->rotation.z;
}

void CPhysics::MassSetUp()
{
	if(mMass != 0)
	{
		Vector3 scale = gameObject->transform->scale;
		float width = scale.x;
		float height = scale.y;
		mColliderType = 0;
		mI_Mass = 1 / mMass;

		/*
		mInertia = width * height * mMass * 12;
		mInveInertia = 1 / mInertia;
		*/

		if(mInertia != 0)
			mInveInertia = 1 / mInertia;
		else
			mInveInertia = 0;
	}
	else
	{
		if(!mIsMovingStatic)
		{
			mAccumulatedForce.ZeroOut();
			mVelocity = mPosition - mPrevPosition;
		}
		mColliderType = 1;
		mInveInertia = 0;
		mInertia = 0;
		mI_Mass = 0;
		
	}
}

void CPhysics::CollisionSetup()
{
	CollisionShape::ShapeType id;
	switch(mCollisionShapeInt)
	{
		case 0 : id = CollisionShape::ShapeType::IdNone; mIsCollision = false;
		break;
		case 1 : id = CollisionShape::ShapeType::IdOBB;
		break;
		case 2 : id = CollisionShape::ShapeType::IdCircle;
		break;
		case 3 : id = CollisionShape::ShapeType::IdGeneric;
		break;
	}
	if(mCollisionData != NULL)
	{
		if(mCollisionData->id == mCollisionShapeInt)
			mCollisionData->SetUpValue();
		else
		{
			delete mCollisionData;
			mCollisionData = NULL;
			mIsCollision = false;
			if(id != CollisionShape::IdNone)
				AddCollision(id);
		}
	}
	else
	{
		if(id != CollisionShape::IdNone)
			AddCollision(id);
	}
	if(mCollisionData != NULL)
		mCollisionData->SetIsUpdated(true);
}

void CPhysics::UpdateAcceleration()
{
	if (mColliderType != 0 && !mIsMovingStatic || mIsJoint)
		mAcceleration.ZeroOut();
	else
	{
		if(mIsMovingStatic)
			mAcceleration = mAccumulatedForce * 0.1f;
		else
			mAcceleration = mAccumulatedForce * mI_Mass;

		//if(mIsGravityOn)
			//mAcceleration += PhysicEngine::getInstance()->mGravity;
	}
}

void CPhysics::UpdateVelocity(float dt)
{
	if(!mIsSleep)
	{
		mVelocity = mVelocity  + mAcceleration * DT * dtScale;
		mVelocity.x *= mDampingX;
		mVelocity.y *= mDampingY;
		mAngVelocity *= mDampingRot;
	}
}

void CPhysics::UpdatePosition(float dt)
{
	/*
	float Wake_Up_Eplson = 0.2f;
	// sleep
	if(mIsSleep)
	{
		float vel = mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y;
		float totalMotion = vel + mAngVelocity * mAngVelocity;
		float bias = powf(float(0.01f), DT * dtScale);
		mMotion = bias * mMotion + (1 - bias) * totalMotion;
		if(totalMotion > Wake_Up_Eplson)
			mIsSleep = false;
		else
			return;
	}
	*/
   
	mPosition = mPrevPosition + mVelocity * DT * dtScale;
	mPreRotation = mRotation;
	mRotation = mPreRotation + mAngVelocity * DT * dtScale;

	/*
	// sleep
	float vel = mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y;
	float totalMotion = vel+ mAngVelocity * mAngVelocity;
	float bias = powf(float(0.0001f), DT * dtScale);
	mMotion = bias * mMotion + (1 - bias) * totalMotion;
	if(mMotion < Wake_Up_Eplson)
		mIsSleep = true;
	else if(mMotion > float(10.0f) *  Wake_Up_Eplson)
		mMotion = float(10.0f) *  Wake_Up_Eplson;
	*/
}

void CPhysics::UpdatePositionToGraphics()
{
	mPosition.x -= mOffsetCenterX;
	mPosition.y -= mOffsetCenterY;
	gameObject->transform->SetPosition(ToVector3(mPosition));
	//gameObject->transform->rotation.z = mRotation;
}

void CPhysics::UpdateLocalToGlobalMatrix(Matrix& world)
{
	world = gameObject->transform->mWorld;
}

void CPhysics::UpdateGlobalToLocalMatrix(Matrix& invWorld)
{
  invWorld = gameObject->transform->mWorld.Inverted();
}

Point3& CPhysics::LocalToGlobalPt(Point3& pt)
{
	static Matrix World;
	UpdateLocalToGlobalMatrix(World);
	return World * pt;
}

Point3& CPhysics::GlobalToLocalPt(Point3& pt)
{
	static Matrix invWorld;
	UpdateGlobalToLocalMatrix(invWorld);
    return invWorld * pt;
}

void CPhysics::ApplyImpulse(Vector& impulseFloat, Vector& contactVecFloat)
{
	static Vector impulse;
	impulse = impulseFloat; 
	static Vector contactVec;
	contactVec = contactVecFloat;

	float torqueImpulse = contactVec.x *  impulse.y - contactVec.y *  impulse.x;
	//test
	mVelocity += impulse * mI_Mass;
	mAngVelocity += mInveInertia * torqueImpulse ;

}
// Setters //
void CPhysics::SetUpDebugDrawEditorMode()
{
	if(!mCollisionData)
		return;
	mCollisionData->SetIsUpdated(false);
}
void CPhysics::ClearAcceYtoZero()
{
	mAcceleration.y = 0;
}
void CPhysics::SetVelocity(float x, float y, float z)
{
	mVelocity.x = x;
	mVelocity.y = y;
	mVelocity.z = z;
	mIsMovingStatic = true;
}
void CPhysics::SetIsSleep(bool switcher)
{
	mIsSleep = switcher;
}
void CPhysics::SetIsForceActive(bool switcher)
{
	mIsForceActive = switcher;
}
void CPhysics::SetAnguVelocity(float angu)
{
	mAngVelocity = angu;
}
void CPhysics::SetRestitution(float res)
{
	mRestitution = res;
}
void CPhysics::SetInertia(float inertia)
{
	if(mMass != 0)
	{
		if (inertia <= 0)
			mInertia = 0;
		else 
			mInertia = inertia;

		if(inertia > 0)
			mInveInertia = 1 / mInertia;
		else
			mInveInertia = 0.0f;
	}
}
void CPhysics::SetMass(float mass)
{
	if (mass <= 0)
	{
		mMass = 0;
		mIsSleep = true;
	}

	else 
	{
		mColliderType = 0;
		mMass = mass;
	}

	if(mMass <= 0)
		mI_Mass = 0.0f;

	else
		mI_Mass = 1 / mass;

}

void CPhysics::AddForce(Vector force)
{
	mIsSleep = false;
	mAccumulatedForce += force;
	mIsMovingStatic = true;
}
void CPhysics::SetForceToZero()
{
	mAccumulatedForce.ZeroOut(); 
}
void CPhysics::AddForce(float x, float y, float z)
{
	Vector force;
	force.x = x;
	force.y = y;
	force.z = z;
	AddForce(force);
}
void CPhysics::SetPosition(Point3 const& position)
{
	mPosition = position;
	gameObject->transform->SetPosition(ToVector3(position));
}
void CPhysics::SetVelocity(Vector const& velocity)
{
	mVelocity = velocity;
	Vector3 vel(velocity.x, velocity.y, velocity.z);
	rigidCom->Body().SetLinearVelocity(vel);
}
void CPhysics::SetFriction(float friction)
{
	if(friction <= 0)
		mFriction = 0;
	else
		mFriction = friction;
}
void CPhysics::SetStatus(bool switcher)
{
	mIsActive = switcher;
}

void CPhysics::AddCollision(CollisionShape::ShapeType Id)
{
	mIsCollision = true;
	switch(Id)
	{
		case CollisionShape::ShapeType::IdCircle :
		mCollisionData = new Circle(this);
		mCollisionData->SetUpValue();
		break;

		case CollisionShape::ShapeType::IdBox :
		mCollisionData = new Box(this);
		mCollisionData->SetUpValue();
		break;

		case CollisionShape::ShapeType::IdOBB :
		mCollisionData = new OBB(this);
		mCollisionData->SetUpValue();
		break;

		case CollisionShape::ShapeType::IdGeneric :
		mCollisionData = new GenericShape(this);
		mCollisionData->SetUpValue();
		break;
	}
}
void CPhysics::SetIsControl(bool switcher)
{
	mIsControl = switcher;
}
void CPhysics::SetRotation(float val) 
{
	mRotation = val;
}
// Getters
const float CPhysics::GetOffSetX() const
{
	return mOffsetCenterX;
}
const float CPhysics::GetOffSetY() const
{
	return mOffsetCenterY;
}
const int CPhysics::GetCOllisionID() const
{
	return mCollisionID;
}
const bool CPhysics::GetIsStatic() const
{
	return 0;
}
const bool CPhysics::GetIsCollision()const
{
	return mIsCollision;
}
const float& CPhysics::GetInvInertia() const
{
	return mInveInertia;
}
const float& CPhysics::GetInertia() const
{
	return mInertia;
}
const float& CPhysics::GetAngVelocity() const
{
	return mAngVelocity;
}
const Vector& CPhysics::GetAcceleration() const
{
	return mAccumulatedForce * mI_Mass;
}
const Vector& CPhysics::GetVelocity() const
{
	return mVelocity;
}
const Point3& CPhysics::GetPosition() const
{
	return mPosition;
}
const Point3& CPhysics::GetPrevPosition() const
{
	return mPrevPosition;
}
const Vector& CPhysics::GetForce() const
{
	return mAccumulatedForce;
}
const float CPhysics::GetMass() const
{
	return mMass;
}
const bool CPhysics::GetIsSleep()const
{
	return mIsSleep;
}
const float CPhysics::GetInverseMass() const
{
	return mI_Mass;
}
const float CPhysics::GetFriction() const
{
	return mFriction;
}
const bool CPhysics::GetStatus() const
{
	return mIsActive;
}
const bool CPhysics::GetIsControl() const
{
	return mIsControl;
}
CollisionShape* CPhysics::GetCollisionShape() const
{
	return mCollisionData;
}

const float CPhysics::GetRotation() const
{
	return mRotation;
}
const float CPhysics::GetRestitution() const
{
	return mRestitution;
}

void CPhysics::CreateRigidBodyComponent(unsigned char bodyType)
{
	rigidCom = CreateRigidBody(bodyType);
	rigidCom->Body().mParent = rigidCom;
	rigidCom->cphy = this;
}

void CPhysics::CreateBoxColliderComponent(const Vector3& halfextend)
{
	collidarCom = CreateBoxCollider(halfextend);
	collidarCom->mCphy = this;
}
void CPhysics::CreateSphereColliderComponent(const float radius)
{
	collidarCom = CreateSphereCollider(radius);
	collidarCom->mCphy = this;
}

void CPhysics::CreateMeshColliderComponent(const std::string& filename)
{
	collidarCom = CreateMeshCollider(filename);
	collidarCom->mCphy = this;
}


void CPhysics::DrawBox(Vector3& pos,Vector3& hx)
{
	//if (rigidCom->GetType() == RigidBody3D::DYNAMIC)
	//printf("Position : (%f , %f, %f)\n", gameObject->transform->position.x, gameObject->transform->position.y, gameObject->transform->position.z);
	//DebugDrawer::AddLine(Vector3(0, 3.51747632f, 0), Vector3(0, 0, 0), Color(1, 0, 0));
	//DebugDrawer::AddLine(Vector3(5,9,60), Vector3(0, 0, 0), Color(1, 0, 0));

	//Matrix3 rot;
	//rot = ToMatrix3(gameObject->transform->GetLocalRotationMatrix());
	////rot.Transpose();
	//Vector3 axisX(1, 0, 0), axisY(0, 1, 0), axisZ(0, 0, 1);
	//
	//// scale
	//axisX *= hx.x; axisY *= hx.y; axisZ *= hx.z;
	//
	//// rotation
	//axisX = rot * axisX; axisY = rot * axisY; axisZ = rot * axisZ;
	//
	//
	//Vector3 p1, p2, p3, p4, p5, p6, p7, p8;
	//
	//p1 = pos - axisX - axisY - axisZ + Vector3(0,hx.y,0);
	//p2 = pos + axisX - axisY - axisZ + Vector3(0, hx.y, 0);
	//p3 = pos - axisX - axisY + axisZ + Vector3(0, hx.y, 0);
	//p4 = pos + axisX - axisY + axisZ + Vector3(0, hx.y, 0);
	//p5 = pos - axisX + axisY - axisZ + Vector3(0, hx.y, 0);
	//p6 = pos + axisX + axisY - axisZ + Vector3(0, hx.y, 0);
	//p7 = pos - axisX + axisY + axisZ + Vector3(0, hx.y, 0);
	//p8 = pos + axisX + axisY + axisZ + Vector3(0, hx.y, 0);

	Matrix4 rot, scale, world;
	rot = gameObject->transform->GetLocalRotationMatrix();
	scale.Scale(hx.x, hx.y, hx.z);

	world = rot * scale;

	Vector3 axisX = ToVector3(world.Basis(0));
	Vector3 axisY = ToVector3(world.Basis(1));
	Vector3 axisZ = ToVector3(world.Basis(2));

	Vector3 p1, p2, p3, p4, p5, p6, p7, p8;

	p1 = pos - axisX - axisY - axisZ;// +Vector3(0, hx.y, 0);
	p2 = pos + axisX - axisY - axisZ;// +Vector3(0, hx.y, 0);
	p3 = pos - axisX - axisY + axisZ;// +Vector3(0, hx.y, 0);
	p4 = pos + axisX - axisY + axisZ;// +Vector3(0, hx.y, 0);
	p5 = pos - axisX + axisY - axisZ;// +Vector3(0, hx.y, 0);
	p6 = pos + axisX + axisY - axisZ;// +Vector3(0, hx.y, 0);
	p7 = pos - axisX + axisY + axisZ;// +Vector3(0, hx.y, 0);
	p8 = pos + axisX + axisY + axisZ;// +Vector3(0, hx.y, 0);
	
	Renderer::getInstance()->DrawLine(p1, p2, Color(1,1,1));
	Renderer::getInstance()->DrawLine(p1, p3, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p1, p5, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p7, p5, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p7, p3, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p7, p8, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p4, p2, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p4, p3, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p4, p8, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p6, p5, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p6, p8, Color(1, 1, 1));
	Renderer::getInstance()->DrawLine(p6, p2, Color(1, 1, 1));
}



// Helper functions //
void CovarianceMatrix(Matrix4 &cov, Vector3 pt[], int numPts)
{
	float oon = 1.0f / (float)numPts;
	Vector3 c(0.0f, 0.0f, 0.0f);
	float e00, e11, e22, e01, e02, e12;
	// Compute the center of mass (centroid) of the points
	for(int i = 0;i<numPts; i++)
		c += pt[i];
	c *= oon;
	// Compute covariance elements
	e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;
	for(int i = 0;i < numPts; i++) 
	{
		// Translate points so center of mass is at origin
		Vector3 p = pt[i] - c;
		// Compute covariance of translated points

    e00 += p.x * p.x;
    e11 += p.y * p.y;
    e22 += p.z * p.z;
    e01 += p.x * p.y;
    e02 += p.x * p.z;
    e12 += p.y * p.z;
	}
	// Fill in the covariance matrix elements
	cov(0,0) = e00 * oon;
	cov(1,1) = e11 * oon;
	cov(2,2) = e22 * oon;
	cov(0,1) = cov(1,0) = e01 * oon;
	cov(0,2) = cov(2,0) = e02 * oon;
	cov(1,2) = cov(2,1) = e12 * oon;
}

void SymSchur2(Matrix4 &a, int p, int q, float &c, float &s)
{
	if (abs(a(p,q)) > 0.0001f) 
	{
		float r = (a(q,q) - a(p,p)) / (2.0f * a(p,q));
		float t;
		if (r >= 0.0f)
			t = 1.0f / (r + sqrt(1.0f + r*r));
		else
			t = -1.0f / (-r + sqrt(1.0f + r*r));
		c = 1.0f / sqrt(1.0f + t*t);
		s = t*c;
	} 
	else 
	{
		c = 1.0f;
		s = 0.0f;
	}
}

void Jacobi(Matrix4 &a, Matrix4 &v)
{
	int i, j, n, p, q;
	float prevoff, c, s;
	Matrix4 J, b, t;
	// Initialize v to identify matrix
	for(i=0;i<3;i++) 
	{
		v(i,0) = v(i,1) = v(i,2) = 0.0f;
		v(i,i) = 1.0f;
	}

	// Repeat for some maximum number of iterations
	const int MAX_ITERATIONS = 50;
	for (n = 0; n < MAX_ITERATIONS; n++) 
	{
		// Find largest off-diagonal absolute element a[p][q]
		p=0;q=1;
		for(i=0;i<3;i++) 
		{
			for(j=0;j<3;j++) 
			{
				if (i == j) continue;
				if (abs(a(i,j)) > abs(a(p,q))) 
				{
					p=i;
					q=j;
				}
			}
		}
		// Compute the Jacobi rotation matrix J(p, q, theta)
		// (This code can be optimized for the three different cases of rotation)
		SymSchur2(a, p, q, c, s);
		for(i=0;i<3;i++) 
		{
			J(i,0) = J(i,1) = J(i,2) = 0.0f;
			J(i,i) = 1.0f;
		}
		J(p,p) = c; J(p,q) = s;
		J(q,p) = -s; J(q,q) = c;
		// Cumulate rotations into what will contain the eigenvectors
		v=v*J;
		// Make ¡¦a¡¦ more diagonal, until just eigenvalues remain on diagonal
		a = (J.Inverted() * a) * J;
		// Compute "norm" of off-diagonal elements
		float off = 0.0f;
		for(i=0;i<3;i++) 
		{
			for(j=0;j<3;j++) 
			{
				if (i == j) continue;
				off += a(i,j) * a(i,j);
			}
		}
		/* off = sqrt(off); not needed for norm comparison */
		// Stop when norm no longer decreasing
		if(n>2 && off>=prevoff)
			return;
		prevoff = off;
	}
}



___COMP_REGISTER(CPhysics);

meta_define(CPhysics);

	/////////////////////////////////////
	//Original Author: Kai Chuan Hsiao
	//Modifications:
	//	Date	-	Change
	//  13/5/13  -   CPhysics Class