/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "PhysicsEngine.h"
#include "CPhysics.h"
#include "GameObject.h"
#include "FrameController.h"
#include "Renderer.h"
#include "GSM.h"
#include "CTransform.h"
#include "BaseState.h"
#include "EditorSubsystem_Interface.h"
#include "EditorSubsystem.h"
#include "MeshCollider.h"
#define DT 0.01666666f

// Non-class functions
bool CheckTwoLineIntersect(Point3* p1,Point3* p2,Point3* p3,Point3* p4)
{
	Point3 ret;
	Vector slope1 = *p1 - *p2;
	Vector slope2 = *p3 - *p4;
	float a1,b1,c1,a2,b2,c2;
	a1 = p2->y - p1->y;
	b1 = p1->x - p2->x;
	c1 = a1 * p1->x + b1 * p1->y;

	a2 = p4->y - p3->y;
	b2 = p3->x - p4->x;
	c2 = a2 * p3->x + b2 * p3->y;

	float delta = a1 * b2  - a2 * b1;

	if(delta == 0)
		int i = 0;

	float x = (b2*c1 - c2*b1) / delta;
	float y = (a1*c2 - a2*c1)/delta;

	ret.x = x;
	ret.y = y;
	ret.z = 0.0f; 
	ret.w = 1.0f;
	float smallX = p1->x < p2->x ? p1->x :p2->x;
	float bigX = p1->x > p2->x ? p1->x :p2->x;
	float smallY = p1->y < p2->y ? p1->y :p2->y;
	float bigY = p1->y > p2->y ? p1->y :p2->y;
	if(smallX <= x && x <= bigX && smallY <= y && y <= bigY)
		return true;
	return false;
}
// Main functions
PhysicEngine::PhysicEngine() : mAccumulatedDT(0), mTimeStep(1.0f / 60.0f)
, mMaxStepsPerFrame(1)
, mVelocityIterations(12)
, mPositionIteratoins(3)
, mGravity(0,-9.8f,0)
{
	//physicsEngine = this;
	Init();
	isActive = false;
	mGravity.y = -30.0f;
	mDebugColorManager.mDebugColors.push_back(Color(1, 0, 0));
	mDebugColorManager.mDebugColors.push_back(Color(0, 1, 0));
	mDebugColorManager.mDebugColors.push_back(Color(0, 0, 1));
	mDebugColorManager.mDebugColors.push_back(Color(1, 1, 0));
	mDebugColorManager.mDebugColors.push_back(Color(1, 0, 1));
	mDebugColorManager.mDebugColors.push_back(Color(0, 1, 1));
}
PhysicEngine::~PhysicEngine()
{
	Dispose();
}
PhysicEngine* PhysicEngine::getInstance()
{
	static PhysicEngine* instance = 0;

	if(!instance)
		instance = new PhysicEngine();
	return instance;
}

