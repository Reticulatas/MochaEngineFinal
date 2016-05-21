/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */

#include "stdafx.h"
#include "GJK.h"
#include "MathUtilities.h"
#include "SupportFunction.h"
#include "Simplex.h"
#include "../MochaMath/Math/Quat.h"
#include "ContactManifold.h"

#define PI_3     (PI / 3.0f)
static Math::Vector3 k_origin(0.0f, 0.0f, 0.0f);
static float k_epsilon = 0.0001f;
static float k_epsilonSq = k_epsilon * k_epsilon;

GjkSimplex::GjkSimplex(void)
{ }

GjkSimplex::GjkSimplex(const ColliderGeometry &geomA, const ColliderGeometry &geomB, const RigidBody &bodyA, const RigidBody &bodyB)
: numVerts(0)
, prevDistSq(FLT_MAX)
, currDistSq(FLT_MAX)
, distance(FLT_MAX)
, geomA(&geomA)
, geomB(&geomB)
, bodyA(&bodyA)
, bodyB(&bodyB)
{ }

void GjkSimplex::ReducePoints(void)
{
	switch (numVerts)
	{
	case 0:
	case 1:
	{
			  redcRecord.push_back(NONE);
			  break;
	}
	case 2:
	{
			  float t;
			  ClosestPointLine(k_origin, verts[0], verts[1], t);
			  if (t > 1.0f) // closest to verts[1]
			  {
				  verts[0] = verts[1];
				  vertsA[0] = vertsA[1];
				  vertsB[0] = vertsB[1];
				  numVerts = 1;
				  redcRecord.push_back(B2);

			  }
			  redcRecord.push_back(NONE);
			  break;
	}
	case 3:
	{
			  const Math::Vector3 &p = k_origin;
			  const Math::Vector3 &a = verts[0];
			  const Math::Vector3 &b = verts[1];
			  const Math::Vector3 &c = verts[2];

			  // Check if P in vertex region outside A
			  const Math::Vector3 ab = b - a;
			  const Math::Vector3 ac = c - a;
			  const Math::Vector3 ap = p - a;
			  float d1 = ab.Dot(ap);
			  float d2 = ac.Dot(ap);
			  if (d1 <= 0.0f && d2 <= 0.0f) // barycentric coordinates (1,0,0)
			  {
				  numVerts = 1;
				  redcRecord.push_back(IMP);
				  return;
			  }
			  // Check if P in vertex region outside B
			  const Math::Vector3 bp = p - b;
			  float d3 = ab.Dot(bp);
			  float d4 = ac.Dot(bp);
			  if (d3 >= 0.0f && d4 <= d3) // barycentric coordinates (0,1,0)
			  {
				  verts[0] = verts[1];
				  vertsA[0] = vertsA[1];
				  vertsB[0] = vertsB[1];
				  numVerts = 1;
				  redcRecord.push_back(IMP);
				  return;
			  }
			  // Check if P in edge region of AB, if so return projection of P onto AB
			  const float vc = d1 * d4 - d3 * d2;
			  if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) // barycentric coordinates (1-v,v,0)
			  {
				  numVerts = 2;
				  redcRecord.push_back(IMP);
				  return;
			  }
			  // Check if P in vertex region outside C
			  const Math::Vector3 cp = p - c;
			  const float d5 = ab.Dot(cp);
			  const float d6 = ac.Dot(cp);
			  if (d6 >= 0.0f && d5 <= d6) // barycentric coordinates (0,0,1)
			  {
				  verts[0] = verts[2];
				  vertsA[0] = vertsA[2];
				  vertsB[0] = vertsB[2];
				  numVerts = 1;
				  redcRecord.push_back(C3);
				  return;
			  }
			  // Check if P in edge region of AC, if so return projection of P onto AC
			  const float vb = d5 * d2 - d1 * d6;
			  if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) // barycentric coordinates (1-w,0,w)
			  {
				  verts[1] = verts[2];
				  vertsA[1] = vertsA[2];
				  vertsB[1] = vertsB[2];
				  numVerts = 2;
				  redcRecord.push_back(CA3);
				  return;
			  }
			  // Check if P in edge region of BC, if so return projection of P onto BC
			  const float va = d3 * d6 - d5 * d4;
			  if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) // barycentric coordinates (0,1-w,w)
			  {
				  verts[0] = verts[2];
				  vertsA[0] = vertsA[2];
				  vertsB[0] = vertsB[2];
				  numVerts = 2;
				  redcRecord.push_back(CB3);
				  return;
			  }
			  redcRecord.push_back(NONE);
			  break;
	}
	case 4:
	{
			  const Math::Vector3 &p = k_origin;
			  const Math::Vector3 &a = verts[0];
			  const Math::Vector3 &b = verts[1];
			  const Math::Vector3 &c = verts[2];
			  const Math::Vector3 &d = verts[3];
			  const Math::Vector3 dp = p - d;
			  const Math::Vector3 da = a - d;
			  const Math::Vector3 db = b - d;
			  const Math::Vector3 dc = c - d;
			  if (dp.Dot(da) <= -EPSILON
				  && dp.Dot(db) <= -EPSILON
				  && dp.Dot(dc) <= -EPSILON)
			  {
				  verts[0] = verts[3];
				  vertsA[0] = vertsA[3];
				  vertsB[0] = vertsB[3];
				  numVerts = 1;
				  redcRecord.push_back(D4);
				  return;
			  }

			  // test edge ad's Voronoi region
			  const Math::Vector3 dba = db.Cross(da).Normalized();
			  const Math::Vector3 dac = da.Cross(dc).Normalized();
			  const Math::Vector3 ad = -da;
			  const Math::Vector3 adXdba = ad.Cross(dba);
			  const Math::Vector3 dacXad = dac.Cross(ad);
			  if (dp.Dot(da) >= -EPSILON
				  && dp.Dot(ad) >= -EPSILON
				  && dp.Dot(adXdba) >= -EPSILON
				  && dp.Dot(dacXad) >= -EPSILON)
			  {
				  verts[1] = verts[3];
				  vertsA[1] = vertsA[3];
				  vertsB[1] = vertsB[3];
				  numVerts = 2;
				  redcRecord.push_back(DA4);
				  return;
			  }

			  // test edge bd's Voronoi region
			  const Math::Vector3 dcb = dc.Cross(db).Normalized();
			  const Math::Vector3 bd = -db;
			  const Math::Vector3 bdXdcb = bd.Cross(dcb);
			  const Math::Vector3 dbaXbd = dba.Cross(bd);
			  if (dp.Dot(db) >= -EPSILON
				  && dp.Dot(bd) >= -EPSILON
				  && dp.Dot(bdXdcb) >= -EPSILON
				  && dp.Dot(dbaXbd) >= -EPSILON)
			  {
				  verts[0] = verts[3];
				  vertsA[0] = vertsA[3];
				  vertsB[0] = vertsB[3];
				  numVerts = 2;
				  redcRecord.push_back(DB4);
				  return;
			  }

			  // test edge cd's Voronoi region
			  const Math::Vector3 cd = -dc;
			  const Math::Vector3 cdXdac = cd.Cross(dac);
			  const Math::Vector3 dcbXcd = dcb.Cross(cd);
			  if (dp.Dot(dc) >= -EPSILON
				  && dp.Dot(cd) >= -EPSILON
				  && dp.Dot(cdXdac) >= -EPSILON
				  && dp.Dot(dcbXcd) >= -EPSILON)
			  {
				  verts[0] = verts[2];
				  vertsA[0] = vertsA[2];
				  vertsB[0] = vertsB[2];
				  verts[1] = verts[3];
				  vertsA[1] = vertsA[3];
				  vertsB[1] = vertsB[3];
				  numVerts = 2;
				  redcRecord.push_back(DC4);
				  return;
			  }

			  // test face abd's Voronoi region
			  if (dp.Dot(dba) >= -EPSILON
				  && dp.Dot(dbaXbd) <= EPSILON
				  && dp.Dot(adXdba) <= EPSILON)
			  {
				  verts[2] = verts[3];
				  vertsA[2] = vertsA[3];
				  vertsB[2] = vertsB[3];
				  numVerts = 3;
				  redcRecord.push_back(DBA4);
				  return;
			  }

			  // test face bcd's Voronoi region
			  if (dp.Dot(dcb) >= -EPSILON
				  && dp.Dot(dcbXcd) <= EPSILON
				  && dp.Dot(bdXdcb) <= EPSILON)
			  {
				  verts[0] = verts[3];
				  vertsA[0] = vertsA[3];
				  vertsB[0] = vertsB[3];
				  numVerts = 3;
				  redcRecord.push_back(DCB4);
				  return;
			  }

			  // test face acd's Voronoi region
			  if (dp.Dot(dac) >= -EPSILON
				  && dp.Dot(dacXad) <= EPSILON
				  && dp.Dot(cdXdac) <= EPSILON)
			  {
				  verts[1] = verts[2];
				  vertsA[1] = vertsA[2];
				  vertsB[1] = vertsB[2];
				  verts[2] = verts[3];
				  vertsA[2] = vertsA[3];
				  vertsB[2] = vertsB[3];
				  numVerts = 3;
				  redcRecord.push_back(DAC4);
				  return;
			  }


			  return;

			  break;
	}
	default:
	{

			   break;
	}
	} // end switch (numVerts)
}

