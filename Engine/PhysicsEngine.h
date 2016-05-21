/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */


#pragma once
#include "CPhysics.h"
#include "Resolution.h"
#include "Collision.h"
#include "Color.h"
#include "ISubsystem.h"
#include "Ray3.h"
#include "InList.h"
#include "Smart_pointer.h"
#include "PhysicsScene.h"
#include "MathIncludes.h"
#include "RigidBodyCom.h"
#include "ColliderCom.h"
#include "RayCastResult.h"

class CMeshRenderer;
class CRenderer;
class PhysicEngine : public ISubsystem
{
friend class RigidBodyComponent;
friend class ColliderComponent;
public:

	// Main functions
	PhysicEngine();
	~PhysicEngine();
	static PhysicEngine* getInstance();
	bool Tick();

	virtual void Init(void);
	virtual void Dispose(void);
	virtual void Update(float dt);

	// Getters
	bool IsActive() const { return isActive; }
	void IsActive(bool val) { isActive = val; }
	std::list <CPhysics*>& GetPhysicsList() { return CPhysicsList; }
	float GetTimeStep(void) const;
	unsigned char GetMaxStepsPerFrame(void) const;
	unsigned char GetVelocityIterations(void) const;
	unsigned char GetPositionIterations(void) const;
	const Vector &GetGravity(void) const;
	PhysicsWorld &World(void) { return *mWorld; }
	// Setters
	void SetUpDebugDraw();
	void SetTimeStep(float timeStep);
	void SetMaxStepsPerFrame(unsigned char steps);
	void SetVelocityIterations(unsigned char iterations);
	void SetPositionIterations(unsigned char iterations);
	PhysicEngine &SetGravity(const Vector &g);

	struct RayCastOutput
	{
		bool hit;
		float t;
		ColliderComponent *collider;
		GameObject* go;
		Math::Vector3 intersection;
		Math::Vector3 normal;
		RayCastOutput(void) : hit(false), collider(nullptr) { }
	};
	// Helper functions
	void Register(RigidBodyComponent &component);
	void Unregister(RigidBodyComponent &component);
	void Register(ColliderComponent &component);
	void Unregister(ColliderComponent &component);
	GameObject* DragObject(Point3 CamPos, Point3 Mouse);
	GameObject* DragObject_DoRaycast(Point3 CamPos, Point3 Mouse, GameObject* obj, float);
	GameObject* RayCasting(Point3 start, Point3 end, Color color);
	 GameObject* RayCast(Point3 start, Vector vec, Color color);


	 GameObject* RayCast3D(const Vector3 &pos, const Vector3 &dir,  float maxDistance);
	 GameObject* RayCast3D(const Vector3& pos, const Vector3& dir, CCamera* camera, int screen_x, int screen_y, float maxDist);
	 GameObject* RayCast3D_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs);

	 GameObject* RayCast3DWithTag(const Vector3 &pos, const Vector3 &dir, float maxDistance, std::string& tag);
	 GameObject* RayCast3DWithTag(const Vector3& pos, const Vector3& dir, CCamera* camera, int screen_x, int screen_y, float maxDist, std::string& tag);
	 GameObject* RayCast3DWithTag_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs, std::string& tag);

	 RayCastResult RayCast3D_Output(const Vector3 &pos, const Vector3 &dir, float maxDistance);
	 RayCastResult RayCast3D_Output_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs, RayCastResult& output);

	 RayCastResult RayCast3DWithTag_Output(const Vector3 &pos, const Vector3 &dir, float maxDistance, std::string& tag);
	 RayCastResult RayCast3DWithTag_Output_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs, std::string& tag, RayCastResult& output);
	 

	/*! \brief The global delta time for the physics simulation. If this is lower, the scene will appear to be in slow motion */
	 void SetDt(float dt);


	 typedef std::vector<ColliderComponent *> RegionQueryResult;
	 const RayCastOutput RayCastReport(const Ray3 &ray, float maxDistance);
	 void QueryRegion(const Vector &minPoint, const Vector &maxPoint, RegionQueryResult &result);

	// Public members
	 struct DebugColor
	 {
		 std::vector<Color> mDebugColors;
		 unsigned index;
		 Color& GetColor() { ++index; if (index == mDebugColors.size()) index = 0;  return mDebugColors[index]; }
	 };
	 DebugColor mDebugColorManager;
	std::list <CPhysics*> CPhysicsList;
	typedef std::list <CPhysics*>::iterator CIterator;
	IN_LIST(RigidBodyComponent, mSystemLink) mRigidBodyComponents;
	IN_LIST(ColliderComponent, mSystemLink) mColliderComponents;

private:

	// Private functions
	void UpdatePosition();
	void Integration();
	void CollisionDetection();
	void Resolution(ContactsManager* mContactManager);

	// Private members
	bool isActive;
	float mDt;
	float mAccumulatedDT;
	Collision mCollision;
	ContactsManager mContactManager;

	unique_ptr_phy<PhysicsWorld> mWorld;
	float mTimeAccumulator;

	float mTimeStep;
	unsigned char mMaxStepsPerFrame;
	unsigned char mVelocityIterations;
	unsigned char mPositionIteratoins;
	Vector mGravity;

	struct SystemContactListener
		: public ContactListener
	{
	public:
		SystemContactListener(PhysicEngine &system);
		virtual void BeginContact(const Collider &colliderA, const Collider &colliderB, const ContactManifold &manifoild);
		virtual void EndContact(const Collider &colliderA, const Collider &colliderB, const ContactManifold &manifoild);
	private:
		PhysicEngine &mSystem;
	};

	public:
		metadef(PhysicEngine)
			endmetadef;
};
 

	/////////////////////////////////////
	//Original Author: Kai Chuan Hsiao
	//Modifications:
	//	Date	-	Change
	//  13/5/13  -   PhysicEngine Class