bool PhysicEngine::Tick()
{
	CPhysicsList.clear();
	if (!GSM::getInstance()->getActiveState() || !GSM::getInstance()->getActiveState()->IsActive())
		return true;
	const std::list<CPhysics*>* CPhysicsListPtr = reinterpret_cast<const std::list<CPhysics*>*>(GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CPhysics>());
	if (!CPhysicsListPtr)
		return true;
	CPhysicsList = *CPhysicsListPtr;
	//if(isActive)
	//{
	//	mAccumulatedDT += FrameController::getInstance()->GetDeltaTime();
	//	if(mAccumulatedDT < DT)
	//	{
	//		mAccumulatedDT += DT;
	//		return false;
	//	}
	//	else
	//		mAccumulatedDT -= DT;
	//
	//	//populate physics list
	//
	//	mDt = FrameController::getInstance()->GetDeltaTime();
	//	//printf("DT is %f \n", mDt);
	//	// using static time step
	//	//SetDt(FrameController::getInstance()->GetDeltaTime());
	//	Integration();
	//	CollisionDetection();
	//	Resolution(&mContactManager);
	//	UpdatePosition();
	//	mContactManager.NumContacts = 0;
	//}
	//SetUpDebugDraw();

	// new stuff

	//auto &engine = GetEngine();
	//
	//// handle debug draw
	//if (!m_debugDrawSet && m_useDebugDraw)
	//{
	//	if (engine.HasSystem<GraphicsSystem>())
	//	{
	//		auto &graphics = engine.GetSystem<GraphicsSystem>();
	//		auto &debugDraw = graphics.GetDebugDrawer();
	//		m_world->SetDebugDraw(new SYMPhysicsDebugDraw3D(debugDraw));
	//
	//		m_debugDrawSet = true;
	//	}
	//}
	//else if (m_debugDrawSet && !m_useDebugDraw)
	//{
	//	auto &graphics = engine.GetSystem<GraphicsSystem>();
	//	m_world->SetDebugDraw(nullptr);
	//
	//	m_debugDrawSet = false;
	//}

	//for (auto &CPhysicsListIter : CPhysicsList)
	//{
	//	if (!CPhysicsListIter->rigidCom)
	//		CPhysicsListIter->CreateRigidBodyComponent(RigidBody3D::DYNAMIC);
	//	if (!CPhysicsListIter->collidarCom)
	//		CPhysicsListIter->CreateColliderComponent(Vector3(1.0f,1.0f,1.0f));
	//}
	//RayCast3D(Vector3(-10, 10, 0), Vector3(1, 0, 0), 100);


	float dt = FrameController::getInstance()->GetDeltaTime();
	isActive = false;
	// apply fixed time step
	mTimeAccumulator += dt;
	unsigned char stepQuota = mMaxStepsPerFrame;
	while (mTimeAccumulator >= mTimeStep && stepQuota--)
	{
		isActive = true;
		std::vector<RigidBodyComponent*> toRemove;
		for (RigidBodyComponent& rigidBodyComponent : mRigidBodyComponents)
		{
			//////////////////////////////////////////////////////////////////////////
			//hyper slow cpu burn for safety hack
			bool found = false;
			for (CPhysics* cphy : *CPhysicsListPtr)
			{
				if (cphy == rigidBodyComponent.cphy)
				{
					found = true;
					break;
				}
			}
			if (!found)
				toRemove.push_back(&rigidBodyComponent);
		}

		for (RigidBodyComponent* rem : toRemove)
		{
			for (auto iter = mRigidBodyComponents.begin(); iter != mRigidBodyComponents.end(); ++iter)
			{
				if (&(*iter) == rem)
				{
					mRigidBodyComponents.erase(iter);
					break;
				}
			}
		}

			//////////////////////////////////////////////////////////////////////////

		for (RigidBodyComponent& rigidBodyComponent : mRigidBodyComponents)
		{
			rigidBodyComponent.GrabPosFromTransform();
			rigidBodyComponent.SyncCphysics();
		}

		for (auto &colliderComponent : mColliderComponents)
		{
			int r = mColliderComponents.size();
			colliderComponent.Update();
		}

		mGravity.y = -20.0f;
		mWorld->SetGravity(Math::Vector3(mGravity.x, mGravity.y, mGravity.z));
		mWorld->Step(mTimeStep, mVelocityIterations , mPositionIteratoins );
		mTimeAccumulator -= mTimeStep;

		// update rigid body components
		for (auto &rigidBodyComponent : mRigidBodyComponents)
			rigidBodyComponent.Update();
	}
	
	return true;
}

