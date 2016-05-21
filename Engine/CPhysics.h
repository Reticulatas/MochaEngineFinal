/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
   -* Physics Engine Kai
	-* See Footer for Revisions                                      */


#pragma once
#include "Component.h"
#include "Collision.h"
#include "CollisionShape.h"
#include "MathIncludes.h"
#include "def.h"

#include "RigidBodyCom.h"
#include "ColliderCom.h"
#include "BoxColliderComponent.h"
#include "SupportFunction.h"
#include "PolyheShape.h"
//#include "PSetters.h"  macro
 
class CPhysics : public Component
{
public:

	CPhysics();
	virtual ~CPhysics();
	virtual void OnStart(void);
	virtual void OnUpdate(void);  
	virtual void OnFree(void);
	virtual void OnInit(void);
	virtual void OnDraw(void);
	void Integrate(float dt);
	void UpdatePositionToGraphics();
	void UpdatePosition(float dt);
	void UpdateVelocity(float dt);
	void UpdateAcceleration();
	void GetPosFromTransform();
	void AddForce(Vector force);
	void AddCollision(CollisionShape::ShapeType i);

	// Setters
	void SetVelocity(Vector const& velocity);
	void SetAnguVelocity(float angu);
	void SetPosition(Point3 const& position);
	void SetStatus(bool switcher);
	void SetIsControl(bool switcher);
	void SetIsForceActive(bool switcher);
	void SetUpDebugDrawEditorMode();
	
	// Getters
	const int   GetCOllisionID() const;
	const bool  GetIsStatic() const;
	const float GetOffSetX() const;
	const float GetOffSetY() const;
	const Vector& GetAcceleration() const;
	const Vector& GetVelocity() const;
	const Point3& GetPosition() const;
	const Point3& GetPrevPosition() const;
	const Vector& GetForce() const;

	// Helper
	void ApplyImpulse(Vector& impulse, Vector& contactVec);
	void CollisionSetup();
	void MassSetUp();
	Point3& GlobalToLocalPt(Point3& pt);
	Point3& LocalToGlobalPt(Point3& pt);
	void UpdateLocalToGlobalMatrix(Matrix& world);
	void UpdateGlobalToLocalMatrix(Matrix& invWorld);
	CollisionShape* GetCollisionShape() const;
	void DrawBox(Vector3& pos, Vector3& halfEx);
	void Dispose();
	// new stuff
	__forceinline const Math::Vector3 LocalToGlobal(const Math::Vector3 &vec) const {
		Math::Vector3 ret, temp;
		temp = mOrientation * vec;
		ret.x = mPosition.x + temp.x;
		ret.y = mPosition.y + temp.y;
		ret.z = mPosition.z + temp.z;
		return ret; }
	__forceinline const Math::Vector3 GlobalToLocal(const Math::Vector3 &vec) const {
		Math::Vector3 ret;
		ret.x = vec.x - mPosition.x;
		ret.y = vec.y - mPosition.y;
		ret.z = vec.z - mPosition.z;

		return mIverseOrientation * ret;
	}
	__forceinline const Math::Vector3 LocalToGlobalVec(const Math::Vector3 &vec) const { return mOrientation * vec; }
	__forceinline const Math::Vector3 GlobalToLocalVec(const Math::Vector3 &vec) const { return mIverseOrientation * vec; }
	__forceinline const Math::Vector3 &LocalCentroid(void) const { return mLocalCentroid; }
	__forceinline const Math::Vector3 &GlobalCentroid(void) const { return mGlobalCentroid; }

