/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */
#pragma once
#include "stdafx.h"
#include "SupportFunction.h"
#include "Simplex.h"
#include "Ray3.h"
#include "AABBTree.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "SphereShape.h"
#include "BoxShape.h"
#include "QuadraShape.h"
#include "MeshShape.h"
#include "GJK.h"
#include "Mesh.h"
#include "AdjancencyInfo.h"

Point3 supportUsed(CollisionShape* CollidarA, CollisionShape* CollidarB, Math::Vector3& dir)
{
	static Point3 FarestPtA;
	static Point3 FarestPtB;
	static Math::Vector3 localDirA;
	static Math::Vector3 localDirB;
	CPhysics* cphyA;
	cphyA = CollidarA->cphysics;
	CPhysics* cphyB = CollidarB->cphysics;

	localDirA = cphyA->GlobalToLocalVec(dir);
	localDirB = cphyB->GlobalToLocalVec(-dir);

	FarestPtA = cphyA->LocalToGlobalPt(FarestPtA);
	FarestPtB = cphyB->LocalToGlobalPt(FarestPtB);

	static Math::Vector3 retVec;

	Point3 ret(retVec.x, retVec.y, retVec.z);
	return ret;
}

Math::Vector3& tripleProductUsed(Math::Vector3& vec1, Math::Vector3& vec2, Math::Vector3& vec)
{
	// A X B X C = B(A dot C) - A(B dot C)
	return vec2 * vec1.Dot(vec) - vec1 * vec2.Dot(vec);;
}

bool containOrigin2DUsed(Simplex& simplex, Math::Vector3& dir)
{
	Point3 a;
	a = simplex.GetLast();
	Math::Vector3 ao(-a.x, -a.y, 0);
	Point3 b;
	Math::Vector3 ab;
	float result;

	if (simplex.Size() == 2)
	{
		b = simplex.GetC();
		//ab = b - a;
		result = ab.Dot(ao);
		if (result > 0)
			int i = 0;//dir = tripleProduct(ab, ao, ab);

		else
			dir = ao;
		return false;
	}
	else
	{
		b = simplex.GetB();
		//ab = b - a;
		Point3 c;
		c = simplex.GetC();

		Math::Vector3 ac;
		//ac = c - a;

		Math::Vector3 abc;
		abc = ab.Cross(ac);

		Math::Vector3 abcCrossAC;
		abcCrossAC = abc.Cross(ac);
		result = abcCrossAC.Dot(ao);
		if (result > 0)
		{
			result = ac.Dot(ao);
			if (result > 0)
				int i = 0;
				//dir = tripleProduct(ac, ao, ac);
			else
			{
				result = ab.Dot(ao);
				if (result > 0)
					int i = 0;
					//dir = tripleProduct(ab, ao, ab);
				else
					dir = ao;
			}
			simplex.Remove(1); // remove B
			return false;
		}
		else
		{
			abcCrossAC = ab.Cross(abc);
			result = abcCrossAC.Dot(ao);
			if (result > 0)
			{
				result = ab.Dot(ao);
				if (result > 0)
					int i = 0; //dir = tripleProduct(ab, ao, ab);
				else
					dir = ao;
				simplex.Remove(0); // remove C
				return false;
			}
			else
			{
				dir = abc;
				return true;
			}
		}

	}
}

const Math::Vector3 ClosestPointLineSegment(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b)
{
	float t;
	return ClosestPointLineSegment(p, a, b, t);
}

const Math::Vector3 ClosestPointLineSegment(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, float &t)
{
	const Math::Vector3 lineVec = b - a;
	const float lineVec_lenSq = lineVec.LengthSq();
	t = Clamp(lineVec.Dot(p - a) / lineVec_lenSq, 0.0f, 1.0f);
	return a + lineVec * t;
}

bool PointOutsideOfPlane(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c)
{
	return (b - a).Cross(c - a).Dot(p - a) > 0.0f;
}

const Math::Vector3 ClosestPointTriangle(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c)
{
	// Check if P in vertex region outside A
	const Math::Vector3 ab = b - a;
	const Math::Vector3 ac = c - a;
	const Math::Vector3 ap = p - a;
	float d1 = ab.Dot(ap);
	float d2 = ac.Dot(ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)
	// Check if P in vertex region outside B
	const Math::Vector3 bp = p - b;
	float d3 = ab.Dot(bp);
	float d4 = ac.Dot(bp);
	if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)
	// Check if P in edge region of AB, if so return projection of P onto AB
	const float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return a + ab * v; // barycentric coordinates (1-v,v,0)
	}
	// Check if P in vertex region outside C
	const Math::Vector3 cp = p - c;
	const float d5 = ab.Dot(cp);
	const float d6 = ac.Dot(cp);
	if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)
	// Check if P in edge region of AC, if so return projection of P onto AC
	const float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		const float w = d2 / (d2 - d6);
		return a + ac * w; // barycentric coordinates (1-w,0,w)
	}
	// Check if P in edge region of BC, if so return projection of P onto BC
	const float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		const float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + (c - b) * w; // barycentric coordinates (0,1-w,w)
	}
	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	const float denom = 1.0f / (va + vb + vc);
	const float v = vb * denom;
	const float w = vc * denom;
	return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

 
const Math::Vector3 ClosestPointTetrahedron(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c, const Math::Vector3 &d)
{
	// windinw assumed: (a - d).Dot((b - d).Cross(c - d)) >= 0.0f;

	// Start out assuming point inside all halfspaces, so closest to itself
	Math::Vector3 closest = p;
	float bestSqDist = FLT_MAX;
	// If point outside face abc then compute closest point on abc
	if (PointOutsideOfPlane(p, a, b, c))
	{
		const Math::Vector3 q = ClosestPointTriangle(p, a, b, c);
		const float sqDist = (q - p).LengthSq();
		// Update best closest point if (squared) distance is less than current best
		if (sqDist < bestSqDist) bestSqDist = sqDist, closest = q;
	}
	// Repeat test for face acd
	if (PointOutsideOfPlane(p, a, c, d))
	{
		const Math::Vector3 q = ClosestPointTriangle(p, a, c, d);
		const float sqDist = (q - p).LengthSq();
		if (sqDist < bestSqDist) bestSqDist = sqDist, closest = q;
	}
	// Repeat test for face adb
	if (PointOutsideOfPlane(p, a, d, b))
	{
		const Math::Vector3 q = ClosestPointTriangle(p, a, d, b);
		const float sqDist = (q - p).LengthSq();
		if (sqDist < bestSqDist) bestSqDist = sqDist, closest = q;
	}
	// Repeat test for face bdc
	if (PointOutsideOfPlane(p, b, d, c))
	{
		const Math::Vector3 q = ClosestPointTriangle(p, b, d, c);
		const float sqDist = (q - p).LengthSq();
		if (sqDist < bestSqDist) bestSqDist = sqDist, closest = q;
	}
	return closest;
}

