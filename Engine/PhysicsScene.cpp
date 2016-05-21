#include "stdafx.h"
#include "PhysicsScene.h"
#include "AABBTree.h"
#include "StopWatch.h"
#include "Island.h"
#include "ScriptSubsystem.h"
#include "CScript.h"

class CPhysics;
class RigidBodyComponent;
PhysicsWorld::PhysicsWorld(void)
: mBroadphase(new AABBTree())
, mDebugDraw(nullptr)
, mGravity(0.0f, 0.0f, 0.0f)
, mContactManager(mManifoldAllocator, mContactAllocator, mBodyContactEdgeAllocator)
, mBodyAllocator(sizeof(RigidBody))
, mBodyContactEdgeAllocator(sizeof(BodyContactEdge))
, mManifoldAllocator(sizeof(ContactManifold))
, mContactAllocator(sizeof(Contact))
, mColliderAllocator(sizeof(Collider))
{ }

PhysicsWorld::~PhysicsWorld(void)
{
	// destroy bodies
	while (!mRigidBodies.empty())
		DestroyBody(*mRigidBodies.begin());
}

// Helpers
const RayCastResult PhysicsWorld::RayCast(const Ray3 &ray, float maxDistance)
{
	++mStats.rayCasts;
	Ray3 nRay = ray;
	nRay.dir.Normalize();
	auto result = mBroadphase->RayCast(nRay, maxDistance);
	result.ray = nRay;
	if (!result.hit)
		result.intersection = nRay.pos + nRay.dir * maxDistance;
	mRayCasts.push_back(result);
	return result;
}

void PhysicsWorld::Pick(const Math::Vector3 &pos, PickResult &result)
{
	Broadphase::PickResult colliders;
	mBroadphase->Pick(pos, colliders);

	// clear pick flags
	for (auto collider : colliders)
	{
		auto &body = *collider->mParent;
		body.mFlags.Clear(PhysicsFlags::PICK);
	}

	// compute result
	for (auto collider : colliders)
	{
		auto &body = *collider->mParent;
		if (!body.mFlags.Test(PhysicsFlags::PICK) && collider->CollidesCollider(pos))
		{
			body.mFlags.Set(PhysicsFlags::PICK);
			result.push_back(&body);
		}
	} // end of collider loop
}

void PhysicsWorld::WakeAll(void)
{
	for (auto &body : mRigidBodies)
		body.SetAwake(true);
}
void PhysicsWorld::DestroyBody(RigidBody &body)
{
	GV_ASSERT(body.mWorld == this);

	// destroy associated joints
	while (!body.mJointEdges.empty())
		DestroyJoint(*body.mJointEdges.pop_front()->joint);

	body.~RigidBody();
	mBodyAllocator.Free(&body);
}
void PhysicsWorld::QueryRegion(const Math::Vector3 &minPoint, const Math::Vector3 &maxPoint, RegionQueryResult &result)
{
	return mBroadphase->QueryRegion(Proxy(minPoint, maxPoint), result);
}


Joint &PhysicsWorld::CreateJoint(const JointDef &def)
{
	Joint *joint = Joint::Create(def, mJointAllocator);

	// link joint edges
	if (joint->mBodyA)
	{
		joint->mBodyA->mJointEdges.push_back(&joint->mEdgeA);
		joint->mBodyA->SetAwake(true);
	}
	if (joint->mBodyB)
	{
		joint->mBodyB->mJointEdges.push_back(&joint->mEdgeB);
		joint->mBodyB->SetAwake(true);
	}

	mJoints.push_back(joint);

	return *joint;
}
RigidBody &PhysicsWorld::CreateBody(const RigidBodyDef &def)
{
	RigidBody *body = new (mBodyAllocator.Allocate()) RigidBody(def);
	mRigidBodies.push_back(body);
	body->mWorld = this;
	return *body;
}

void PhysicsWorld::DestroyJoint(Joint &joint)
{
	if (joint.mBodyA)
		joint.mBodyA->SetAwake(true);
	if (joint.mBodyB)
		joint.mBodyB->SetAwake(true);

	Joint::Destroy(&joint, mJointAllocator);
}

void PhysicsWorld::Draw(void)
{}
PhysicsDebugDraw &PhysicsWorld::DebugDraw(void)
{
	return *mDebugDraw;
}