	// Binding functions
	/*! \brief Sets the mass of this object */
	void SetMass(float mass);
	/*! \brief Sets the inertia of this object */
	void SetInertia(float mass);
	/*! \brief Sets the friction of this object when colliding */
	void SetFriction(float friction);
	/*! \brief Sets the restitution (bounciness) of this object when colliding */
	void SetRestitution(float res);
	/*! \brief Adds an impulse force to the object */
	void AddForce(float x, float y, float z);
	/*! \brief Force set the rotation on the 2D plane (radians */
	void SetRotation(float val);
	/*! \brief Force the object to sleep (deactivates physics until a new force is applied) */
	void SetIsSleep(bool switcher);
	/*! \brief Force the object to stop moving and remove accumulated forces */
	void SetForceToZero();
	/*! \brief Get the current angular velocity of this object */
	const float& GetAngVelocity() const;
	/*! \brief Get the current inertia coefficient of this object */
	const float& GetInertia() const;
	/*! \brief Get the inverse of the current inertia coefficient of this object */
	const float& GetInvInertia() const;
	/*!	\brief Get the mass of the current object */
	const float GetMass() const;
	/*!	\brief Get the inverted mass of the current object */
	const float GetInverseMass() const;
	/*!	\brief Get the friction coefficient of the current object */
	const float GetFriction() const;
	/*!	\brief Get the current rotation in 2D radians of the current object */
	const float GetRotation() const;
	/*!	\brief Get the restitution coefficient of the current object */
	const float GetRestitution() const;
	/*!	\brief Get the status of the current object */
	const bool GetStatus() const;
	/*!	\brief Get the control factor of the current object */
	const bool GetIsControl() const;
	/*!	\brief Is this object in collision? */
	const bool GetIsCollision()const;
	/*!	\brief Is this object sleeping? (force idling) */
	const bool GetIsSleep()const;
	/*!	\brief Get the velocity of the current object */
	void SetVelocity(float x, float y, float z);
	/*!	\clear acce Y to 0 */
	void ClearAcceYtoZero();
	___BEGIN_REGISTERED___
	int mCollisionID;
	int mColliderType;	   // static or not
	bool mIsTriggered;     // Is a triggered object(no resolution)
	bool mIsGravityOn;     // Is effected by gravity
	bool mIsRotation;
	bool mLockOnX;
	bool mLockOnY;
	bool mLockOnZ;
	float mFriction;
	float mRestitution;
	float mMass;
	Vector3 mGravityFactor;
	//STATIC = 0;
	//DYNAMIC = 1;
	//KINEMATIC = 2;
	___END_REGISTERED___
	bool mIsForceActive;   // Is effected by force
	float dtScale;         // Scale DT
	int mCollisionShapeInt; // 0:circle, 1: box
	bool mIsJoint;         // Is a Joint object
	bool mIsSleep;         // Is in sleep mode
	bool mIsControl;	   // Is controllable
	bool mIsActive;		   // Is physics component active
	float mWidthScale;     // Scale width  of collision box
	float mHeightScale;    // Scale height of collision box
	float mOffsetCenterX;  // Offset centerX
	float mOffsetCenterY;  // Offset centerY
	float mDampingX;
	float mDampingY;
	float mDampingRot;
	float mInertia;
    float mRotation;
	float mAngVelocity;
	Point4 mPosition;
	Point4 mPrevPosition;
	Vector mVelocity;
	Vector mAcceleration;
	Vector mAccumulatedForce;
	Vector mAngAccel;
	Vector mAngPos;
	float mMotion;
	float mI_Mass;
	float mInveInertia;
	float mPreRotation;
	bool mIsCollision;     // Is collision on
	bool mIsJump;
	
	bool mIsMovingStatic;
	CollisionShape* mCollisionData;

	// new stuff 
	Math::Vector3 mLocalCentroid;
	Math::Vector3 mGlobalCentroid;
	Math::Matrix3 mOrientation;
	Math::Matrix3 mIverseOrientation;
	RigidBodyComponent* rigidCom;
	ColliderComponent* collidarCom;
	
