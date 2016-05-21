#include "stdafx.h"
#include "PolyheShape.h"
#include "RigidBody.h"
#include "RigidBodyCom.h"
#include "CPhysics.h"
#include "InertialTensor.h"
#include "AdjancencyInfo.h"
const Math::Vector3 PolyhedronColliderGeometry::Support(const Math::Vector3 &dir) const
{
	return mAdjacency.Support(dir);
}

bool PolyhedronColliderGeometry::RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const
{
	Ray3 localRay;
	auto &body = mParent->Parent();
	localRay.pos = body.GlobalToLocal(ray.pos);
	localRay.dir = body.GlobalToLocalVec(ray.dir);
	const Math::Vector3 &p = localRay.pos;
	const Math::Vector3 &d = maxDistance * localRay.dir;
	const Math::Vector3 q = p + d;

	float tEnter = 0.0f;
	float tExit = 1.0f;

	auto &verts = mAdjacency.Verts();
	auto &edges = mAdjacency.Edges();
	auto &faces = mAdjacency.Faces();
	for (auto &face : faces)
	{
		if (!face.active)
			continue;

		// triangle edges
		auto &edge0 = edges[face.edge];
		auto &edge1 = edges[edge0.next];
		auto &edge2 = edges[edge1.next];

		// triangle verts & normal
		const Math::Vector3 &a = verts[edge0.vert].position;
		const Math::Vector3 &b = verts[edge1.vert].position;
		const Math::Vector3 &c = verts[edge2.vert].position;
		const Math::Vector3 normal = (b - a).Cross(c - a);

		float denom = normal.Dot(d);
		float dist = normal.Dot(p - a); // positive: outside plane

		// test if segment runs parallel to the plane
		if (std::fabs(denom) < EPSILON && dist > 0.0f)
			return false;

		const float tempT = -dist / denom;
		if (denom < -EPSILON)
		{
			if (tempT > tEnter)
			{
				n = normal;
				tEnter = tempT;
			}
		}
		else if (denom > EPSILON)
		{
			tExit = (tExit < tempT) ? tExit : tempT;
		}

		// early out
		if (tEnter > tExit) return false;
	}

	n = body.LocalToGlobalVec(n);
	n.Normalize();
	t = tEnter;
	return true;
}

bool PolyhedronColliderGeometry::Collides(const Math::Vector3 &pos) const
{
	auto &verts = mAdjacency.Verts();
	auto &edges = mAdjacency.Edges();
	auto &faces = mAdjacency.Faces();
	for (auto &face : faces)
	{
		// grab face verts
		int e = face.edge;
		const Math::Vector3 &v0 = verts[edges[e].vert].position;
		e = edges[e].next;
		const Math::Vector3 &v1 = verts[edges[e].vert].position;
		e = edges[e].next;
		const Math::Vector3 &v2 = verts[edges[e].vert].position;

		// compute signed distance from face (positive: outside, negative: inside)
		if ((pos - v0).Dot((v1 - v0).Cross(v2 - v0)) >= 0.0f)
			return false;
	}
	return true;
}

void PolyhedronColliderGeometry::UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid)
{
	auto &verts = mAdjacency.Verts();
	auto &edges = mAdjacency.Edges();
	auto &faces = mAdjacency.Faces();
	const Math::Vector3 &c = mAdjacency.Centroid();

	mass = 0.0f;
	unitInertiaTensor.ZeroOut();
	localCentroid.ZeroOut();

	float totalVolume = 0.0f;
	for (auto &face : faces)
	{
		if (!face.active)
			continue;

		// grab face verts
		int e = face.edge;
		const Math::Vector3 &v0 = verts[edges[e].vert].position;
		e = edges[e].next;
		const Math::Vector3 &v1 = verts[edges[e].vert].position;
		e = edges[e].next;
		const Math::Vector3 &v2 = verts[edges[e].vert].position;

		// volume of tetrahedron formed by face & centroid (not divided by 6)
		const float tetrahedronVolume = std::fabs((v0 - c).Dot((v1 - c).Cross(v2 - c)));

		// accumulate volume
		totalVolume += tetrahedronVolume;

		// accumulate weighted centroid
		const Math::Vector3 tetrahedronCentroid = 0.25f * (c + v0 + v1 + v2);
		localCentroid += tetrahedronVolume * tetrahedronCentroid;
	}
	if (totalVolume == 0)
		totalVolume = 1;
	localCentroid /= totalVolume;
	
	//mass = material.mDensity * totalVolume / 6.0f;
	mass = mParent->Parent().mParent->cphy->mMass * totalVolume / 6.0f;
	

	// compute inertia tensor
	for (auto &face : faces)
	{
		if (!face.active)
			continue;

		// grab face verts
		int e = face.edge;
		const Math::Vector3 &v0 = verts[edges[e].vert].position;
		e = edges[e].next;
		const Math::Vector3 &v1 = verts[edges[e].vert].position;
		e = edges[e].next;
		const Math::Vector3 &v2 = verts[edges[e].vert].position;

		// accumulate inertia tensor
		unitInertiaTensor += UnitInertiaTensorTetrahedron(c, v0, v1, v2, localCentroid);
	}
}