const Math::Vector3 ClosestPointLine(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b)
{
	float t;
	return ClosestPointLine(p, a, b, t);
}

const Math::Vector3 ClosestPointLine(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, float &t)
{
	const Math::Vector3 lineVec = b - a;
	const float lineVec_lenSq = lineVec.LengthSq();
	t = lineVec.Dot(p - a) / lineVec_lenSq;
	return a + lineVec * t;
}

int LeastSignificantComponent(const Math::Vector3 &v)
{
	const float absX = std::fabs(v.x);
	const float absY = std::fabs(v.y);
	const float absZ = std::fabs(v.z);

	if (absX < absY)
	{
		if (absX < absZ)
			return 0; // X < Y, X < Z
		else
			return 2; // Z <= X < Y
	}
	else
	{
		if (absY < absZ)
			return 1; // Y <= X, Y < Z
		else
			return 2; // Z <= Y < X
	}
}

void BarycentricCoordinates(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c, float &u, float &v, float &w)
{
	const Math::Vector3 v0 = b - a, v1 = c - a, v2 = p - a;
	const float d00 = v0.Dot(v0);
	const float d01 = v0.Dot(v1);
	const float d11 = v1.Dot(v1);
	const float d20 = v2.Dot(v0);
	const float d21 = v2.Dot(v1);
	const float denom_inv = 1.0f / (d00 * d11 - d01 * d01);
	v = (d11 * d20 - d01 * d21) * denom_inv;
	w = (d00 * d21 - d01 * d20) * denom_inv;
	u = 1.0f - v - w;
}


RigidBodyComponent *CreateRigidBody(unsigned char bodyType)
{
	static int s_counter = 0;
	char name[32];
	std::sprintf(name, "obj_%d", s_counter++);

	RigidBodyComponent* newRigidBody = new RigidBodyComponent();
	newRigidBody->SetType(bodyType);
	return newRigidBody;
}
CBoxColliderComponent *CreateBoxCollider(Math::Vector3 halfExtend)
{
	CBoxColliderComponent* newCollidar = new CBoxColliderComponent();
	newCollidar->SetHalfExtents(halfExtend);
	return newCollidar;
}
CSphereColliderComponent *CreateSphereCollider(const float radius)
{
	CSphereColliderComponent* newCollidar = new CSphereColliderComponent();
	newCollidar->SetRadius(radius);
	return newCollidar;
}

CMeshColliderComponent * CreateMeshCollider(const std::string& filename)
{
	CMeshColliderComponent* newCollidar = new CMeshColliderComponent();
	newCollidar->SetMesh(filename);
	return newCollidar;
}

void AABBFromModelSapceToWorldSpace(AABB& aabb, CTransform& transform)
{
	//Matrix4 T = transform.GetLocalTranslationMatrix();
	//Matrix4 R = transform.GetLocalRotationMatrix();
	//Matrix4 S = transform.GetLocalScaleMatrix();
	//Matrix4 W = transform.GetWorldMatrix();
  //
	//const Math::Vector3 c = ToMatrix3(transform.GetLocalRotationMatrix()) * ((0.5f * (aabb.minPoint + aabb.maxPoint)) * transform.scale) + transform.position(); // proxy center
	//const Math::Vector3 h = TransformPoint(S, 0.5f * (aabb.maxPoint - aabb.minPoint)); // proxy half extents
  //
	//Vector3 axis[3] = { Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1) };
	//// get new axis
	//for (int i = 0; i < 3; ++i)
	//	axis[0] = TransformPoint(R, axis[0]);
  //
	//Vector3 newVerts[8];
	//// + + +
	//newVerts[0] = c + h.x * axis[0] + h.y * axis[1] + h.z * axis[2];
	//// + + -
	//newVerts[1] = c + h.x * axis[0] + h.y * axis[1] - h.z * axis[2];
	//// + - +
	//newVerts[2] = c + h.x * axis[0] - h.y * axis[1] + h.z * axis[2];
	//// - + +
	//newVerts[3] = c - h.x * axis[0] + h.y * axis[1] + h.z * axis[2];
	//// - - +
	//newVerts[4] = c - h.x * axis[0] - h.y * axis[1] + h.z * axis[2];
	//// - + -
	//newVerts[5] = c - h.x * axis[0] + h.y * axis[1] - h.z * axis[2];
	//// + - -
	//newVerts[6] = c + h.x * axis[0] - h.y * axis[1] - h.z * axis[2];
	//// - - -
	//newVerts[7] = c - h.x * axis[0] - h.y * axis[1] - h.z * axis[2];

  Vector3 origin = 0.5f * (aabb.minPoint + aabb.maxPoint);
  Quaternion rot = transform.GetRotationQuaternion();
  Vector3 scaledHalfVec = aabb.maxPoint - origin;
  scaledHalfVec *= transform.scale;
  origin *= transform.scale;

  Vector3 rotatedX = rot.RotatedVector(Vector3::cXAxis * scaledHalfVec.x);
  Vector3 rotatedY = rot.RotatedVector(Vector3::cYAxis * scaledHalfVec.y);
  Vector3 rotatedZ = rot.RotatedVector(Vector3::cZAxis * scaledHalfVec.z);
  rot.RotateVector(&origin);
  origin += transform.GetGlobalPosition();

  Vector3 newVerts[8];
  // + + +
  newVerts[0] = origin + rotatedX + rotatedY + rotatedZ;
  // + + -
  newVerts[1] = origin + rotatedX + rotatedY - rotatedZ;
  // + - +
  newVerts[2] = origin + rotatedX - rotatedY + rotatedZ;
  // - + +
  newVerts[3] = origin - rotatedX + rotatedY + rotatedZ;
  // - - +
  newVerts[4] = origin - rotatedX - rotatedY + rotatedZ;
  // - + -
  newVerts[5] = origin - rotatedX + rotatedY - rotatedZ;
  // + - -
  newVerts[6] = origin + rotatedX - rotatedY - rotatedZ;
  // - - -
  newVerts[7] = origin - rotatedX - rotatedY - rotatedZ;

	// update aabb
	aabb.minPoint.x =  FLT_MAX; aabb.minPoint.y =  FLT_MAX; aabb.minPoint.z =  FLT_MAX;
	aabb.maxPoint.x = -FLT_MAX; aabb.maxPoint.y = -FLT_MAX; aabb.maxPoint.z = -FLT_MAX;
	for (int i = 0; i < 8; ++i)
	{
		if (newVerts[i].x > aabb.maxPoint.x)
			aabb.maxPoint.x = newVerts[i].x;

		if (newVerts[i].y > aabb.maxPoint.y)
			aabb.maxPoint.y = newVerts[i].y;

		if (newVerts[i].z > aabb.maxPoint.z)
			aabb.maxPoint.z = newVerts[i].z;

		if (newVerts[i].x < aabb.minPoint.x)
			aabb.minPoint.x = newVerts[i].x;

		if (newVerts[i].y < aabb.minPoint.y)
			aabb.minPoint.y = newVerts[i].y;

		if (newVerts[i].z < aabb.minPoint.z)
			aabb.minPoint.z = newVerts[i].z;
	}
}