// Getters
PhysicsParams &PhysicsWorld::Params(void) { return mParams; }

const PhysicsParams &PhysicsWorld::Params(void) const { return mParams; }

Math::Vector3 &PhysicsWorld::Gravity(void) { return mGravity; }

const Math::Vector3 &PhysicsWorld::Gravity(void) const { return mGravity; }

const PhysicsStats &PhysicsWorld::Stats(void) const { return mStats; }
// Setters
void PhysicsWorld::SetSleepThreshold(float linearVelocity, float angularVelocity, float time)
{
	mParams.sleepLinearThreshold = linearVelocity;
	mParams.sleepAngularThreshold = angularVelocity;
	mParams.sleepTimeThreshold = time;
}
void PhysicsWorld::SetParams(const PhysicsParams &params)
{
	mParams = params;
}
void PhysicsWorld::SetGravity(const Math::Vector3 &gravity)
{
	mGravity = gravity;
}
void PhysicsWorld::SetAllowSleep(bool allowSleep)
{
	mParams.allowSleep = allowSleep;
}
void PhysicsWorld::SetWarmStart(bool warmStart)
{
	mParams.warmStart = warmStart;
}

void PhysicsWorld::SetWarmStartFraction(float fraction)
{
	mParams.warmStartFraction = Clamp(fraction, 0.0f, 1.0f);
}
void PhysicsWorld::SetBroadphase(unique_ptr_phy<Broadphase> broadphase)
{
	mBroadphase->Transfer(*broadphase);
	mBroadphase = broadphase;
}
void PhysicsWorld::SetDamping(float damping)
{
	SetDamping(damping, damping);
}

void PhysicsWorld::SetDamping(float linear, float angular)
{
	mParams.linearDamping = Clamp(linear, 0.0f, 1.0f);
	mParams.angularDamping = Clamp(angular, 0.0f, 1.0f);
	WakeAll();
}
void PhysicsWorld::SetDebugDraw(unique_ptr_phy<PhysicsDebugDraw> debugDraw)
{
	mDebugDraw = debugDraw;
}
void PhysicsWorld::SetListener(ContactListener *listener)
{
	mContactManager.mListener.reset(listener);
}