void PhysicEngine::SetUpDebugDraw()
{
	CIterator firstObj = CPhysicsList.begin();
	CIterator lastObg = CPhysicsList.end();
	for(; firstObj != lastObg; ++firstObj)
		(*firstObj)->SetUpDebugDrawEditorMode();
}
// Helper functions
GameObject* PhysicEngine::DragObject(Point3 CamPos, Point3 Mouse)
{
	GameObject* potentialObj = NULL;
	float potentialZ = FLT_MAX;

	for (GameObject*const& obj : *(GSM::getInstance()->getActiveState()->mManagedObjects))
	{
			GameObject* retObj = DragObject_DoRaycast(CamPos, Mouse, obj, potentialZ);
			if (retObj)
				potentialObj = retObj;
	}
	for (GameObject*const& obj : (GSM::getInstance()->getActiveState()->mUnmanagedObjects))
	{
		{
			GameObject* retObj = DragObject_DoRaycast(CamPos, Mouse, obj, potentialZ);
			if (retObj)
				potentialObj = retObj;
		}
	}
	return potentialObj;
}

GameObject* PhysicEngine::DragObject_DoRaycast( Point3 CameraPos, Point3 Mouse, GameObject* obj, float potentialZ )
{
	GameObject* potentialObj = NULL;
	bool flag = true;
	Matrix4 rot = obj->transform->GetGlobalRotationMatrix();
	Vector oriAxisX(1.0f,0,0,0);
	Vector oriAxisY(0,1.0f,0,0);
	Vector AxisX = rot * oriAxisX;
	Vector AxisY = rot * oriAxisY;
	Point4 mPoints[4];
	Point4 mCenter;
	//offset by camera position
	Mouse.x += CameraPos.x;
	Mouse.y += CameraPos.y;
	Mouse.z += CameraPos.z;
	mCenter.x = obj->transform->GetGlobalPosition().x;
	mCenter.y = obj->transform->GetGlobalPosition().y;
	mCenter.z = obj->transform->GetGlobalPosition().z;
	float halfX,halfY;
	halfX = abs(obj->transform->scale.x) * 0.5f;
	halfY = abs(obj->transform->scale.y) * 0.5f;
	AxisX.Normalize();
	AxisY.Normalize();
	mPoints[0] = mCenter - AxisX * halfX + AxisY * halfY;
	mPoints[1] = mCenter - AxisX * halfX - AxisY * halfY;
	mPoints[2] = mCenter + AxisX * halfX - AxisY * halfY;
	mPoints[3] = mCenter + AxisX * halfX + AxisY * halfY;
	for (int i = 0; i < 4; ++i)
	{
		int next;
		if(i == 3)
			next = 0;
		else 
			next = i+1;
		Vector normal = mPoints[next] - mPoints[i];
		float temp = normal.x;
		normal.x = normal.y;
		normal.y = -temp;
		normal.z = 0;
		Mouse.z = 0;
		Vector checkvec = Mouse - mPoints[i];
		if(checkvec.Dot(normal) > 0)
		{
			flag = false;
			break;
		}
	}
	if(flag == true && obj->HasComponent<CCamera>() == false && obj->HasComponent<CMeshRenderer>())
	{
		if(mCenter.z < potentialZ)
		{
			potentialObj = obj;
			potentialZ = mCenter.z;
		}
	}
	flag = true;
	return potentialObj;
}

GameObject* PhysicEngine::RayCasting(Point3 start, Point3 end, Color color)
{
	int index, next;
	for (CIterator cphy = CPhysicsList.begin(); cphy != CPhysicsList.end(); ++cphy)
	{
		CollisionShape::ShapeType shapeId = (*cphy)->GetCollisionShape()->id;
		if(shapeId == CollisionShape::IdOBB)
		{
			OBB* obb = static_cast<OBB*>((*cphy)->GetCollisionShape());
			for (unsigned i = 0; i < 4; ++i)
			{
				index = i;
				if(index == 3)
					next = 0;
				else
					next = i + 1;
				bool res = CheckTwoLineIntersect(&start,&end,&obb->mPoints[index], &obb->mPoints[next]);

        Renderer::getInstance()->DrawLine(ToVector3(start), ToVector3(end), color);
				if(res == true)
					return (*cphy)->gameObject;
			}

		}
		else if(shapeId == CollisionShape::IdBox)
		{
			Box* aabb = static_cast<Box*>((*cphy)->GetCollisionShape());
			for (unsigned i = 0; i < 4; ++i)
			{
				bool res = CheckTwoLineIntersect(&start,&end,&aabb->mPoints[i], &aabb->mPoints[i+1]);
				if(res == true)
					return (*cphy)->gameObject;
			}
		}
		else if(shapeId == CollisionShape::IdCircle)
		{
			Circle* circle = static_cast<Circle*>((*cphy)->GetCollisionShape());
		}

	}
	return NULL;
}