void SwapMaxAndMin(float& max, float& min)
{
	if (max < min)
	{
		float temp = min;
		min = max;
		max = temp;
	}
}

void CorrectAABBMinAndMax(AABB& aabb)
{
	SwapMaxAndMin(aabb.maxPoint.x, aabb.minPoint.x);
	SwapMaxAndMin(aabb.maxPoint.y, aabb.minPoint.y);
	SwapMaxAndMin(aabb.maxPoint.z, aabb.minPoint.z);
}
bool RayAABBWithTime(const Ray3 &ray, const AABB &aabb, float maxDistance, float &t, bool considerInside)
{
	// Gino Algorithm 3.3 (p.75)
	const Math::Vector3 c = 0.5f * (aabb.minPoint + aabb.maxPoint); // proxy center
	const Math::Vector3 h = 0.5f * (aabb.maxPoint - aabb.minPoint); // proxy half extents
	const Math::Vector3 source = ray.pos - c;                         // ray source (in proxy space)
	const Math::Vector3 target = ray.pos + maxDistance * ray.dir - c; // ray target (in proxy space)

	const unsigned char bitsS = OutCode(source, h);
	const unsigned char bitsT = OutCode(target, h);

	if (!(bitsS & bitsT)) // none of the side planes separate the ray from the proxy
	{
		float tEnter = 0.0f;
		float tExit = 1.0f;
		unsigned char bit = 1;
		for (unsigned char i = 0; i < 3; ++i)
		{
			if (bitsS & bit) // point of intersection is entering
				tEnter = MaxFLT(tEnter, (h[i] - source[i]) / (target[i] - source[i]));
			else if (bitsT & bit) // point of intersection is exiting
				tExit = MinFLT(tExit, (h[i] - source[i]) / (target[i] - source[i]));
			bit <<= 1;
			if (bitsS & bit) // point of intersection is entering
				tEnter = MaxFLT(tEnter, (-h[i] - source[i]) / (target[i] - source[i]));
			else if (bitsT & bit) // point of intersection is exiting
				tExit = MinFLT(tExit, (-h[i] - source[i]) / (target[i] - source[i]));
			bit <<= 1;
		}
		if (considerInside && tEnter == 0.0f) // starting from inside
		{
			t = tExit;
			return true;
		}
		if (tEnter < tExit + EPSILON) // only return positive hit if ray is REALLY ENTERING
		{
			t = tEnter;
			return true;
		}
	}

	return false;
}

bool RayAABB(const Ray3 &ray, const AABB &aabb, float maxDistance, float& potentialZ)
{
	// SAT
	const Math::Vector3 c = 0.5f * (aabb.minPoint + aabb.maxPoint); // proxy center
	const Math::Vector3 h = 0.5f * (aabb.maxPoint - aabb.minPoint); // proxy half extents
	const Math::Vector3 s = ray.pos - c;                              // ray source (in proxy space)
	const Math::Vector3 t = ray.pos + maxDistance * ray.dir - c;      // ray target (in proxy space)

	// test major axes
	//for (unsigned char i = 0; i < 3; ++i)
	//{
	//	if (s[i] < -h[i] && t[i] < -h[i])
	//		return false;
	//	if (s[i] > h[i] && t[i] > h[i])
	//		return false;
	//}

	// test cross products between ray and major axes
	static const Math::Vector3 majorAxes[] =
	{
		Math::Vector3(1.0f, 0.0f, 0.0f),
		Math::Vector3(0.0f, 1.0f, 0.0f),
		Math::Vector3(0.0f, 0.0f, 1.0f)
	};
	for (unsigned char i = 0; i < 3; ++i)
	{
		const Math::Vector3 &majorAxis = majorAxes[i];
		const Math::Vector3 testAxis = ray.dir.Cross(majorAxis);

		float extentDot = std::fabs(h.x * testAxis.x) + std::fabs(h.y * testAxis.y) + std::fabs(h.z * testAxis.z);
		float rayDot0 = testAxis.Dot(s);
		float rayDot1 = testAxis.Dot(t);
		if (rayDot0 > rayDot1)
			std::swap(rayDot0, rayDot1);

		if (rayDot0 > extentDot || rayDot1 < -extentDot)
			return false;
	}

	//float dist = (s - c).Length();
	//if (dist < potentialZ)
    //{ 
    //  potentialZ = dist;
      return true;
    //}	
	//else
	//  return false;
}


// ray-proxy tests
//---------------------------------------------------------------------------

// [-z][+z][-y][+y][-x][+x]
//  32  16   8   4   2   1
const unsigned char OUT_CODE_ZN = 32u;
const unsigned char OUT_CODE_ZP = 16u;
const unsigned char OUT_CODE_YN = 8u;
const unsigned char OUT_CODE_YP = 4u;
const unsigned char OUT_CODE_XN = 2u;
const unsigned char OUT_CODE_XP = 1u;

unsigned char OutCode(const Math::Vector3 &p, const Math::Vector3 &halfExtents)
{
	unsigned char bits = 0u;
	if (p.x >= halfExtents.x)
		bits |= OUT_CODE_XP;
	if (p.x <= -halfExtents.x)
		bits |= OUT_CODE_XN;
	if (p.y >= halfExtents.y)
		bits |= OUT_CODE_YP;
	if (p.y <= -halfExtents.y)
		bits |= OUT_CODE_YN;
	if (p.z >= halfExtents.z)
		bits |= OUT_CODE_ZP;
	if (p.z <= -halfExtents.z)
		bits |= OUT_CODE_ZN;
	return bits;
}


bool RayBox(const Ray3 &ray, const Math::Vector3 &center, const Math::Vector3 &halfExtents, float maxDistance, float &t, Math::Vector3 &n)
{
	// Gino Algorithm 3.3 (p.75)
	const Math::Vector3 c = center;
	const Math::Vector3 h = halfExtents;
	const Math::Vector3 source = ray.pos - c;                         // ray source (in proxy space)
	const Math::Vector3 target = ray.pos + maxDistance * ray.dir - c; // ray target (in proxy space)

	const unsigned char bitsS = OutCode(source, h);
	const unsigned char bitsT = OutCode(target, h);

	if (!(bitsS & bitsT)) // none of the side planes separate the ray from the proxy
	{
		bool hit = false;
		float tEnter = 0.0f;
		float tExit = FLT_MAX;
		unsigned char bit = 1;
		unsigned char hitBit;
		for (unsigned char i = 0; i < 2; ++i)
		{
			if (bitsS & bit) // point of intersection is entering
				tEnter = MaxFLT(tEnter, (h[i] - source[i]) / (target[i] - source[i]));
			else if (bitsT & bit) // point of intersection is exiting
				tExit = MinFLT(tExit, (h[i] - source[i]) / (target[i] - source[i]));
			bit <<= 1;
			if (bitsS & bit) // point of intersection is entering
				tEnter = MaxFLT(tEnter, (-h[i] - source[i]) / (target[i] - source[i]));
			else if (bitsT & bit) // point of intersection is exiting
				tExit = MinFLT(tExit, (-h[i] - source[i]) / (target[i] - source[i]));
			bit <<= 1;
		}
		if (tEnter < tExit + EPSILON) // only return positive hit if ray is REALLY ENTERING
		{
			t = tEnter;
			return true;
			if (hit)
			{
				t = tEnter;
				switch (hitBit)
				{
				case OUT_CODE_XP:
					n.Set(1.0f, 0.0f, 0.0f);
					break;
				case OUT_CODE_XN:
					n.Set(-1.0f, 0.0f, 0.0f);
					break;
				case OUT_CODE_YP:
					n.Set(0.0f, 1.0f, 0.0f);
					break;
				case OUT_CODE_YN:
					n.Set(0.0f, -1.0f, 0.0f);
					break;
				case OUT_CODE_ZP:
					n.Set(0.0f, 0.0f, 1.0f);
					break;
				case OUT_CODE_ZN:
					n.Set(0.0f, 0.0f, -1.0f);
					break;
				}
				return true;
			}
		}
	}

	return false;
}

