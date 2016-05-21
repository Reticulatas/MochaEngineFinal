#include "stdafx.h"
#include "SphereShape.h"
#include "BoxShape.h"
#include "QuadraShape.h"
#include "MeshShape.h"
#include "CollisionFuncs.h"
#include "GJK.h"
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



bool RayAABB(const Ray3 &ray, const AABB &aabb, float maxDistance, float &t, bool considerInside)
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

static bool Unimplemented(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideGeneric(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideSpheres(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideSphereQuad(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);
static bool CollideQuadSphere(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator);

typedef bool(*CollisionCallback) (ContactManifold3D &, const ColliderGeometry &, const ColliderGeometry &, FixedSizeAllocator &);

static CollisionCallback s_collisionCallbacks[5][5] =
{
	//sphere            // quad            //polyhedron    // box          // mesh
	{ CollideSpheres, CollideSphereQuad, CollideGeneric, CollideGeneric, CollideGeneric }, // sphere
	{ CollideQuadSphere, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // quad
	{ CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // polyhedron
	{ CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // box
	{ CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric, CollideGeneric }, // mesh
};

bool Collide(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	bool result = false;

	const RigidBody3D &bodyA = manifold.colliderA->Parent();
	const RigidBody3D &bodyB = manifold.colliderB->Parent();

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
			Contact3D &contact = *manifold.contacts[i];

			// compute local points
			contact.localPositionA = bodyA.GlobalToLocal(contact.globalPositionA);
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

static bool Unimplemented(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	//GV_ASSERT_MESSAGE(false, "Collision detection for this type pair is not implemented.");
	return false;
}


// GJK / EPA
//-------------------------------------------------------

static PhysicsDebugDraw3D *s_debugDraw = nullptr;
static int s_gjkIterations = 32;
static int s_epaIterations = 32;
static GjkSimplex s_simplex;
void SetCollisionDebugDraw(PhysicsDebugDraw3D &debugDraw)
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

void MinkowskiDifferenceSupport(const ColliderGeometry &geomA, const ColliderGeometry &geomB, const RigidBody3D &bodyA, const RigidBody3D &bodyB, const Math::Vector3 &dir, Math::Vector3 &support, Math::Vector3 &supportA, Math::Vector3 &supportB)
{
	supportA = bodyA.LocalToGlobal(geomA.Support(bodyA.GlobalToLocalVec(dir)));
	supportB = bodyB.LocalToGlobal(geomB.Support(bodyB.GlobalToLocalVec(-dir)));
	support = supportA - supportB;
}

static bool CollideGeneric(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	static const Math::Vector3 k_origin(0.0f, 0.0f, 0.0f);
	static const float k_epsilon = 0.0001f;
	static const float k_epsilonSq = k_epsilon * k_epsilon;

	const RigidBody3D &bodyA = geomA.mParent->Parent();
	const RigidBody3D &bodyB = geomB.mParent->Parent();

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
				Contact3D &contact = *(manifold.contacts[0] = new (contactAllocator.Allocate()) Contact3D());
				contact.globalPositionA = tu * v0a + tv * v1a + tw * v2a;
				contact.globalPositionB = tu * v0b + tv * v1b + tw * v2b;
				contact.depth = closestFace.distance;
				if (contact.depth < k_epsilon)
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


static bool CollideSpheres(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	const SphereColliderGeometry3D &sphereA = static_cast<const SphereColliderGeometry3D &>(geomA);
	const SphereColliderGeometry3D &sphereB = static_cast<const SphereColliderGeometry3D &>(geomB);
	const RigidBody3D &bodyA = sphereA.mParent->Parent();
	const RigidBody3D &bodyB = sphereB.mParent->Parent();
	const Math::Vector3 centerA = bodyA.LocalToGlobal(sphereA.center);
	const Math::Vector3 centerB = bodyB.LocalToGlobal(sphereB.center);
	Math::Vector3 d = centerB - centerA;
	const float dLenSq = d.LengthSq();
	const float r = sphereA.radius + sphereB.radius;

	if (dLenSq > r * r)
		return false;

	// build contact point
	d.Normalize();
	Contact3D &contact = *(manifold.contacts[0] = new (contactAllocator.Allocate()) Contact3D());
	contact.normal = d;
	contact.globalPositionA = centerA + contact.normal * sphereA.radius;
	contact.globalPositionB = centerB - contact.normal * sphereB.radius;
	contact.depth = r - std::sqrtf(dLenSq);
	manifold.numContacts = 1;
	manifold.invalidateExistingContacts = true;

	return true;
}

static bool CollideSphereQuad(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	// A: sphere, B: quad
	const SphereColliderGeometry3D &sphere = static_cast<const SphereColliderGeometry3D &>(geomA);
	const QuadColliderGeometry3D &quad = static_cast<const QuadColliderGeometry3D &>(geomB);
	const RigidBody3D &sphereBody = sphere.mParent->Parent();
	const RigidBody3D &quadBody = quad.mParent->Parent();

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
		Contact3D &contact = *(manifold.contacts[0] = new (contactAllocator.Allocate()) Contact3D());
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

static bool CollideQuadSphere(ContactManifold3D &manifold, const ColliderGeometry &geomA, const ColliderGeometry &geomB, FixedSizeAllocator &contactAllocator)
{
	const bool result = CollideSphereQuad(manifold, geomB, geomA, contactAllocator);
	manifold.FlipNormalsAndPosition();
	return result;
}