bool GjkSimplex::CheckOrigin(void) const
{
	switch (numVerts)
	{
	case 1:
	{
			  currDistSq = verts[0].LengthSq();
			  break;
	}
	case 2:
	{
			  currDistSq = ClosestPointLineSegment(k_origin, verts[0], verts[1]).LengthSq();
			  break;
	}
	case 3:
	{
			  currDistSq = ClosestPointTriangle(k_origin, verts[0], verts[1], verts[2]).LengthSq();
			  break;
	}
	case 4:
	{
			  currDistSq = ClosestPointTetrahedron(k_origin, verts[0], verts[1], verts[2], verts[3]).LengthSq();
			  break;
	}
	}
	return currDistSq <= k_epsilonSq;
}
// returns whether further search is necessary
GjkSimplex::UpdateResult GjkSimplex::Update(void)
{
	if (CheckOrigin())
		return COLLISION;

	if (currDistSq < prevDistSq || numVerts == 0)
		prevDistSq = currDistSq;
	else
		return NO_COLLISION;

	// reduction guarantees that simplex only contains minimum feature closest to origin
	ReducePoints();

	numVertsRecord.push_back(numVerts);

	// add new vertex
	return AddSupportPoint();
}
GjkSimplex::UpdateResult GjkSimplex::AddSupportPoint(void)
{
	Math::Vector3 closestPoint;
	Math::Vector3 searchDir;
	switch (numVerts)
	{
	case 0:
	{
			  searchDir =
				  bodyB->LocalToGlobal(geomB->mParent->LocalCentroid()) - bodyA->LocalToGlobal(geomA->mParent->LocalCentroid());
			  break;
	}
	case 1:
	{
			  // find search direction
			  closestPoint = verts[0];
			  searchDir = -closestPoint;
			  break;
	}
	case 2:
	{
			  // find search direction
			  float t;
			  closestPoint = ClosestPointLine(k_origin, verts[0], verts[1], t);
			  searchDir = -closestPoint;
			  break;
	}
	case 3:
	{
			  // find search direction
			  closestPoint = ClosestPointTriangle(k_origin, verts[0], verts[1], verts[2]);
			  searchDir = -closestPoint;
			  break;
	}
	default:
	{
			   return NO_COLLISION;
			   break;
	}
	} // end switch (numVerts)

	// point too close to origin
	if (numVerts > 0 && closestPoint.LengthSq() < EPSILON * EPSILON)
		return NO_COLLISION;

	// find support point

	dirRecord.push_back(searchDir);

	if (searchDir != Vector3().cZero)
		searchDir.Normalize();
	Math::Vector3 support, supportA, supportB;
	MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, support, supportA, supportB);
	verts[numVerts] = support;
	vertsA[numVerts] = supportA;
	vertsB[numVerts] = supportB;
	for (unsigned char i = 0; i < numVerts; ++i)
	if (support == verts[i])
		return NO_COLLISION; // no collision if support point already found
	++numVerts;

	// no better result, return
	if (searchDir.Dot(support) <= k_epsilon)
		return NO_COLLISION;

	// fix tetrahedron winding
	if (numVerts == 4 && (verts[0] - verts[3]).Dot((verts[1] - verts[3]).Cross(verts[2] - verts[3])) < 0.0f)
	{
		std::swap(verts[0], verts[1]);
		std::swap(vertsA[0], vertsA[1]);
		std::swap(vertsB[0], vertsB[1]);
	}


	vertRecord.push_back(support);
	// new distance of simplex from origin should be smaller than old one
	float newDistance;
	switch (numVerts)
	{
	case 1:
		newDistance = verts[0].Length();
		break;
	case 2:
		newDistance = ClosestPointLineSegment(k_origin, verts[0], verts[1]).Length();
		break;
	case 3:
		newDistance = ClosestPointTriangle(k_origin, verts[0], verts[1], verts[2]).Length();
		break;
	case 4:
		newDistance = ClosestPointTetrahedron(k_origin, verts[0], verts[1], verts[2], verts[3]).Length();
		break;
	default:
		break;
	}
	distance = newDistance;
	return CONTINUE;
}



