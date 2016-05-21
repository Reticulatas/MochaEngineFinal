/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "MathIncludes.h"

struct Ray3;
struct ContactManifold3D;
class ColliderGeometry;
class RigidBody3D;
struct AABB;
class FixedSizeAllocator;

class PhysicsDebugDraw3D;

void SetCollisionDebugDraw(PhysicsDebugDraw3D &debugDraw);
void SetGjkEpaIterations(int gjkIterations, int epaIteratinos);
void DebugDrawGjk(void);

extern const unsigned char OUT_CODE_ZN;
extern const unsigned char OUT_CODE_ZP;
extern const unsigned char OUT_CODE_YN;
extern const unsigned char OUT_CODE_YP;
extern const unsigned char OUT_CODE_XN;
extern const unsigned char OUT_CODE_XP;

unsigned char OutCode(const Math::Vector3 &p, const Math::Vector3 &halfExtents);


bool RayAABB(const Ray3 &ray, const AABB &aabb, float maxDistance, float &t, bool considerInside = false); // t = 1.0f corresponds to maxDistance

bool RayTriangle(const Ray3 &ray, float maxDistance, const Math::Vector3 &a, const Math::Vector3 &b, const Math::Vector3 &c, float &t);

bool Collide(ContactManifold3D &manifold, const ColliderGeometry &geom1, const ColliderGeometry &geom2, FixedSizeAllocator &contactAllocator);

void MinkowskiDifferenceSupport(const ColliderGeometry &geomA, const ColliderGeometry &geomB, const RigidBody3D &bodyA, const RigidBody3D &bodyB, const Math::Vector3 &dir, Math::Vector3 &support, Math::Vector3 &supportA, Math::Vector3 &supportB);

