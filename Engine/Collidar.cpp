#include "stdafx.h"
#include "Collidar.h"
#include "SphereShape.h"
#include "QuadraShape.h"
#include "PolyheShape.h"
#include "BoxShape.h"
#include "MeshShape.h"
#include "RigidBody.h"
#include "PhysicsScene.h"

Collider::Collider(const ColliderDef &def, ColliderGeometry *geom, RigidBody* rigidBody)
: mUserData(def.mUserData)
, mFilter(def.filter)
, mMaterial(def.mMaterial)
, mIsSensor(def.mIsSensor)
, mGeometry(geom)
{
	mParent = rigidBody;
	mGeometry->mParent = this;
	mGeometry->UpdateMassAndLocalCentroid(mMaterial, mMassData.mass, mMassData.unitLocalInertiaTensor, mLocalCentroid);
	mMassData.inverseMass = 1.0f / mMassData.mass;
	mProxy.parent = this;
}

Collider::~Collider(void)
{
	while (!mContactEdges.empty())
	{
		ContactEdge &edge = *mContactEdges.begin();
		edge.other->mParent;
		edge.other->mParent->SetAwake(true);
		edge.manifold->~ContactManifold();
		mParent->mWorld->mManifoldAllocator.Free(edge.manifold);
	}
}

const Collider::ContactEdgeList &Collider::ContactEdges(void) const
{
	return mContactEdges;
}

void Collider::Destroy(Collider *collider, FixedSizeAllocator &colliderAllocator, Allocator &geometryAllocator)
{
	ColliderGeometry *geo = collider->mGeometry;
	unsigned geoSize = 0;
	geo->Dispose();
	switch (geo->mType)
	{
	case ColliderType::POLYHEDRON: geoSize = sizeof(PolyhedronColliderGeometry); break;
	case ColliderType::BOX: geoSize = sizeof(BoxColliderGeometry); break;
	case ColliderType::MESH: geoSize = sizeof(MeshColliderGeometry3D); break;
	case ColliderType::SPHERE: geoSize = sizeof(SphereColliderGeometry3D); break;
	case ColliderType::QUAD: geoSize = sizeof(QuadColliderGeometry3D); break;
	default: _asm int 3;
	}
	geo->~ColliderGeometry();
	geometryAllocator.Free(geo, geoSize);
	collider->~Collider();
	colliderAllocator.Free(collider);
}

Collider *Collider::Create(const ColliderDef &def, FixedSizeAllocator &colliderAllocator, Allocator &geometryAllocator, RigidBody* rigidBody)
{
	ColliderGeometry *geometry = nullptr;
	switch (def.mType)
	{
	case ColliderType::MESH:
	{
		geometry =
		 new (geometryAllocator.Allocate(sizeof(MeshColliderGeometry3D)))
		 MeshColliderGeometry3D(static_cast<const MeshColliderDef3D &>(def));
		break;
	}
	case ColliderType::QUAD:
	{
		geometry =
		  new (geometryAllocator.Allocate(sizeof(QuadColliderGeometry3D)))
		  QuadColliderGeometry3D(static_cast<const QuadColliderDef3D&>(def));
		break;
	}
	case ColliderType::SPHERE:
	{
		geometry =
		 new (geometryAllocator.Allocate(sizeof(SphereColliderGeometry3D)))
		 SphereColliderGeometry3D(static_cast<const SphereColliderDef3D &>(def));
		break;
	}
	case ColliderType::BOX:
	{
		geometry =
		  new (geometryAllocator.Allocate(sizeof(BoxColliderGeometry)))
		  BoxColliderGeometry(static_cast<const BoxColliderDef &>(def));
		break;
	}
	case ColliderType::POLYHEDRON:
	{
		geometry =
		 new (geometryAllocator.Allocate(sizeof(PolyhedronColliderGeometry)))
		 PolyhedronColliderGeometry(static_cast<const PolyhedronColliderDef &>(def));
		break;
	}
	default:
		_asm int 3;
	}
	Collider *collider =
		new (colliderAllocator.Allocate()) Collider(def, geometry, rigidBody);
	return collider;
}

void Collider::SetMass(float mass)
{
	mMassData.mass = mass;
	mMassData.inverseMass = 1.0f / mMassData.mass;
	// TODO
	if (mParent) mParent->InvalidateColliders();
}

const Vector3 Collider::UpdateSize(const Vector3 &scale)
{
	Vector3 size = mGeometry->UpdateSize(scale);
	//m_geometry->UpdateMassAndLocalCentroid(m_material, m_massData.mass, m_massData.unitLocalInertiaTensor, m_localCentroid);
	return size;
}

bool Collider::RayCastCollider(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const
{
	return mGeometry->RayCast(ray, maxDistance, t, n);
}

void Collider::UpdateProxy(void)
{
	mGeometry->UpdateProxy(*mParent, mProxy);
}

void Collider::Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const
{}

bool Collider::CollidesCollider(const Math::Vector3 &pos) const
{
	return mGeometry->Collides(pos);
}

ColliderDef::ColliderDef(ColliderType::value_type type)
: mType(type)
, mUserData(nullptr)
, mIsSensor(false)
{ 
	mMaterial;
	//mMaterial.mRestitution = 0;
	int i = 0;
}