GameObject* PhysicEngine::RayCast(Point3 start,  Vector vec, Color color)
{
	Point3 end = start + vec;
	return RayCasting(start , end, color);
}

// Private functions
void PhysicEngine::UpdatePosition()
{
	CIterator firstObj = CPhysicsList.begin();
	CIterator lastObg = CPhysicsList.end();
	for(; firstObj != lastObg; ++firstObj)
		(*firstObj)->UpdatePositionToGraphics();
}

void PhysicEngine::Integration()
{
	CIterator firstObj = CPhysicsList.begin();
	CIterator lastObg = CPhysicsList.end();
	for(; firstObj != lastObg; ++firstObj)
		(*firstObj)->Integrate(mDt);
}

void PhysicEngine::CollisionDetection()
{
	CIterator CompA = CPhysicsList.begin();
	CIterator lastObg = CPhysicsList.end();
	for(; CompA != lastObg; ++CompA)
	{
		CIterator CompB = CompA;
		++CompB;
		for(; CompB != lastObg; ++CompB)
		{
			// static don't collide with static and both need collision on
			if(((*CompA)->GetIsStatic() == false || (*CompB)->GetIsStatic() == false)&& 
			   //((*CompA)->GetIsSleep()  == false || (*CompB)->GetIsSleep()  == false)&&
			   ((*CompA)->GetIsCollision()		 && (*CompB)->GetIsCollision())      &&          
			   (*CompA)->gameObject->GetState()->GetCollisionTable()->GetDoesIDsCollide((*CompA)->GetCOllisionID(),(*CompB)->GetCOllisionID()) == true  )
					bool result = mCollision.Detection((*CompA)->GetCollisionShape(), (*CompB)->GetCollisionShape(),&mContactManager);
		}
	}
}

void PhysicEngine::Resolution(ContactsManager* CM)
{
  if(mContactManager.NumContacts)
    Resolution::ResolveContactList(CM);
}

//Setters
void PhysicEngine::SetDt(float dt)
{
	mDt = dt;
}

const PhysicEngine::RayCastOutput PhysicEngine::RayCastReport(const Ray3 &ray, float maxDistance)
{
	RayCastOutput output;
	output.hit = false;

	if (mWorld)
	{
		auto result = mWorld->RayCast(ray, maxDistance);
		if (result.hit)
		{
			output.hit = true;
			output.collider = static_cast<ColliderComponent *>(result.collider->mUserData);
			output.intersection = result.intersection;
			output.normal = result.normal;
		}
	}

	return output;
}

void PhysicEngine::QueryRegion(const Vector &minPoint, const Vector &maxPoint, RegionQueryResult &result)
{
	PhysicsWorld::RegionQueryResult r;
	mWorld->QueryRegion(Math::Vector3(minPoint.x, minPoint.y, minPoint.z), Math::Vector3(maxPoint.x, maxPoint.y, maxPoint.z), r);
	for (auto collider : r)
		result.push_back(static_cast<ColliderComponent *>(collider->mUserData));
}