	void CreateRigidBodyComponent(unsigned char bodyType);
	void CreateBoxColliderComponent(const Vector3& halfextend);
	void CreateSphereColliderComponent(const float radius);
	void CreateMeshColliderComponent(const std::string& filename);
	___SERIALIZE_BEGIN___
		___DEFBASE(Component)
		___DEFSER(mInertia, 1)
		___DEFSER(mMass, 1)
		___DEFSER(mFriction, 1)
		___DEFSER(mDampingX, 1)
		___DEFSER(mDampingY, 1)
		___DEFSER(mRotation, 1)
		___DEFSER(mRestitution, 1)
		___DEFSER(mAngVelocity, 1)
		___DEFSER(mPosition, 1)
		___DEFSER(mPrevPosition, 1)
		___DEFSER(mVelocity, 1)
		___DEFSER(mAcceleration, 1)
		___DEFSER(mAccumulatedForce, 1)
		___DEFSER(mAngAccel, 1)
		___DEFSER(mAngPos, 1)
		___DEFSER(mMotion, 1)
		___DEFSER(mI_Mass, 1)
		___DEFSER(mInveInertia, 1)
		___DEFSER(mPreRotation, 1)
		___DEFSER(mIsCollision, 1)
		___DEFSER(mIsForceActive, 1)
		___DEFSER(mIsActive, 1)
		bool mIsDynamic;
	___DEFSERONLYORLESS(mIsDynamic, 10)
		___DEFSER(mIsControl, 1)
		___DEFSER(mIsSleep, 1)
		___DEFSER(mCollisionData, 1)
		___DEFSER(mDampingRot, 2)
		___DEFSER(mIsTriggered, 2)
		___DEFSER(mWidthScale, 3)
		___DEFSER(mHeightScale, 3)
		___DEFSER(mCollisionShapeInt, 4)
		___DEFSER(mIsJoint, 5)
		___DEFSER(mOffsetCenterX, 6)
		___DEFSER(mOffsetCenterY, 6)
		___DEFSER(mIsGravityOn, 7)
		___DEFSER(mIsMovingStatic, 8)
		___DEFSER(mCollisionID, 9)
		___DEFSER(mIsRotation, 10)
		___DEFSER(mColliderType, 11)
		DEPRECATED float mGravityFactor;
		___DEFSER(mGravityFactor, 12)
		Vector3 temp_gravity_vector = CPhysics::mGravityFactor;
		___DEFSER(temp_gravity_vector, 13)
		CPhysics::mGravityFactor = temp_gravity_vector;
		___DEFSER(mLockOnX, 14)
		___DEFSER(mLockOnY, 14)
		___DEFSER(mLockOnZ, 14)
	___SERIALIZE_END___

	virtual void CopyFrom(const ::CPhysics* c)
	{
		mInertia = c->mInertia;
		mMass = c->mMass;
		mFriction = c->mFriction;
		mDampingX = c->mDampingX;
		mDampingY = c->mDampingY;
		mRotation = c->mRotation;
		mRestitution = c->mRestitution;
		mAngVelocity = c->mAngVelocity;
		mPosition = c->mPosition;
		mPrevPosition = c->mPrevPosition;
		mVelocity = c->mVelocity;
		mAcceleration = c->mAcceleration;
		mAccumulatedForce = c->mAccumulatedForce;
		mAngAccel = c->mAngAccel;
		mAngPos = c->mAngPos;
		mMotion = c->mMotion;
		mI_Mass = c->mI_Mass;
		mInveInertia = c->mInveInertia;
		mPreRotation = c->mPreRotation;
		mIsCollision = c->mIsCollision;
		mIsForceActive = c->mIsForceActive;
		mIsActive = c->mIsActive;
		mColliderType = c->mColliderType;
		mIsControl = c->mIsControl;
		mIsSleep = c->mIsSleep;
		mCollisionData = c->mCollisionData;
		mDampingRot = c->mDampingRot;
		mIsTriggered = c->mIsTriggered;
		mWidthScale = c->mWidthScale;
		mHeightScale = c->mHeightScale;
		mCollisionShapeInt = c->mCollisionShapeInt;
		mIsJoint = c->mIsJoint;
		mOffsetCenterX = c->mOffsetCenterX;
		mOffsetCenterY = c->mOffsetCenterY;
		mIsGravityOn = c->mIsGravityOn;
		mIsMovingStatic = c->mIsMovingStatic;
		mCollisionID = c->mCollisionID;
		mIsRotation = c->mIsRotation;
		mGravityFactor = c->mGravityFactor;
		mLockOnX = c->mLockOnX;
		mLockOnY = c->mLockOnY;
		mLockOnZ = c->mLockOnZ;
		Component::CopyFrom(c);
	}
	COMPONENTCLONE(CPhysics);

	metadef(CPhysics)
		endmetadef;
};
 
	___SERIALIZE_CLASS(CPhysics,14);
___DEC_EXT_REG___(CPhysics);
/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   CPhysics Class