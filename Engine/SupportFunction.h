/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"
#include "RayCastResult.h"

class CPhysics;
class ContactsManager;
class GameObject;
class Simplex;
struct Ray3;
struct AABB;
class CSphereColliderComponent;
class CMeshColliderComponent;
class Mesh;
class AdjacencyInfo;
class CRenderer;

Point3 supportUsed(CollisionShape* a, CollisionShape* b, Math::Vector3& dir);

bool containOrigin2DUsed(Simplex& simplex, Math::Vector3& dir);

Math::Vector3& tripleProductUsed(Math::Vector3& vec1, Math::Vector3& vec2, Math::Vector3& vec);

bool PointOutsideOfPlane(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c);

const Math::Vector3 ClosestPointLineSegment(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b);

const Math::Vector3 ClosestPointLineSegment(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, float &t);

const Math::Vector3 ClosestPointTriangle(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c);

const Math::Vector3 ClosestPointTetrahedron(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c, const Math::Vector3 &d);

const Math::Vector3 ClosestPointLine(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b);

const Math::Vector3 ClosestPointLine(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, float &t);

void AABBFromModelSapceToWorldSpace(AABB& aabb, CTransform& transform);

void CorrectAABBMinAndMax(AABB& aabb);

void SetCollisionDebugDraw(PhysicsDebugDraw &debugDraw);

void SetGjkEpaIterations(int gjkIterations, int epaIteratinos);

void DebugDrawGjk(void);

extern const unsigned char OUT_CODE_ZN;
extern const unsigned char OUT_CODE_ZP;
extern const unsigned char OUT_CODE_YN;
extern const unsigned char OUT_CODE_YP;
extern const unsigned char OUT_CODE_XN;
extern const unsigned char OUT_CODE_XP;

unsigned char OutCode(const Math::Vector3 &p, const Math::Vector3 &halfExtents);


bool RayAABBWithTime(const Ray3 &ray, const AABB &aabb, float maxDistance, float &t, bool considerInside); // t = 1.0f corresponds to maxDistance

bool RayAABB(const Ray3 &ray, const AABB &aabb, float maxDistance, float& potentialZ);

bool RayMesh(const Ray3 &ray, CRenderer &mesh, float maxDistance, float& potentialZ);

RayCastResult& RayMesh_Output(const Ray3 &ray, CRenderer &mesh, float maxDistance, float& potentialZ, RayCastResult& output);

void CreateAdjancency(AdjacencyInfo& adjacency, std::vector<Vector3>& verts, std::vector<int>& indices);

bool RayTriangle(const Ray3 &ray, float maxDistance, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c, float &t);

int LeastSignificantComponent(const Math::Vector3 &v);

void BarycentricCoordinates(const Math::Vector3 &p, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c, float &u, float &v, float &w);

RigidBodyComponent *CreateRigidBody(unsigned char bodyType);

CBoxColliderComponent *CreateBoxCollider(Math::Vector3 halfExtend);

CSphereColliderComponent *CreateSphereCollider(const float radius);

CMeshColliderComponent *CreateMeshCollider(const std::string& filename);

bool Collide(ContactManifold &manifold, const ColliderGeometry &geom1, const ColliderGeometry &geom2, FixedSizeAllocator &contactAllocator);

void MinkowskiDifferenceSupport(const ColliderGeometry &geomA, const ColliderGeometry &geomB, const RigidBody &bodyA, const RigidBody &bodyB, const Math::Vector3 &dir, Math::Vector3 &support, Math::Vector3 &supportA, Math::Vector3 &supportB);

bool PointInTriangle(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c);

float SignForTriangle(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3);

bool SameSide(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Vector3 &p4);

bool IsPointInAABB(const Vector3& point, const Vector3& boxCenter, const Vector3& boxDim);

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   SupportFunction Class