float PhysicEngine::GetTimeStep(void) const
{
	return mTimeStep;
}
void PhysicEngine::SetTimeStep(float timeStep)
{
	mTimeStep = timeStep;
}
unsigned char PhysicEngine::GetMaxStepsPerFrame(void) const
{
	return mMaxStepsPerFrame;
}
void PhysicEngine::SetMaxStepsPerFrame(unsigned char steps)
{
	mMaxStepsPerFrame = steps;
}
unsigned char PhysicEngine::GetVelocityIterations(void) const
{
	return mVelocityIterations;
}
void PhysicEngine::SetVelocityIterations(unsigned char iterations)
{
	mVelocityIterations = iterations;
}
unsigned char PhysicEngine::GetPositionIterations(void) const
{
	return mPositionIteratoins;
}
void PhysicEngine::SetPositionIterations(unsigned char iterations)
{
	mPositionIteratoins = iterations;
}
const Vector &PhysicEngine::GetGravity(void) const
{
	return mGravity;
}
PhysicEngine &PhysicEngine::SetGravity(const Vector &g)
{
	mGravity = g;
	return *this;
}

void PhysicEngine::Init(void)
{
	mWorld.reset(new PhysicsWorld());
	mWorld->SetListener(new SystemContactListener(*this));
	mTimeAccumulator = 0.0f;
}

void PhysicEngine::Dispose(void)
{
	mWorld.reset();
}

void PhysicEngine::Update(float dt)
{}

void PhysicEngine::Register(RigidBodyComponent &component)
{
	mRigidBodyComponents.push_back(&component);
}

void PhysicEngine::Unregister(RigidBodyComponent &component)
{
	mRigidBodyComponents.erase(&component);
}

void PhysicEngine::Register(ColliderComponent &component)
{
	mColliderComponents.push_back(&component);
}

void PhysicEngine::Unregister(ColliderComponent &component)
{
	mColliderComponents.erase(&component);
}

GameObject* PhysicEngine::RayCast3DWithTag(const Vector3& pos, const Vector3& dir, CCamera* camera, int screen_x, int screen_y, float maxDist, std::string& tag)
{
	Vector3 dir2 = camera->ScreenToWorldPosition(screen_x, screen_y);
	return RayCast3DWithTag(pos, dir2, maxDist, tag);
}

RayCastResult PhysicEngine::RayCast3D_Output(const Vector3 &pos, const Vector3 &dir, float maxDistance)
{
	RayCastResult output;

	GameObject* potentialObj = NULL;
	const std::list<CRenderer*>* cmeshes = 0;
	if (Engine::getInstance()->GetIsEditorMode() && !EditorSS::IsTesting())
	{
		//also ray cast editor state objects
		cmeshes = EditorSubsystem::getInstance()->GetEditorState()->GetListOfComponentsOfType<CRenderer>();
		if (cmeshes)
			output = RayCast3D_Output_DoRaycast(pos, dir, maxDistance, cmeshes, output);
		if (potentialObj)
			return output;	//prioritize editor state objects
	}

	cmeshes = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CRenderer>();
	if (!cmeshes)
		return output;
	output = RayCast3D_Output_DoRaycast(pos, dir, maxDistance, cmeshes, output);

	return output;
}

RayCastResult PhysicEngine::RayCast3D_Output_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs, RayCastResult& output)
{
	std::vector<CRenderer*> objList;
	GameObject* potentialObj = NULL;
	float potentialZ = FLT_MAX;

	Ray3 ray;
	ray.pos = pos;
	if (dir != Vector3::cZero)
		ray.dir = dir.Normalized();

	//Renderer::getInstance()->UpdateRay(ray.pos, ray.dir);
	for (CRenderer* cmesh : *objs)
	{
		if (!cmesh->gameObject->HasComponent<CMeshColliderComponent>())
			continue;
		if (cmesh->GetMesh() == 0)
			continue;
		if (cmesh->gameObject->mLayerMask.IsOnLayer(LayerMask::LAYER_NORAYCAST))
			continue;
		if (!Engine::getInstance()->GetIsEditorMode() && !cmesh->globalEnabled())
			continue;
		AABB aabb = cmesh->GetMesh()->GetAABB();
		AABBFromModelSapceToWorldSpace(aabb, *cmesh->gameObject->transform);

		if (RayAABB(ray, aabb, maxDistance, potentialZ))
			objList.push_back(cmesh);
	}

	potentialZ = FLT_MAX;

	for (CRenderer* cmesh : objList)
		RayMesh_Output(ray, *cmesh, maxDistance, potentialZ, output);

	return output;
}