//---------------------------------------------------------------------------
// end of ray-proxy tests


// triangle tests
//---------------------------------------------------------------------------

// Real-Time Collision Detection by Christer Ericson p.191
bool RayTriangle(const Ray3 &ray, float maxDistance, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c, float &t)
{
	Math::Vector3 p = ray.pos;
	Math::Vector3 q = p + ray.dir * maxDistance;
	const Math::Vector3 ab = b - a;
	const Math::Vector3 ac = c - a;
	const Math::Vector3 n = ab.Cross(ac);
	if ((p - a).Dot(n) < 0.0f)
		std::swap(p, q);
	const Math::Vector3 qp = p - q;

	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = qp.Dot(n);
	if (d <= 0.0f) return false;

	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle
	const Math::Vector3 ap = p - a;
	t = ap.Dot(n);
	if (t < 0.0f) return false;
	if (t > d) return false;
	// Compute barycentric coordinate components and test if within bounds
	const Math::Vector3 e = qp.Cross(ap);
	const float v = ac.Dot(e);
	if (v < 0.0f || v > d) return false;
	const float w = -ab.Dot(e);
	if (w < 0.0f || v + w > d) return false;
	// Segment/ray intersects triangle.
	t /= d;
	return true;
}

//---------------------------------------------------------------------------
// end of triangle tests


// physics collision detection
//---------------------------------------------------------------------------

