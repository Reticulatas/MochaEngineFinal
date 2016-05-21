%{
#include "CPhysics.h"
	%}



class CPhysics : public Component
{
public:
	void AddForce(Vector force);
	void SetVelocity(Vector const& velocity);
	void SetAnguVelocity(float angu);
	void SetPosition(Point3 const& position);
	void SetStatus(bool switcher);
	void SetIsControl(bool switcher);
	void SetIsForceActive(bool switcher);

	const int   GetCOllisionID() const;
	const bool  GetIsStatic() const;
	const float GetOffSetX() const;
	const float GetOffSetY() const;
	const Vector& GetAcceleration() const;
	const Vector& GetVelocity() const;
	const Point3& GetPosition() const;
	const Point3& GetPrevPosition() const;
	const Vector& GetForce() const;

	void SetMass(float mass);
	void SetInertia(float mass);
	void SetFriction(float friction);
	void SetRestitution(float res);
	void AddForce(float x, float y, float z);
	void SetRotation(float val);
	void SetIsSleep(bool switcher);
	void SetForceToZero();
	const float& GetAngVelocity() const;
	const float& GetInertia() const;
	const float& GetInvInertia() const;
	const float GetMass() const;
	const float GetInverseMass() const;
	const float GetFriction() const;
	const float GetRotation() const;
	const float GetRestitution() const;
	const bool GetStatus() const;
	const bool GetIsControl() const;
	const bool GetIsCollision()const;
	const bool GetIsSleep()const;
	void SetVelocity(float x, float y, float z);
	void ClearAcceYtoZero();

	int mCollisionID;
	int mColliderType;	  
	bool mIsTriggered;    
	bool mIsGravityOn;    
	bool mIsRotation;
	bool mLockOnX;
	bool mLockOnY;
	bool mLockOnZ;
	float mFriction;
	float mRestitution;
	Math::Vector3 mGravityFactor;
	float mMass;
	RigidBodyComponent* rigidCom;
	ColliderComponent* collidarCom;
};

class RigidBodyComponent

{
	RigidBodyComponent(void);
	~RigidBodyComponent(void);
public:
	RigidBody &Body(void);

	int GetType(void) const;
	RigidBodyComponent &SetType(int type);

	bool GetAwake(void) const;
	RigidBodyComponent &SetAwake(bool awake);

	const Math::Vector3 &GetCentroid(void) const;

	const Math::Vector3 &GetPosition(void) const;
	RigidBodyComponent &SetPosition(const Math::Vector3 &pos);

	const Math::Matrix3 &GetOrientation(void) const;
	RigidBodyComponent &SetOrientation(const Math::Matrix3 &orientation);

	Math::Vector3 Forward(void);
	Math::Vector3 Right(void);
	Math::Vector3 Up(void);

	const Math::Vector3 GetLinearVelocity(void) const;
	RigidBodyComponent &SetLinearVelocity(const Math::Vector3 &pos);

	const Math::Vector3 GetAngularVelocity(void) const;
	RigidBodyComponent &SetAngularVelocity(const Math::Vector3 &pos);

	RigidBodyComponent &ApplyForce(const Math::Vector3 &force, const Math::Vector3 &at);
	RigidBodyComponent &ApplyLinearForce(const Math::Vector3 &force);
	RigidBodyComponent &ApplyAngularForce(const Math::Vector3 &force, const Math::Vector3 &at);
	RigidBodyComponent &ApplyMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);
	RigidBodyComponent &ApplyLinearMasslessForce(const Math::Vector3 &force);
	RigidBodyComponent &ApplyAngularMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);

	RigidBodyComponent &ApplyImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	RigidBodyComponent &ApplyLinearImpulse(const Math::Vector3 &impulse);
	RigidBodyComponent &ApplyAngularImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	RigidBodyComponent &ApplyImmediateImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);

	bool GetAllowRotation(void) const;
	RigidBodyComponent &SetAllowRotation(bool allowRotation);

	bool GetHasGravity(void) const;
	RigidBodyComponent &SetHasGravity(bool hasGravity);
};

class RigidBody
{
	RigidBody();
	~RigidBody();
public:
	void ApplyForce(const Math::Vector3 &force, const Math::Vector3 &at);
	void ApplyLinearForce(const Math::Vector3 &force);
	void ApplyAngularForce(const Math::Vector3 &force, const Math::Vector3 &at);
	void ApplyMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);
	void ApplyLinearMasslessForce(const Math::Vector3 &force);
	void ApplyAngularMasslessForce(const Math::Vector3 &force, const Math::Vector3 &at);

	void ApplyImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	void ApplyLinearImpulse(const Math::Vector3 &impulse);
	void ApplyAngularImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);
	void ApplyImmediateImpulse(const Math::Vector3 &impulse, const Math::Vector3 &at);

	void SetPosition(const Math::Vector3 &position);
	void SetOrientation(Math::Matrix3 orientation);
	void SetLinearVelocity(const Math::Vector3 &velocity);
	void SetAngularVelocity(const Math::Vector3 &velocity);
	void SetAwake(bool awake);
	bool AllowRotation(void) const;
	void SetAllowRotation(bool allowRotation);

	bool HasGravity(void) const;
	void SetHasGravity(bool hasGravity);

	const Math::Vector3 &Position(void) const;
	const Math::Vector3 &LinearVelocity(void) const;
	const Math::Matrix3 &Orientation(void) const;
	const Math::Matrix3 &InverseOrienation(void) const;
	const Math::Vector3 &AngularVelocity(void) const;
};

%template(Vector3Vector) std::vector<Math::Vector3>;

//data to be sent to scripting layer
struct CollisionData : public IMeta
{
	 GameObject* collidedObj;
	 CPhysics* selfCPhy;
	 CPhysics* collidedObjCPhy;
	 std::vector<Math::Vector3> normals;
	 int numContacts;
	 bool isA;
	 //NO VEC3S IN THIS CLASS
};