RayCastResult PhysicEngine::RayCast3DWithTag_Output(const Vector3 &pos, const Vector3 &dir, float maxDistance, std::string& tag)
{
	RayCastResult output;

	GameObject* potentialObj = NULL;
	const std::list<CRenderer*>* cmeshes = 0;
	if (Engine::getInstance()->GetIsEditorMode() && !EditorSS::IsTesting())
	{
		//also ray cast editor state objects
		cmeshes = EditorSubsystem::getInstance()->GetEditorState()->GetListOfComponentsOfType<CRenderer>();
		if (cmeshes)
			output = RayCast3DWithTag_Output_DoRaycast(pos, dir, maxDistance, cmeshes, tag, output);
		if (potentialObj)
			return output;	//prioritize editor state objects
	}

	cmeshes = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CRenderer>();
	if (!cmeshes)
		return output;
	output = RayCast3DWithTag_Output_DoRaycast(pos, dir, maxDistance, cmeshes, tag, output);

	return output;
}

RayCastResult PhysicEngine::RayCast3DWithTag_Output_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs, std::string& tag, RayCastResult& output)
{
	std::vector<CRenderer*> objList;
	GameObject* potentialObj = NULL;
	float potentialZ = FLT_MAX;

	Ray3 ray;
	ray.pos = pos;
	if (dir != Vector3::cZero)
		ray.dir = dir.Normalized();

	//Renderer::getInstance()->UpdateRay(ray.pos, ray.dir);
	for (CRenderer* cmesh : *objs)
	{
		if (!cmesh->gameObject->HasComponent<CMeshColliderComponent>())
			continue;
		if (!cmesh->gameObject->GetFlag(tag))
			continue;
		if (cmesh->GetMesh() == 0)
			continue;
		if (cmesh->gameObject->mLayerMask.IsOnLayer(LayerMask::LAYER_NORAYCAST))
			continue;
		if (!Engine::getInstance()->GetIsEditorMode() && !cmesh->globalEnabled())
			continue;
		AABB aabb = cmesh->GetMesh()->GetAABB();
		AABBFromModelSapceToWorldSpace(aabb, *cmesh->gameObject->transform);

		if (RayAABB(ray, aabb, maxDistance, potentialZ))
			objList.push_back(cmesh);
	}

	potentialZ = FLT_MAX;

	for (CRenderer* cmesh : objList)
		RayMesh_Output(ray, *cmesh, maxDistance, potentialZ, output);

	return output;
}

GameObject* PhysicEngine::RayCast3D(const Vector3 &pos, const Vector3 &dir, float maxDistance)
{
	GameObject* potentialObj = NULL;
	const std::list<CRenderer*>* cmeshes = 0;
	if (Engine::getInstance()->GetIsEditorMode() && !EditorSS::IsTesting())
	{
		//also ray cast editor state objects
		cmeshes = EditorSubsystem::getInstance()->GetEditorState()->GetListOfComponentsOfType<CRenderer>();
		if (cmeshes)
			potentialObj = RayCast3D_DoRaycast(pos, dir, maxDistance, cmeshes);
		if (potentialObj)
			return potentialObj;	//prioritize editor state objects
	}

	cmeshes = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CRenderer>();
	if (!cmeshes)
		return 0;
	potentialObj = RayCast3D_DoRaycast(pos, dir, maxDistance, cmeshes);

	return potentialObj;
}