// main
void PhysicsWorld::Step(float dt, unsigned char velocityIterations, unsigned char positionIterations)
{
	const TimeStep timeStep(dt, velocityIterations, positionIterations, mParams, mStats);
	Stopwatch timer;
	mStats.Clear();

	// update rigid bodies
	const float linearDampFactor = 1.0f - Clamp(mParams.linearDamping  * timeStep.dt, 0.0f, 1.0f);
	const float angularDampFactor = 1.0f - Clamp(mParams.angularDamping * timeStep.dt, 0.0f, 1.0f);
	timer.Start();
	for (auto &body : mRigidBodies)
	{
		// TODOKai mass
		if (!body.CollidersValid())
			body.ValidateColliders();

		// update
		body.UpdateMassAndLocalCentroid();
		body.UpdateOrientation();
		body.UpdatePositionFromGlobalCentroid();
		body.UpdateGlobalInverseInertiaTensor();
		body.UpdateProxies();

		timeStep.stats.colliders += body.mColliders.size();
	}
	timeStep.stats.rigidBodies = mRigidBodies.size();
	timeStep.stats.integration += timer.Stop();

	// broadphase
	timer.Start();
	auto &pairList = mBroadphase->ComputePairs();
	timeStep.stats.broadphasePairs = pairList.size();
	timeStep.stats.broadphase += timer.Stop();

	// narrowphase
	timer.Start();
	mContactManager.PreNarrowphase();
	for (ColliderPair &pair : pairList)
	{
		Collider *colliderA = pair.mCollider1;
		Collider *colliderB = pair.mCollider2;
		RigidBody &bodyA = *colliderA->mParent;
		RigidBody &bodyB = *colliderB->mParent;
		CPhysics* cphyA = bodyA.mParent->cphy;
		CPhysics* cphyB = bodyB.mParent->cphy;

		if (!bodyA.CanCollide(bodyB) || !colliderA->CanBeCollide(*colliderB))
			continue;

		// collision table check
		if (!cphyA->gameObject->GetState()->GetCollisionTable()->GetDoesIDsCollide(cphyA->GetCOllisionID(), cphyB->GetCOllisionID()))
			continue;

		// make sure colliderA is always less than colliderB in memory address for consistency
		if (colliderA > colliderB)
		{
			std::swap(colliderA, colliderB);
			auto temp = cphyA;
			cphyA = cphyB;
			cphyB = temp;
		}


		ContactManifold *manifold = new (mManifoldAllocator.Allocate()) ContactManifold();
		manifold->colliderA = colliderA;
		manifold->colliderB = colliderB;

		//TODO
		manifold->isColliding =
			Collide(*manifold, *colliderA->mGeometry, *colliderB->mGeometry, mContactAllocator);

		if (!manifold->isColliding
			|| !mContactManager.Add(*manifold, mParams.contactPersistenceThreshold * mParams.contactPersistenceThreshold))
		{
			// manifold not colliding OR persistent manifold already exists, delete
			manifold->~ContactManifold();
			mManifoldAllocator.Free(manifold);
		}
		else
		{
			//these will be deleted by the script subsystem
			CollisionData* dataA = new CollisionData();
			CollisionData* dataB = new CollisionData();

			// send manifold to OnCollide function
			{
				dataA->isA = true;
				dataA->collidedObj = cphyB->gameObject;
				dataA->selfCPhy = cphyA;
				dataA->collidedObjCPhy = cphyB;
				dataA->numContacts = manifold->numContacts;
				for (int i = 0; i < dataA->numContacts; ++i)
				{
					dataA->normals.push_back(manifold->contacts[i]->normal);
				}
				ScriptSubsystem::getInstance()->QueueEvent<CollisionData>(cphyA->gameObject, "OnCollisionEnter", dataA);
			}
			{
				dataB->isA = false;
				dataB->collidedObj = cphyA->gameObject;
				dataB->selfCPhy = cphyB;
				dataB->collidedObjCPhy = cphyA;
				dataB->numContacts = manifold->numContacts;
				for (int i = 0; i < dataB->numContacts; ++i)
				{
					dataB->normals.push_back(manifold->contacts[i]->normal);
				}
				ScriptSubsystem::getInstance()->QueueEvent<CollisionData>(cphyB->gameObject, "OnCollisionEnter", dataB);
			}
			if (cphyA->mIsTriggered || cphyB->mIsTriggered)
			{
				manifold->~ContactManifold();
				mManifoldAllocator.Free(manifold);
			}
		}
	}
	mContactManager.PostNarrowphase();
	timeStep.stats.narrowphase += timer.Stop();

	SimulateIslands(timeStep);

	// update broadphase after position integration
	mBroadphase->Update(timeStep);

	mContactManager.Draw();

}
void PhysicsWorld::SimulateIslands(const TimeStep &timeStep)
{
	// clear & update flags
	for (RigidBody &body : mRigidBodies)
	{
		// body
		body.mFlags.Clear(PhysicsFlags::ISLAND);
		body.mFlags.Set(PhysicsFlags::CAN_SLEEP);

		// joints
		for (auto &jointEdge : body.mJointEdges)
		{
			Joint &joint = *jointEdge.joint;
			joint.mFlags.Clear(PhysicsFlags::ISLAND);

			if (!joint.mAllowSleep)
				body.mFlags.Clear(PhysicsFlags::CAN_SLEEP);
		}

		// manifolds
		for (auto &collider : body.mColliders)
		for (auto &contactEdge : collider.mContactEdges)
			contactEdge.manifold->flags.Clear(PhysicsFlags::ISLAND);
	}

	// build & solve islands
	const float linearDampFactor = 1.0f - Clamp(mParams.linearDamping  * timeStep.dt, 0.0f, 1.0f);
	const float angularDampFactor = 1.0f - Clamp(mParams.angularDamping * timeStep.dt, 0.0f, 1.0f);
	 float sleepLinearThresholdSQ = mParams.sleepLinearThreshold * mParams.sleepLinearThreshold;
	const float sleepAngularThresholdSQ = mParams.sleepAngularThreshold * mParams.sleepAngularThreshold;
	Island island(mGravity, linearDampFactor, angularDampFactor);
	IN_LIST(RigidBody, mIslandSearchLink) queue;
	for (auto &seed : mRigidBodies)
	{
		// already belongs to one island, continue
		if (seed.mFlags.Test(PhysicsFlags::ISLAND))
			continue;

		// skip sleeping bodies
		if (mParams.allowSleep && !seed.IsAwake())
			continue;

		// skip static bodies
		if (seed.mType == RigidBody::STATIC)
			continue;

		// clear island and construct a new island 
		// using current rigid body as a "seed"
		float minSleepTime = FLT_MAX;
		island.Clear();
		seed.mFlags.Set(PhysicsFlags::ISLAND);
		queue.push_back(&seed);

		// build island
		// breadth first search (BFS) on the constraint graph (using a FIFO queue)
		while (!queue.empty())
		{
			// pop top body off the queue
			RigidBody &body = *queue.pop_front(); // if we use pop_back then this becomes a depth first search (DFS)

			// skip static bodies
			if (body.mType == RigidBody::STATIC)
				continue;

			island.Add(body);

			if (!body.IsAwake())
			{
				// make sure the body stays awake
				body.SetAwake(true);
			}

			if (body.mFlags.Test(PhysicsFlags::CAN_SLEEP)
				&& body.mLinearVelocity.LengthSq() < sleepLinearThresholdSQ
				&& body.mAngularVelocity.LengthSq() < sleepAngularThresholdSQ)
				body.mSleepTimer += timeStep.dt;
			else
				body.mSleepTimer = 0.0f;

			minSleepTime = Min(minSleepTime, body.mSleepTimer);

			// add joints to island (loop through joint edges)
			for (auto &edge : body.mJointEdges)
			{
				Joint &joint = *edge.joint;
				if (!joint.mFlags.Test(PhysicsFlags::ISLAND))
				{
					// add other rigid body (if existent) to island
					if (edge.other)
					{
						RigidBody &otherBody = *edge.other;
						if (!otherBody.mFlags.Test(PhysicsFlags::ISLAND) && otherBody.mType != RigidBody::STATIC)
						{
							otherBody.mFlags.Set(PhysicsFlags::ISLAND);
							queue.push_back(&otherBody);
						}
					}

					// add joint to island
					joint.mFlags.Set(PhysicsFlags::ISLAND);
					island.Add(joint);
				}
			} // end of looping through joint edges

			// add contact manifolds to the island
			for (auto &collider : body.mColliders)
			{
				for (auto &edge : collider.mContactEdges)
				{
					ContactManifold &manifold = *edge.manifold;
					if (manifold.isColliding && !manifold.flags.Test(PhysicsFlags::ISLAND))
					{
						// add other rigid body to island
						RigidBody &otherBody = *edge.other->mParent;
						if (!otherBody.mFlags.Test(PhysicsFlags::ISLAND) && otherBody.mType != RigidBody::STATIC)
						{
							otherBody.mFlags.Set(PhysicsFlags::ISLAND);
							queue.push_back(&otherBody);
						}

						// add manifold to island
						manifold.flags.Set(PhysicsFlags::ISLAND);
						if (!manifold.colliderA->mIsSensor && !manifold.colliderB->mIsSensor) // skip sensors
							island.Add(manifold);
					} // end of looping through contact edges
				} // end of looping through contact edges

			} // end of looping through colliders
		} // end of building island

		// solve island or put island to sleep
		if (minSleepTime < mParams.sleepTimeThreshold)
		{
			island.Step(timeStep);

			// post-solve
			for (auto &body : island.mRigidBodies)
			{
				// record energy (ignores 0.5 constant factor)
				if (body.mType != RigidBody::STATIC)
				{
					const MassData &m = body.mMassData;
					mStats.energy += m.mass * body.mLinearVelocity.LengthSq();
					mStats.energy += body.mAngularVelocity.Dot(m.unitLocalInertiaTensor * body.mAngularVelocity);
				}
			} // end of post-solve
		}
		else
			island.Sleep(timeStep);

		// update island stats
		timeStep.stats.minIslandSize = Min(island.NumRigidBodies(), timeStep.stats.minIslandSize);
		timeStep.stats.maxIslandSize = Max(island.NumRigidBodies(), timeStep.stats.maxIslandSize);
		++timeStep.stats.islands;
	}
	if (!timeStep.stats.islands)
		timeStep.stats.minIslandSize = timeStep.stats.maxIslandSize = 0;
	timeStep.stats.avgIslandSize = timeStep.stats.islands ? static_cast<float>(timeStep.stats.rigidBodies) / timeStep.stats.islands : 0.0f;

}