static bool Unimplemented(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideGeneric(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideSpheres(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideSphereQuad(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideQuadSphere(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);

typedef bool(*CollisionCallback) (ContactManifold &, const ColliderGeometry &, const ColliderGeometry &, FixedSizeAllocator &);

static CollisionCallback s_collisionCallbacks[5][5] =
{
	//sphere            // quad            //polyhedron    // box          // mesh
	{ CollideSpheres, CollideSphereQuad, CollideGeneric, CollideGeneric, CollideGeneric }, // sphere
	{ CollideQuadSphere, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // quad
	{ CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // polyhedron
	{ CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // box
	{ CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // mesh
};

bool Collide(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	bool result = false;

	const RigidBody &bodyA = manifold.colliderA->Parent();
	const RigidBody &bodyB = manifold.colliderB->Parent();

	if (geomA.mType == ColliderType::DEFAULT
		|| geomB.mType == ColliderType::DEFAULT)
		result = CollideGeneric(manifold, geomA, geomB, contactAllocator);
	else
		result = s_collisionCallbacks[geomA.mType][geomB.mType](manifold, geomA, geomB, contactAllocator);

	if (result)
	{
		// prevent spheres from rolling due to contact caching
		if (geomA.mType == ColliderType::SPHERE || geomB.mType == ColliderType::SPHERE)
			manifold.invalidateExistingContacts = true;

		for (unsigned char i = 0; i < manifold.numContacts; ++i)
		{
			Contact &contact = *manifold.contacts[i];

			// compute local points
			//if (bodyA.mParent->cphy->collidarCom->GetCollider().mGeometry->mType == 3)
			//	contact.localPositionA = bodyA.GlobalToLocalNoScale(contact.globalPositionA);
			//else
				contact.localPositionA = bodyA.GlobalToLocal(contact.globalPositionA);
			//if (bodyB.mParent->cphy->collidarCom->GetCollider().mGeometry->mType == 3)
			//	contact.localPositionB = bodyB.GlobalToLocalNoScale(contact.globalPositionB);
			//else
				contact.localPositionB = bodyB.GlobalToLocal(contact.globalPositionB);

			// compute tangents from normals
			const float absX = std::fabs(contact.normal.x);
			const float absY = std::fabs(contact.normal.y);
			const float absZ = std::fabs(contact.normal.z);
			Math::Vector3 majorAxis(0.0f, 0.0f, 0.0f);
			if (absX < absY)
			{
				if (absX < absZ)
					majorAxis.x = 1.0f; // X < Y < Z, X < Z < Y
				else
					majorAxis.z = 1.0f; // Z < X < Y
			}
			else
			{
				if (absY < absZ)
					majorAxis.y = 1.0f; // Y < X < Z, Y < Z X
				else
					majorAxis.z = 1.0f; // Z < Y < X
			}
			contact.tangent1 = contact.normal.Cross(majorAxis).Normalized();
			contact.tangent2 = contact.normal.Cross(contact.tangent1).Normalized();
		}
	}
	return result;
}

static bool Unimplemented(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	//GV_ASSERT_MESSAGE(false, "Collision detection for this type pair is not implemented.");
	return false;
}


// GJK / EPA
//-------------------------------------------------------

static PhysicsDebugDraw *s_debugDraw = nullptr;
static int s_gjkIterations = 32;
static int s_epaIterations = 32;
static GjkSimplex s_simplex;
void SetCollisionDebugDraw(PhysicsDebugDraw &debugDraw)
{
	s_debugDraw = &debugDraw;
}
void SetGjkEpaIterations(int gjkIterations, int epaIteratinos)
{
	s_gjkIterations = gjkIterations;
	s_gjkIterations = epaIteratinos;
}
void DebugDrawGjk(void)
{
	if (s_debugDraw)
	{
		static unsigned colors[4] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00 };
		for (unsigned i = 0; i < s_simplex.numVerts; ++i)
		{
			s_debugDraw->DrawBox(s_simplex.verts[i], Math::Vector3(0.1f, 0.1f, 0.1f), Math::Matrix3(), colors[i]);
		}
		switch (s_simplex.numVerts)
		{
		case 2:
			s_debugDraw->DrawLine(s_simplex.verts[0], s_simplex.verts[1], 0xFF00FFFF);
			break;
		case 3:
			s_debugDraw->DrawLine(s_simplex.verts[0], s_simplex.verts[1], 0xFF00FFFF);
			s_debugDraw->DrawLine(s_simplex.verts[1], s_simplex.verts[2], 0xFF00FFFF);
			s_debugDraw->DrawLine(s_simplex.verts[2], s_simplex.verts[0], 0xFF00FFFF);
			break;
		case 4:
			s_debugDraw->DrawLine(s_simplex.verts[0], s_simplex.verts[1], 0xFF00FFFF);
			s_debugDraw->DrawLine(s_simplex.verts[1], s_simplex.verts[2], 0xFF00FFFF);
			s_debugDraw->DrawLine(s_simplex.verts[2], s_simplex.verts[0], 0xFF00FFFF);
			s_debugDraw->DrawLine(s_simplex.verts[0], s_simplex.verts[3], 0xFF00FFFF);
			s_debugDraw->DrawLine(s_simplex.verts[1], s_simplex.verts[3], 0xFF00FFFF);
			s_debugDraw->DrawLine(s_simplex.verts[2], s_simplex.verts[3], 0xFF00FFFF);
			break;
		}
	}
}

static bool CollideSpheres(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	const SphereColliderGeometry3D &sphereA = static_cast<const SphereColliderGeometry3D &>(geomA);
	const SphereColliderGeometry3D &sphereB = static_cast<const SphereColliderGeometry3D &>(geomB);
	const RigidBody &bodyA = sphereA.mParent->Parent();
	const RigidBody &bodyB = sphereB.mParent->Parent();
	const Math::Vector3 centerA = bodyA.LocalToGlobal(sphereA.center);
	const Math::Vector3 centerB = bodyB.LocalToGlobal(sphereB.center);
	Math::Vector3 d = centerB - centerA;
	const float dLenSq = d.LengthSq();
	const float r = sphereA.radius + sphereB.radius;

	if (dLenSq > r * r)
		return false;

	// build contact point
	d.Normalize();
	Contact &contact = *(manifold.contacts[0] = new (contactAllocator.Allocate()) Contact());
	contact.normal = d;
	contact.globalPositionA = centerA + contact.normal * sphereA.radius;
	contact.globalPositionB = centerB - contact.normal * sphereB.radius;
	contact.depth = r - std::sqrtf(dLenSq);
	manifold.numContacts = 1;
	manifold.invalidateExistingContacts = true;

	return true;
}

static bool CollideSphereQuad(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	// A: sphere, B: quad
	const SphereColliderGeometry3D &sphere = static_cast<const SphereColliderGeometry3D &>(geomA);
	const QuadColliderGeometry3D &quad = static_cast<const QuadColliderGeometry3D &>(geomB);
	const RigidBody &sphereBody = sphere.mParent->Parent();
	const RigidBody &quadBody = quad.mParent->Parent();

	// bring sphere center to quad's local space
	const Math::Vector3 globalC = sphereBody.LocalToGlobal(sphere.center);
	const Math::Vector3 localC = quad.inverseOrientation * (quadBody.GlobalToLocal(globalC) - quad.center);

	// find closest point on quad to sphere center
	const float closestX = Clamp(localC.x, -quad.halfWidth, quad.halfWidth);
	const float closestZ = Clamp(localC.z, -quad.halfHeight, quad.halfHeight);
	const float dx = closestX - localC.x;
	const float dz = closestZ - localC.z;
	const float drr = sphere.radius * sphere.radius - (dx * dx + dz * dz + localC.y * localC.y);
	if (drr >= 0.0f)
	{
		const Math::Vector3 localClosestOnQuad(closestX, 0.0f, closestZ);
		const Math::Vector3 globalClosestOnQuad = quadBody.LocalToGlobal(quad.orientation * localClosestOnQuad + quad.center);
		//Math::Vector3 globalClosestOnSphereOffset = globalClosestOnQuad - globalC;
		//if (globalClosestOnSphereOffset.LengthSQ() > EPSILON)
		//  globalClosestOnSphereOffset.SetLength(sphere.radius); 

		// build contact point
		Contact &contact = *(manifold.contacts[0] = new (contactAllocator.Allocate()) Contact());
		contact.normal = quadBody.LocalToGlobalVec(quad.orientation * Math::Vector3(dx, -localC.y, dz).Normalized()); // points from sphere to quad
		contact.globalPositionA = globalClosestOnQuad;
		contact.globalPositionB = globalClosestOnQuad;
		contact.depth = std::sqrt(drr);
		manifold.numContacts = 1;
		manifold.invalidateExistingContacts = true;

		return true;
	}
	return false;
}

void MinkowskiDifferenceSupport(const ColliderGeometry &geomA, const ColliderGeometry &geomB, const RigidBody &bodyA, const RigidBody &bodyB, const Math::Vector3 &dir, Math::Vector3 &support, Math::Vector3 &supportA, Math::Vector3 &supportB)
{
	supportA = bodyA.LocalToGlobal(geomA.Support(bodyA.GlobalToLocalVec(dir)));
	supportB = bodyB.LocalToGlobal(geomB.Support(bodyB.GlobalToLocalVec(-dir)));
	support = supportA - supportB;
}

static bool CollideGeneric(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	static const Math::Vector3 k_origin(0.0f, 0.0f, 0.0f);
	static const float k_epsilon = 0.0001f;
	static const float k_epsilonSq = k_epsilon * k_epsilon;

	const RigidBody &bodyA = geomA.mParent->Parent();
	const RigidBody &bodyB = geomB.mParent->Parent();

	// for use in both GJK and EPA
	GjkSimplex simplex(geomA, geomB, bodyA, bodyB);

	// GJK
	//---------------------------------
	{

		// constants
		const unsigned k_gjkMaxIterations = s_gjkIterations; //32;

		// detect collision
		GjkSimplex::UpdateResult gjkUpdateResult;
		bool gjkSearchEnd = false;
		bool gjkCollisionDetected = false;
		unsigned gjkIterations = 0;
		do
		{
			++gjkIterations;

			// update simplex (add new point or update tetrahedron)
			gjkUpdateResult = simplex.Update();

			// compute end condition
			switch (gjkUpdateResult)
			{
			case GjkSimplex::NO_COLLISION:
				gjkSearchEnd = true;
				break;
			case GjkSimplex::COLLISION:
				gjkSearchEnd = true;
				gjkCollisionDetected = true;
				break;
			case GjkSimplex::CONTINUE:
				gjkSearchEnd = gjkIterations >= k_gjkMaxIterations;
				break;
			}

		} while (!gjkSearchEnd);

		// for debug draw
		s_simplex = simplex;

		if (!gjkCollisionDetected)
			return false;

		// we need a tetrahedron to start EPA
		simplex.BlowUpToTetrahedron();

	}
	//---------------------------------
	// end of GJK


	// EPA
	//---------------------------------
	{

		struct Vert
		{
			Math::Vector3 pW, pA, pB; // world, body A, body B
			Vert(const Math::Vector3 &pW, const Math::Vector3 &pA, const Math::Vector3 &pB)
				: pW(pW), pA(pA), pB(pB)
			{ }
		};

		struct Edge
		{
			unsigned v0, v1;
			Edge(unsigned v0, unsigned v1) : v0(v0), v1(v1)  { }
			bool operator==(const Edge &rhs) const
			{
				return v0 == rhs.v0 && v1 == rhs.v1;
			}
			bool operator<(const Edge &rhs) const { return v0 == rhs.v0 ? v1 < rhs.v1 : v0 < rhs.v0; }
		};

		struct Face
		{
			unsigned v0, v1, v2;
			float distance;
			Math::Vector3 closestPoint;
			bool valid;
			Face(void) : valid(true) { }
			Face(unsigned v0, unsigned v1, unsigned v2)
				: v0(v0), v1(v1), v2(v2)
				, valid(true)
			{ }
			Face(unsigned v0, unsigned v1, unsigned v2, float distance, const Math::Vector3 &closestPoint)
				: v0(v0), v1(v1), v2(v2)
				, distance(distance)
				, closestPoint(closestPoint)
				, valid(true)
			{ }
			bool operator==(const Face &rhs) const { return v0 == rhs.v0 && v1 == rhs.v1 && v2 == rhs.v2; }
			bool operator<(const Face &rhs) const { return distance > rhs.distance; }
		};

		// expanding polytope data
		typedef std::vector<Vert> VertList;
		typedef std::vector<Edge> EdgeList;
		typedef std::vector<Face> FaceList;
		static VertList verts;
		static EdgeList edges;
		static FaceList faces;
		verts.clear();
		edges.clear();
		faces.clear();

		// initially populate vert list & face list
		{
			const Math::Vector3 &v0 = simplex.verts[0];
			const Math::Vector3 &v1 = simplex.verts[1];
			const Math::Vector3 &v2 = simplex.verts[2];
			const Math::Vector3 &v3 = simplex.verts[3];
			verts.emplace_back(v0, simplex.vertsA[0], simplex.vertsB[0]);
			verts.emplace_back(v1, simplex.vertsA[1], simplex.vertsB[1]);
			verts.emplace_back(v2, simplex.vertsA[2], simplex.vertsB[2]);
			verts.emplace_back(v3, simplex.vertsA[3], simplex.vertsB[3]);
			if ((v3 - v0).Dot((v1 - v0).Cross(v2 - v0)) < 0.0f)
			{
				faces.emplace_back(0, 1, 2);
				faces.emplace_back(0, 3, 1);
				faces.emplace_back(0, 2, 3);
				faces.emplace_back(1, 3, 2);
			}
			else
			{
				faces.emplace_back(0, 2, 1);
				faces.emplace_back(0, 1, 3);
				faces.emplace_back(0, 3, 2);
				faces.emplace_back(1, 2, 3);
			}

			// enqueue 4 faces
			for (Face &face : faces)
			{
				const Math::Vector3 &v0 = verts[face.v0].pW;
				const Math::Vector3 &v1 = verts[face.v1].pW;
				const Math::Vector3 &v2 = verts[face.v2].pW;
				face.closestPoint = ClosestPointTriangle(k_origin, v0, v1, v2);
				face.distance = face.closestPoint.Length();
			}

			// build initial face heap
			std::make_heap(faces.begin(), faces.end());
		}

		// find closest point of CSO to origin (meat of EPA)
		static const unsigned k_epaMaxIterations = s_epaIterations; //32;
		unsigned iterations = 0;
		float lastFaceDistance = FLT_MAX;
		do
		{
			// degenerate case, break
			if (faces.empty())
				break;

			// get closest face
			Face closestFace = faces.front();
			while (!closestFace.valid)
			{
				std::pop_heap(faces.begin(), faces.end());
				faces.pop_back();
				closestFace = faces.front();
			}

			// find the direction to expand polytope
			const Math::Vector3 &v0 = verts[closestFace.v0].pW;
			const Math::Vector3 &v1 = verts[closestFace.v1].pW;
			const Math::Vector3 &v2 = verts[closestFace.v2].pW;
			const Math::Vector3 expandDir = (v1 - v0).Cross(v2 - v0).Normalized();


			// find expanding vert
			Math::Vector3 expandVert, expandVertA, expandVertB;
			MinkowskiDifferenceSupport(geomA, geomB, bodyA, bodyB, expandDir, expandVert, expandVertA, expandVertB);
			unsigned expandVertIndex = verts.size();
			verts.emplace_back(expandVert, expandVertA, expandVertB);

			// check end condition (expanding vert close to closest face, closest face found, or max iterations exhausted)
			++iterations;
			if ((expandVert - verts[closestFace.v0].pW).Dot(expandDir) < k_epsilon
				|| std::fabs(closestFace.distance - lastFaceDistance) < k_epsilon
				|| iterations >= k_epaMaxIterations)
			{
				// compute barycentric coordinates for closest point
				const Math::Vector3 &v0w = verts[closestFace.v0].pW;
				const Math::Vector3 &v0a = verts[closestFace.v0].pA;
				const Math::Vector3 &v0b = verts[closestFace.v0].pB;
				const Math::Vector3 &v1w = verts[closestFace.v1].pW;
				const Math::Vector3 &v1a = verts[closestFace.v1].pA;
				const Math::Vector3 &v1b = verts[closestFace.v1].pB;
				const Math::Vector3 &v2w = verts[closestFace.v2].pW;
				const Math::Vector3 &v2a = verts[closestFace.v2].pA;
				const Math::Vector3 &v2b = verts[closestFace.v2].pB;
				float tu, tv, tw;
				BarycentricCoordinates(closestFace.closestPoint, v0w, v1w, v2w, tu, tv, tw);
				//GV_ASSERT(tu + tv + tw >= -EPSILON && tu + tv + tw <= 1.0f + EPSILON);

				// generate contact information (use point from bodyA for now)
				Contact &contact = *(manifold.contacts[0] = new (contactAllocator.Allocate()) Contact());
				contact.globalPositionA = tu * v0a + tv * v1a + tw * v2a;
				contact.globalPositionB = tu * v0b + tv * v1b + tw * v2b;
				contact.depth = closestFace.distance;
				if (contact.depth < 0)
					contact.normal = (v1w - v0w).Cross(v2w - v0w).Normalized();
				else
					contact.normal = closestFace.closestPoint.Normalized();
				manifold.numContacts = 1;

				break; // end EPA, WOO~
			}
			lastFaceDistance = closestFace.distance; // record last face distance


			{
				// sanity check
				const Face &firstFace = faces.front();
				const Math::Vector3 &v0 = verts[firstFace.v0].pW;
				const Math::Vector3 &v1 = verts[firstFace.v1].pW;
				const Math::Vector3 &v2 = verts[firstFace.v2].pW;

				// expanding vert should be able to see the first face
				//GV_ASSERT((expandVert - v0).Dot((v1 - v0).Cross(v2 - v0)) >= -EPSILON);
			}

			// remove faces that can be "seen" by the expanding vert
			for (Face &face : faces)
			{
				if (!face.valid)
					continue;

				const Math::Vector3 &v0 = verts[face.v0].pW;
				const Math::Vector3 &v1 = verts[face.v1].pW;
				const Math::Vector3 &v2 = verts[face.v2].pW;

				// expanding vert can "see" the face, remove face, record edges
				if ((expandVert - v0).Dot((v1 - v0).Cross(v2 - v0)) >= 0.0f)
				{
					face.valid = false;

					const Edge e01(face.v0, face.v1);
					const Edge e10(face.v1, face.v0);
					const Edge e12(face.v1, face.v2);
					const Edge e21(face.v2, face.v1);
					const Edge e20(face.v2, face.v0);
					const Edge e02(face.v0, face.v2);

					// remove edge from edge list if the opposite edge already exists (i.e. not a boundary edge)
					// otherwise add edge
					bool oppositeEdgeFound = false;
					for (auto iter = edges.begin(); iter != edges.end(); ++iter)
					{
						if (*iter == e10)
						{
							*iter = edges.back();
							edges.pop_back();
							oppositeEdgeFound = true;
							break;
						}
					}
					if (!oppositeEdgeFound)
					{

						for (const Edge &edge : edges)
						{
							//GV_ASSERT(!(edge == e01));
						}

						edges.push_back(e01);
					}

					oppositeEdgeFound = false;
					for (auto iter = edges.begin(); iter != edges.end(); ++iter)
					{
						if (*iter == e21)
						{
							*iter = edges.back();
							edges.pop_back();
							oppositeEdgeFound = true;
							break;
						}
					}
					if (!oppositeEdgeFound)
					{

						for (const Edge &edge : edges)
						{
							//GV_ASSERT(!(edge == e12));
						}

						edges.push_back(e12);
					}

					oppositeEdgeFound = false;
					for (auto iter = edges.begin(); iter != edges.end(); ++iter)
					{
						if (*iter == e02)
						{
							*iter = edges.back();
							edges.pop_back();
							oppositeEdgeFound = true;
							break;
						}
					}
					if (!oppositeEdgeFound)
					{

						for (const Edge &edge : edges)
						{
							//GV_ASSERT(!(edge == e20));
						}

						edges.push_back(e20);
					}
				}
			}

			// "patch up" polytope to make it a manifold again
			for (const Edge &edge : edges)
			{
				const Math::Vector3 closestPoint = ClosestPointTriangle(k_origin, expandVert, verts[edge.v0].pW, verts[edge.v1].pW);

				Face newFace(expandVertIndex, edge.v0, edge.v1, closestPoint.Length(), closestPoint);
				for (const Face &face : faces)
				{
					//GV_ASSERT(!(newFace == face));
				}
				faces.emplace_back(newFace);

				std::push_heap(faces.begin(), faces.end());
			}
			edges.clear();

		} while (true); // end finding closest point of CSO to origin

	}
	//---------------------------------
	// end of EPA

	return true;
}

//-------------------------------------------------------
// end of GJK / EPA




static bool CollideQuadSphere(ContactManifold &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	const bool result = CollideSphereQuad(manifold, geomB, geomA, contactAllocator);
	manifold.FlipNormalsAndPosition();
	return result;
}

bool RayMesh(const Ray3 &ray, CRenderer &mesh, float maxDistance, float& PotentialDist)
{
	
	CTransform* transform = mesh.gameObject->transform;
	//Matrix4 T = transform->GetGlobalTranslationMatrix();
	//Matrix4 R = transform->GetGlobalRotationMatrix();
	//Matrix4 S = transform->GetGlobalScaleMatrix();

	//Matrix4 invWorld = T * (R * S);
  Matrix4 WorldM = transform->GetWorldMatrix();

	Ray3 localRay;
	localRay.pos = ray.pos;
	localRay.dir = ray.dir;

	const Math::Vector3 &p = localRay.pos;
	const Math::Vector3 &direc = maxDistance * localRay.dir;
	const Math::Vector3 q = p + direc;

	float tEnter = FLT_MAX;
	float tExit = -FLT_MAX;

	std::vector<Vector3>& verts = mesh.GetMesh()->GetUniqueVertices();
	std::vector<int>& faces = mesh.GetMesh()->GetUnqiueIndices();
	unsigned size = faces.size();
	unsigned counter = 0;
	Vector3 triangleVerts[3];
	while (counter < size-3)
	{
		for (int i = 0; i < 3; ++i, ++counter)
		{
			triangleVerts[i] = verts[faces[counter]];
		}
		--counter;
		// test - get rid of WorldM
		const Math::Vector3 &a = TransformPoint(WorldM , triangleVerts[0]);
		const Math::Vector3 &b = TransformPoint(WorldM , triangleVerts[1]);
		const Math::Vector3 &c = TransformPoint(WorldM , triangleVerts[2]);
		// end

		Math::Vector3 normal = (b - a).Cross(c - a);
		if (normal != Vector3::cZero)
			normal = normal.Normalized();

		float d = -normal.Dot(a); // positive: outside plane
		float time = -(p.Dot(normal) + d) / localRay.dir.Dot(normal);
		if (time < 0 || time > tEnter)
			continue;

		Vector3 interPoint = p + localRay.dir * time;
		float checkD = interPoint.Dot(normal);
		if (!PointInTriangle(interPoint, a, b, c))
			continue;

		tEnter = time;
	}
	if (tEnter < 0)
		return false;
	float length = (tEnter);
	if (length < PotentialDist)
	{
		PotentialDist = length;
		return true;
	}
	else
		return false;
}
bool RayMeshTrasformRayToWorldFaster(const Ray3 &ray, CRenderer &mesh, float maxDistance, float& PotentialDist)
{

	CTransform* transform = mesh.gameObject->transform;
	Matrix4 T = transform->GetGlobalTranslationMatrix();
	Matrix4 R = transform->GetGlobalRotationMatrix();
	Matrix4 S = transform->GetGlobalScaleMatrix();

	Matrix4 invWorld = T * (R * S);
	if (!Math::IsZero(invWorld.Determinant()))
		invWorld.Invert();
	Matrix4 invRS = R.Transposed() * S;
	if (!Math::IsZero(invRS.Determinant()))
		invRS.Transpose();

	Ray3 localRay;
	localRay.pos = TransformPoint(invWorld, ray.pos);
	localRay.dir = TransformPoint(invRS, ray.dir);
	localRay.dir = localRay.dir.Normalized();

	const Math::Vector3 &p = localRay.pos;
	const Math::Vector3 &direc = maxDistance * localRay.dir;
	const Math::Vector3 q = p + direc;

	float tEnter = FLT_MAX;
	float tExit = -FLT_MAX;

	std::vector<Vector3>& verts = mesh.GetMesh()->GetUniqueVertices();
	std::vector<int>& faces = mesh.GetMesh()->GetUnqiueIndices();
	unsigned size = faces.size();
	unsigned counter = 0;
	Vector3 triangleVerts[3];
	while (counter < size - 3)
	{
		for (int i = 0; i < 3; ++i, ++counter)
		{
			triangleVerts[i] = verts[faces[counter]];
		}
		const Math::Vector3 &a = triangleVerts[0];
		const Math::Vector3 &b = triangleVerts[1];
		const Math::Vector3 &c = triangleVerts[2];

		Math::Vector3 normal = (b - a).Cross(c - a);
		if (normal != Vector3::cZero)
			normal = normal.Normalized();

		float d = -normal.Dot(a); // positive: outside plane
		float time = -(p.Dot(normal) + d) / localRay.dir.Dot(normal);
		if (time < 0 || time > tEnter)
			continue;

		Vector3 interPoint = p + localRay.dir * time;
		float checkD = interPoint.Dot(normal);
		if (!PointInTriangle(interPoint, a, b, c))
			continue;


		tEnter = time;
	}

	Vector3 penPoint;
	penPoint = localRay.dir * tEnter;
	invRS = R * S;
	if (!Math::IsZero(invRS.Determinant()))
		invRS.Invert();
	if (!Math::IsZero(invRS.Determinant()))
		invRS.Transpose();
	float length = TransformPoint(invRS, penPoint).LengthSq();
	//float length = (tEnter);
	if (length < PotentialDist)
	{
		PotentialDist = length;
		return true;
	}
	else
		return false;
}
void CreateAdjancency(AdjacencyInfo& adjacency, std::vector<Vector3>& verts, std::vector<int>& indices)
{
	int i;
	
	i = 0;

	for (const Math::Vector3 &v : verts)
		adjacency.AddVert(v);

	unsigned faceVertIndices[3];
	int j = 0;
	for (auto index : indices)
	{
		faceVertIndices[j++] = index;
		if (j == 3)
		{
			adjacency.AddFace(faceVertIndices[0], faceVertIndices[1], faceVertIndices[2]);
			j = 0;
		}
	} 

	adjacency.isCreated = true;
}
/*
function SameSide(p1, p2, a, b)
cp1 = CrossProduct(b - a, p1 - a)
cp2 = CrossProduct(b - a, p2 - a)
if DotProduct(cp1, cp2) >= 0 then return true
else return false

function PointInTriangle(p, a, b, c)
if SameSide(p, a, b, c) and SameSide(p, b, a, c)
and SameSide(p, c, a, b) then return true
else return false
*/
bool PointInTriangle(const Vector3 &pt, const Vector3 &A, const Vector3 &B, const Vector3 &C)
{
	// Compute vectors        
	Vector3 v0 = C - A;
	Vector3	v1 = B - A;
	Vector3	v2 = pt - A;
	
	// Compute dot products
	float dot00 = v0.Dot(v0);
	float dot01 = v0.Dot(v1);
	float dot02 = v0.Dot(v2);
	float dot11 = v1.Dot(v1);
	float dot12 = v1.Dot(v2);
	
	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
	
	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v <= 1);

	//bool b1, b2, b3;
	//
	//b1 = SignForTriangle(pt, A, B) < 0.0f;
	//b2 = SignForTriangle(pt, B, C) < 0.0f;
	//b3 = SignForTriangle(pt, C, A) < 0.0f;
	//
	//return ((b1 == b2) && (b2 == b3));
	//
	//if (SameSide(pt, A, B, C) && SameSide(pt, B, A, C) && SameSide(pt, C, A, B))
	//	return true;
	//else
	//	return false;
}

float SignForTriangle(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool SameSide(const Vector3 &p1, const Vector3 &p2, const Vector3 &a, const Vector3 &b)
{
	Vector3 cp1 = (b - a).Cross(p1 - a);
	Vector3 cp2 = (b - a).Cross(p2 - a);
	if (cp1.Dot(cp2) >= 0)
		return true;
	else
		return false;
}

bool IsPointInAABB(const Vector3& point, const Vector3& boxCenter, const Vector3& boxDim)
{
	Vector3 min = boxCenter - boxDim;
	Vector3 max = boxCenter + boxDim;

	//Check if the point is less than max and greater than min
	if (point.x > min.x && point.x < max.x &&
		point.y > min.y && point.y < max.y &&
		point.z > min.z && point.z < max.z)
	{
		return true;
	}

	//If not, then return false
	return false;
}

RayCastResult& RayMesh_Output(const Ray3 &ray, CRenderer &mesh, float maxDistance, float& PotentialDist, RayCastResult& output)
{

	CTransform* transform = mesh.gameObject->transform;
	//Matrix4 T = transform->GetGlobalTranslationMatrix();
	//Matrix4 R = transform->GetGlobalRotationMatrix();
	//Matrix4 S = transform->GetGlobalScaleMatrix();

	//Matrix4 invWorld = T * (R * S);
	Matrix4 WorldM = transform->GetWorldMatrix();

	Ray3 localRay;
	localRay.pos = ray.pos;
	localRay.dir = ray.dir;

	const Math::Vector3 &p = localRay.pos;
	const Math::Vector3 &direc = maxDistance * localRay.dir;
	const Math::Vector3 q = p + direc;
	Vector3 interPoint, finalInterPoint;
	Math::Vector3 normal;

	float tEnter = FLT_MAX;
	float tExit = -FLT_MAX;

	std::vector<Vector3>& verts = mesh.GetMesh()->GetUniqueVertices();
	std::vector<int>& faces = mesh.GetMesh()->GetUnqiueIndices();
	unsigned size = faces.size();
	unsigned counter = 0;
	Vector3 triangleVerts[3];
	while (counter < size - 3)
	{
		for (int i = 0; i < 3; ++i, ++counter)
		{
			triangleVerts[i] = verts[faces[counter]];
		}
		--counter;
		// test - get rid of WorldM
		const Math::Vector3 &a = TransformPoint(WorldM, triangleVerts[0]);
		const Math::Vector3 &b = TransformPoint(WorldM, triangleVerts[1]);
		const Math::Vector3 &c = TransformPoint(WorldM, triangleVerts[2]);
		// end

		normal = (b - a).Cross(c - a);
		if (normal != Vector3::cZero)
			normal = normal.Normalized();

		float d = -normal.Dot(a); // positive: outside plane
		float time = -(p.Dot(normal) + d) / localRay.dir.Dot(normal);
		if (time < 0 || time > tEnter)
			continue;

		interPoint = p + localRay.dir * time;

		if (time > maxDistance)
			continue;

		float checkD = interPoint.Dot(normal);
		if (!PointInTriangle(interPoint, a, b, c))
			continue;
			
		tEnter = time;
		finalInterPoint = interPoint;
	}

	// early out
	if (tEnter < 0)
		return output;

	float length = (tEnter);
	if (length < PotentialDist)
	{
		PotentialDist = length;
		
		output.hit = true;
		output.t = tEnter;
		output.intersection = finalInterPoint;
		output.go = mesh.gameObject;
		output.normal = normal;
	}
	
	return output;
}

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   SupportFunction Class