void PolyhedronColliderGeometry::UpdateAABB(const RigidBody &body, Proxy &proxy)
{
	// POSSIBLE OPTIMIZATION: use cached local AABB to produce a fat AABB


	const Math::Vector3 k_xNeg = body.GlobalToLocalVec(Math::Vector3(-1.0f, 0.0, 0.0f));
	const Math::Vector3 k_yNeg = body.GlobalToLocalVec(Math::Vector3(0.0f, -1.0f, 0.0f));
	const Math::Vector3 k_zNeg = body.GlobalToLocalVec(Math::Vector3(0.0f, 0.0f, -1.0f));
	const Math::Vector3 k_xPos = body.GlobalToLocalVec(Math::Vector3(1.0f, 0.0f, 0.0f));
	const Math::Vector3 k_yPos = body.GlobalToLocalVec(Math::Vector3(0.0f, 1.0f, 0.0f));
	const Math::Vector3 k_zPos = body.GlobalToLocalVec(Math::Vector3(0.0f, 0.0f, 1.0f));

	CMeshRenderer* mesh = static_cast<CMeshRenderer*>(mParent->Parent().mParent->cphy->gameObject->GetComponentByName("CMeshRenderer"));
	//CTransform* ct = mParent->Parent().mParent->cphy->gameObject->transform;
	//
	//AABB aabb = mesh->GetMesh()->GetAABB();
	//proxy.maxPoint = aabb.maxPoint * ct->scale + ct->position;
	//proxy.minPoint = aabb.minPoint * ct->scale + ct->position;

	//if (mesh)
		//mAdjacency.mCentroid = mesh->GetMesh()->GetOrigin();

	Vector3 s = body.mScale;
	Matrix3 r = body.mOrientation;
	Vector3 t = body.mPosition;
	Vector3 xNeg = body.LocalToGlobal(Support(k_xNeg));

	Vector3 yNeg = body.LocalToGlobal(Support(k_yNeg));
	Vector3 zNeg = body.LocalToGlobal(Support(k_zNeg));
	proxy.minPoint.Set(xNeg.x,
		yNeg.y,
		zNeg.z);

	proxy.maxPoint.Set(body.LocalToGlobal(Support(k_xPos)).x,
		body.LocalToGlobal(Support(k_yPos)).y,
		body.LocalToGlobal(Support(k_zPos)).z);
}

void PolyhedronColliderGeometry::UpdateProxy(const RigidBody &body, Proxy &proxy)
{
	if (body.mParent->cphy->mColliderType != 0 || body.mParent->cphy->gameObject->GetFlag("Kinetic"))
		UpdateAABB(body, proxy);
	else
	{
		if (mDoItOnce)
		{
			UpdateAABB(body, proxy);
			mDoItOnce = false;
		}
	}
}

void PolyhedronColliderGeometry::Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const
{
	const RigidBody &body = mParent->Parent();
	const auto &verts = mAdjacency.Verts();
	const auto &edges = mAdjacency.Edges();
	const auto &faces = mAdjacency.Faces();

	for (auto &face : faces)
	{
		if (!face.active)
			continue;

		int e = face.edge;
		const int startEdge = e;
		do
		{
			const auto &edge = edges[e];
			const auto &next = edges[edge.next];

			debugDraw.DrawLine(body.LocalToGlobal(verts[edge.vert].position),
				body.LocalToGlobal(verts[next.vert].position),
				lineColor);

			e = edge.next;
		} while (e != startEdge);
	}
}

const Vector3& PolyhedronColliderGeometry::UpdateSize(const Vector3 &scale)
{
	//TODOKai
	mScale = scale;
	return scale;
}