void GjkSimplex::BlowUpToTetrahedron(void)
{
	switch (numVerts)
	{
		// intentional case fall-through
	case 1:
	{
			  if (verts[0].LengthSq() < k_epsilonSq)
			  {
				  static const Math::Vector3 k_searchDirs[] =
				  {
					  Math::Vector3(1.0f, 0.0f, 0.0f),
					  Math::Vector3(-1.0f, 0.0f, 0.0f),
					  Math::Vector3(0.0f, 1.0f, 0.0f),
					  Math::Vector3(0.0f, -1.0f, 0.0f),
					  Math::Vector3(0.0f, 0.0f, 1.0f),
					  Math::Vector3(0.0f, 0.0f, -1.0f),
				  };

				  // iterate until a good search direction is used
				  for (const Math::Vector3 &searchDir : k_searchDirs)
				  {
					  MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, verts[1], vertsA[1], vertsB[1]);
					  if ((verts[1] - verts[0]).LengthSq() >= k_epsilonSq)
						  break;
				  }
			  }
			  else
			  {
				  const Math::Vector3 searchDir = -verts[0];
				  MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, verts[1], vertsA[1], vertsB[1]);
			  }
	}
	case 2:
	{
			  const Math::Vector3 closestPoint = ClosestPointLineSegment(k_origin, verts[0], verts[1]);
			  if (closestPoint.LengthSq() < k_epsilonSq)
			  {
				  static const Math::Vector3 k_axes[3] = { Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector3(0.0f, 0.0f, 1.0f) };
				  const Math::Vector3 lineVec = verts[1] - verts[0];

				  Math::Vector3 searchDir = lineVec.Cross(k_axes[LeastSignificantComponent(lineVec)]);
				  Math::Matrix3 rot;
				  Quat().SetFromRotation((float)PI_3, lineVec).ToMatrix(rot);

				  for (int i = 0; i < 6; ++i)
				  {
					  MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, verts[2], vertsA[2], vertsB[2]);
					  if (verts[2].LengthSq() >= k_epsilonSq)
						  break;
					  searchDir = rot * searchDir;
				  }
			  }
			  else
			  {
				  const Math::Vector3 searchDir = -closestPoint;
				  MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, verts[2], vertsA[2], vertsB[2]);
			  }
	}
	case 3:
	{
			  const Math::Vector3 closestPoint = ClosestPointTriangle(k_origin, verts[0], verts[1], verts[2]);
			  if (closestPoint.LengthSq() < k_epsilonSq)
			  {
				  Math::Vector3 searchDir = (verts[1] - verts[0]).Cross(verts[2] - verts[0]);
				  MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, verts[3], vertsA[3], vertsB[3]);
				  if (verts[3].LengthSq() < k_epsilonSq)
				  {
					  searchDir.Negate();
					  MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, verts[3], vertsA[3], vertsB[3]);
				  }
			  }
			  else
			  {
				  const Math::Vector3 searchDir = -closestPoint;
				  MinkowskiDifferenceSupport(*geomA, *geomB, *bodyA, *bodyB, searchDir, verts[3], vertsA[3], vertsB[3]);
			  }
	}
	}
	numVerts = 4;

	// fix tetrahedron winding
	if (numVerts == 4 && (verts[0] - verts[3]).Dot((verts[1] - verts[3]).Cross(verts[2] - verts[3])) < 0.0f)
	{
		std::swap(verts[0], verts[1]);
		std::swap(vertsA[0], vertsA[1]);
		std::swap(vertsB[0], vertsB[1]);
	}

}

bool GjkSimplex::TetrahedronContainsOrigin(void) const
{
	static unsigned char faces[4][3] =
	{
		{ 0, 1, 2 },
		{ 0, 3, 1 },
		{ 0, 2, 3 },
		{ 1, 3, 2 }
	};

	for (auto &face : faces)
	{
		const Math::Vector3 &v0 = verts[face[0]];
		const Math::Vector3 &v1 = verts[face[1]];
		const Math::Vector3 &v2 = verts[face[2]];
		const float d = (v1 - v0).Cross(v2 - v0).Normalized().Dot(-v0);
		if (d >= 2.0f * k_epsilon)
			return false;
	}

	return true;
}



/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   GJK algorithem