GameObject* PhysicEngine::RayCast3DWithTag(const Vector3 &pos, const Vector3 &dir, float maxDistance, std::string& tag)
{
	GameObject* potentialObj = NULL;
	const std::list<CRenderer*>* cmeshes = 0;
	if (Engine::getInstance()->GetIsEditorMode() && !EditorSS::IsTesting())
	{
		//also ray cast editor state objects
		cmeshes = EditorSubsystem::getInstance()->GetEditorState()->GetListOfComponentsOfType<CRenderer>();
		if (cmeshes)
			potentialObj = RayCast3DWithTag_DoRaycast(pos, dir, maxDistance, cmeshes, tag);
		if (potentialObj)
			return potentialObj;	//prioritize editor state objects
	}

	cmeshes = GSM::getInstance()->getActiveState()->GetListOfComponentsOfType<CRenderer>();
	if (!cmeshes)
		return 0;
	potentialObj = RayCast3DWithTag_DoRaycast(pos, dir, maxDistance, cmeshes, tag);

	return potentialObj;
}

GameObject* PhysicEngine::RayCast3DWithTag_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs, std::string& tag)
{
	std::vector<CRenderer*> objList;
	GameObject* potentialObj = NULL;
	float potentialZ = FLT_MAX;

	Ray3 ray;
	ray.pos = pos;
	if (dir != Vector3::cZero)
		ray.dir = dir.Normalized();

	for (CRenderer* cmesh : *objs)
	{
		if (cmesh->GetMesh() == 0)
			continue;
		if (!cmesh->gameObject->GetFlag(tag))
			continue;
		//if (!Engine::getInstance()->GetIsEditorMode() && !cmesh->isEnabled())
			//continue;
		AABB aabb = cmesh->GetMesh()->GetAABB();
		AABBFromModelSapceToWorldSpace(aabb, *cmesh->gameObject->transform);

		if (RayAABB(ray, aabb, maxDistance, potentialZ))
			objList.push_back(cmesh);
	}

	potentialZ = FLT_MAX;
	for (CRenderer* cmesh : objList)
	{
		if (RayMesh(ray, *cmesh, maxDistance, potentialZ))
			potentialObj = cmesh->gameObject;
	}

	return potentialObj;
}

GameObject* PhysicEngine::RayCast3D(const Vector3& pos, const Vector3& dir, CCamera* camera, int screen_x, int screen_y, float maxDist)
{
	Vector3 dir2 = camera->ScreenToWorldPosition(screen_x, screen_y);
	return RayCast3D(pos, dir2, maxDist);
}



GameObject* PhysicEngine::RayCast3D_DoRaycast(const Vector3 &pos, const Vector3 &dir, float maxDistance, const std::list<CRenderer*>* objs)
{
	std::vector<CRenderer*> objList;
	GameObject* potentialObj = NULL;
	float potentialZ = FLT_MAX;

	Ray3 ray;
	ray.pos = pos;
	if (dir != Vector3::cZero)
		ray.dir = dir.Normalized();

	//Renderer::getInstance()->UpdateRay(ray.pos, ray.dir);
	for (CRenderer* cmesh : *objs)
	{
		if (cmesh->GetMesh() == 0)
			continue;

		if (!Engine::getInstance()->GetIsEditorMode() && !cmesh->globalEnabled())
			continue;
		AABB aabb = cmesh->GetMesh()->GetAABB();
		AABBFromModelSapceToWorldSpace(aabb, *cmesh->gameObject->transform);

		if (RayAABB(ray, aabb, maxDistance, potentialZ))
			objList.push_back(cmesh);
	}
	
	potentialZ = FLT_MAX;
	for (CRenderer* cmesh : objList)
	{
		if (RayMesh(ray, *cmesh, maxDistance, potentialZ))
			potentialObj = cmesh->gameObject;
	}

	return potentialObj;
}



PhysicEngine::SystemContactListener::SystemContactListener(PhysicEngine &system)
: mSystem(system)
{ }

void PhysicEngine::SystemContactListener::BeginContact(const Collider &colliderA, const Collider &colliderB, const ContactManifold &manifoild)
{
	// TODO
}

void PhysicEngine::SystemContactListener::EndContact(const Collider &colliderA, const Collider &colliderB, const ContactManifold &manifoild)
{
	// TODO
}

meta_define(PhysicEngine);

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   PhysicEngine Class