/* Copyright (c) <2003-2011> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "dgMeshEffect.h"
#include "dgConvexHull3d.h"
#include "dgStack.h"
#include <string.h>

#pragma warning(disable:4100)

class dgFlatClipEdgeAttr
{
	public:
	hacd::HaI32 m_rightIndex;
	hacd::HaI32 m_leftIndex;
	hacd::HaI32 m_leftEdgeAttr;
	hacd::HaI32 m_leftTwinAttr;
	hacd::HaI32 m_rightEdgeAttr;
	hacd::HaI32 m_rightTwinAttr;
	dgEdge* m_edge;
	dgEdge* m_twin;
};

dgMeshEffect::dgMeshEffect(bool preAllocaBuffers)
{
	Init(preAllocaBuffers);
}

dgMeshEffect::dgMeshEffect (const dgMatrix& planeMatrix, hacd::HaF32 witdth, hacd::HaF32 breadth, hacd::HaI32 material, const dgMatrix& textureMatrix0, const dgMatrix& textureMatrix1)
{
	hacd::HaI32 index[4];
	hacd::HaI64 attrIndex[4];
	dgBigVector face[4];

	Init(true);

	face[0] = dgBigVector (hacd::HaF32 (0.0f), -witdth, -breadth, hacd::HaF32 (0.0f));
	face[1] = dgBigVector (hacd::HaF32 (0.0f),  witdth, -breadth, hacd::HaF32 (0.0f));
	face[2] = dgBigVector (hacd::HaF32 (0.0f),  witdth,  breadth, hacd::HaF32 (0.0f));
	face[3] = dgBigVector (hacd::HaF32 (0.0f), -witdth,  breadth, hacd::HaF32 (0.0f));

	for (hacd::HaI32 i = 0; i < 4; i ++) {
		dgBigVector uv0 (textureMatrix0.TransformVector(face[i]));
		dgBigVector uv1 (textureMatrix1.TransformVector(face[i]));

		m_points[i] = planeMatrix.TransformVector(face[i]);

		m_attib[i].m_vertex.m_x = m_points[i].m_x;
		m_attib[i].m_vertex.m_y = m_points[i].m_y;
		m_attib[i].m_vertex.m_z = m_points[i].m_z;
		m_attib[i].m_vertex.m_w = hacd::HaF64 (0.0f);

		m_attib[i].m_normal_x = planeMatrix.m_front.m_x;
		m_attib[i].m_normal_y = planeMatrix.m_front.m_y;
		m_attib[i].m_normal_z = planeMatrix.m_front.m_z;
		
		m_attib[i].m_u0 = uv0.m_y;
		m_attib[i].m_v0 = uv0.m_z;

		m_attib[i].m_u1 = uv1.m_y;
		m_attib[i].m_v1 = uv1.m_z;

		m_attib[i].m_material = material;

		index[i] = i;
		attrIndex[i] = i;
	}

	m_pointCount = 4;
	m_atribCount = 4;
	BeginFace();
	AddFace (4, index, attrIndex);
	EndFace();
}


dgMeshEffect::dgMeshEffect(dgPolyhedra& mesh, const dgMeshEffect& source)
	:dgPolyhedra (mesh) 
{
	m_pointCount = source.m_pointCount;
	m_maxPointCount = source.m_maxPointCount;
	m_points = (dgBigVector*) HACD_ALLOC(hacd::HaI32 (m_maxPointCount * sizeof(dgBigVector)));
	memcpy (m_points, source.m_points, m_pointCount * sizeof(dgBigVector));

	m_atribCount = source.m_atribCount;
	m_maxAtribCount = source.m_maxAtribCount;
	m_attib = (dgVertexAtribute*) HACD_ALLOC(hacd::HaI32 (m_maxAtribCount * sizeof(dgVertexAtribute)));
	memcpy (m_attib, source.m_attib, m_atribCount * sizeof(dgVertexAtribute));
}


dgMeshEffect::dgMeshEffect(const dgMeshEffect& source)
	:dgPolyhedra (source) 
{
	m_pointCount = source.m_pointCount;
	m_maxPointCount = source.m_maxPointCount;
	m_points = (dgBigVector*) HACD_ALLOC(hacd::HaI32 (m_maxPointCount * sizeof(dgBigVector)));
	memcpy (m_points, source.m_points, m_pointCount * sizeof(dgBigVector));

	m_atribCount = source.m_atribCount;
	m_maxAtribCount = source.m_maxAtribCount;
	m_attib = (dgVertexAtribute*) HACD_ALLOC(hacd::HaI32 (m_maxAtribCount * sizeof(dgVertexAtribute)));
	memcpy (m_attib, source.m_attib, m_atribCount * sizeof(dgVertexAtribute));
}


dgMeshEffect::dgMeshEffect(dgCollision* const collision)
	:dgPolyhedra () 
{
	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED
	class dgMeshEffectBuilder
	{
		public:
		dgMeshEffectBuilder ()
		{
			m_brush = 0;
			m_faceCount = 0;
			m_vertexCount = 0;
			m_maxFaceCount = 32;
			m_maxVertexCount = 32;
			m_vertex = (dgVector*) HACD_ALLOC(m_maxVertexCount * sizeof(dgVector));
			m_faceIndexCount = (hacd::HaI32*) HACD_ALLOC(m_maxFaceCount * sizeof(hacd::HaI32));
		}

		~dgMeshEffectBuilder ()
		{
			HACD_FREE (m_faceIndexCount);
			HACD_FREE (m_vertex);
		}

		static void GetShapeFromCollision (void* userData, hacd::HaI32 vertexCount, const hacd::HaF32* faceVertex, hacd::HaI32 id)
		{
			hacd::HaI32 vertexIndex; 
			dgMeshEffectBuilder& builder = *((dgMeshEffectBuilder*)userData);


			if (builder.m_faceCount >= builder.m_maxFaceCount) {
				hacd::HaI32* index;

				builder.m_maxFaceCount *= 2;
				index = (hacd::HaI32*) HACD_ALLOC(builder.m_maxFaceCount * sizeof(hacd::HaI32));
				memcpy (index, builder.m_faceIndexCount, builder.m_faceCount * sizeof(hacd::HaI32));
				HACD_FREE(builder.m_faceIndexCount);
				builder.m_faceIndexCount = index;
			}
			builder.m_faceIndexCount[builder.m_faceCount] = vertexCount;
			builder.m_faceCount = builder.m_faceCount + 1;

			vertexIndex = builder.m_vertexCount; 
			hacd::HaF32 brush = hacd::HaF32 (builder.m_brush);
			for (hacd::HaI32 i = 0; i < vertexCount; i ++) {
				if (vertexIndex >= builder.m_maxVertexCount) {
					dgVector* points;

					builder.m_maxVertexCount *= 2;
					points = (dgVector*) HACD_ALLOC(builder.m_maxVertexCount * sizeof(dgVector));
					memcpy (points, builder.m_vertex, vertexIndex * sizeof(dgVector));
					HACD_FREE(builder.m_vertex);
					builder.m_vertex = points;
				}

				builder.m_vertex[vertexIndex].m_x = faceVertex[i * 3 + 0];
				builder.m_vertex[vertexIndex].m_y = faceVertex[i * 3 + 1];
				builder.m_vertex[vertexIndex].m_z = faceVertex[i * 3 + 2];
				builder.m_vertex[vertexIndex].m_w = brush;
				vertexIndex ++;
			}

			builder.m_vertexCount = vertexIndex;
		}

		hacd::HaI32 m_brush;
		hacd::HaI32 m_vertexCount;
		hacd::HaI32 m_maxVertexCount;

		hacd::HaI32 m_faceCount;
		hacd::HaI32 m_maxFaceCount;

		dgVector* m_vertex;
		hacd::HaI32* m_faceIndexCount;
	};

	dgMeshEffectBuilder builder;

	if (collision->IsType (dgCollision::dgCollisionCompound_RTTI)) {
		dgCollisionInfo collisionInfo;
		collision->GetCollisionInfo (&collisionInfo);

		dgMatrix matrix (collisionInfo.m_offsetMatrix);
		//dgCollisionInfo::dgCoumpountCollisionData& data = collisionInfo.m_compoundCollision;
		//for (hacd::HaI32 i = 0; i < data.m_chidrenCount; i ++) {
		dgCollisionCompound* const compoundCollision = (dgCollisionCompound*) collision;
		//for (void* node = collisionInfo.m_compoundCollision)
		hacd::HaI32 brush = 0;
		for (dgTree<dgCollisionCompound::dgNodeBase*, dgCollisionConvex*>::dgTreeNode* node = compoundCollision->GetFirstNode(); node; node = compoundCollision->GetNextNode(node)) {
			builder.m_brush = brush;
			brush ++;
			//dgCollision* const childShape = data.m_chidren[i];
			dgCollision* const childShape = compoundCollision->GetCollisionFromNode(node);
			childShape->DebugCollision (matrix, (OnDebugCollisionMeshCallback) dgMeshEffectBuilder::GetShapeFromCollision, &builder);
		}

	} else {
		dgMatrix matrix (dgGetIdentityMatrix());
		collision->DebugCollision (matrix, (OnDebugCollisionMeshCallback) dgMeshEffectBuilder::GetShapeFromCollision, &builder);
	}

	dgStack<hacd::HaI32>indexList (builder.m_vertexCount);

	dgVertexListToIndexList (&builder.m_vertex[0].m_x, sizeof (dgVector), sizeof (dgVector), 0, builder.m_vertexCount, &indexList[0], DG_VERTEXLIST_INDEXLIST_TOL);	

	dgStack<hacd::HaI32> materialIndex(builder.m_faceCount);
	dgStack<hacd::HaI32> m_normalUVIndex(builder.m_vertexCount);

	dgVector normalUV(hacd::HaF32 (0.0f), hacd::HaF32 (0.0f), hacd::HaF32 (0.0f), hacd::HaF32 (0.0f));

	memset (&materialIndex[0], 0, size_t (materialIndex.GetSizeInBytes()));
	memset (&m_normalUVIndex[0], 0, size_t (m_normalUVIndex.GetSizeInBytes()));

	Init(true);
	BuildFromVertexListIndexList(builder.m_faceCount, builder.m_faceIndexCount, &materialIndex[0],
								 &builder.m_vertex[0].m_x, sizeof (dgVector), &indexList[0],
								 &normalUV.m_x, sizeof (dgVector), &m_normalUVIndex[0],
								 &normalUV.m_x, sizeof (dgVector), &m_normalUVIndex[0],
								 &normalUV.m_x, sizeof (dgVector), &m_normalUVIndex[0]);

        RepairTJoints(true);
	CalculateNormals(hacd::HaF32 (45.0f * 3.1416f/180.0f));
#endif
}

dgMeshEffect::~dgMeshEffect(void)
{
	HACD_FREE (m_points);
	HACD_FREE (m_attib);
}

void dgMeshEffect::Init(bool preAllocaBuffers)
{
	m_pointCount = 0;
	m_atribCount = 0;
	m_maxPointCount = DG_MESH_EFFECT_INITIAL_VERTEX_SIZE;
	m_maxAtribCount = DG_MESH_EFFECT_INITIAL_VERTEX_SIZE;

	m_points = NULL;
	m_attib = NULL;
	if (preAllocaBuffers) {
		m_points = (dgBigVector*) HACD_ALLOC(hacd::HaI32 (m_maxPointCount * sizeof(dgBigVector)));
		m_attib = (dgVertexAtribute*) HACD_ALLOC(hacd::HaI32 (m_maxAtribCount * sizeof(dgVertexAtribute)));
	}
}


void dgMeshEffect::Triangulate  ()
{
	dgPolyhedra polygon;

	hacd::HaI32 mark = IncLRU();
	polygon.BeginFace();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);

		if ((face->m_mark != mark) && (face->m_incidentFace > 0)) {
			hacd::HaI32	index[DG_MESH_EFFECT_POINT_SPLITED];

			dgEdge* ptr = face;
			hacd::HaI32 indexCount = 0;
			do {
				hacd::HaI32 attribIndex = hacd::HaI32 (ptr->m_userData);
				m_attib[attribIndex].m_vertex.m_w = hacd::HaF64 (ptr->m_incidentVertex);
				ptr->m_mark = mark;
				index[indexCount] = attribIndex;
				indexCount ++;
				ptr = ptr->m_next;
			} while (ptr != face);
			polygon.AddFace(indexCount, index);
		}
	}
	polygon.EndFace();


	dgPolyhedra leftOversOut;
	polygon.Triangulate(&m_attib[0].m_vertex.m_x, sizeof (dgVertexAtribute), &leftOversOut);
	HACD_ASSERT (leftOversOut.GetCount() == 0);


	RemoveAll();
	SetLRU (0);

	mark = polygon.IncLRU();
	BeginFace();
	dgPolyhedra::Iterator iter1 (polygon);
	for (iter1.Begin(); iter1; iter1 ++){
		dgEdge* const face = &(*iter1);
		if ((face->m_mark != mark) && (face->m_incidentFace > 0)) {
			hacd::HaI32	index[DG_MESH_EFFECT_POINT_SPLITED];
			hacd::HaI64	userData[DG_MESH_EFFECT_POINT_SPLITED];

			dgEdge* ptr = face;
			hacd::HaI32 indexCount = 0;
			do {
				ptr->m_mark = mark;
				index[indexCount] = hacd::HaI32 (m_attib[ptr->m_incidentVertex].m_vertex.m_w);

				userData[indexCount] = ptr->m_incidentVertex;
				indexCount ++;
				ptr = ptr->m_next;
			} while (ptr != face);
			AddFace(indexCount, index, userData);
		}
	}
	EndFace();

	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if (face->m_incidentFace > 0) {
			hacd::HaI32 attribIndex = hacd::HaI32 (face->m_userData);
			m_attib[attribIndex].m_vertex.m_w = m_points[face->m_incidentVertex].m_w;
		}
	}

}

void dgMeshEffect::ConvertToPolygons ()
{
	dgPolyhedra polygon;

	hacd::HaI32 mark = IncLRU();
	polygon.BeginFace();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if ((face->m_mark != mark) && (face->m_incidentFace > 0)) {
			hacd::HaI32	index[DG_MESH_EFFECT_POINT_SPLITED];

			dgEdge* ptr = face;
			hacd::HaI32 indexCount = 0;
			do {
				hacd::HaI32 attribIndex = hacd::HaI32 (ptr->m_userData);

				m_attib[attribIndex].m_vertex.m_w = hacd::HaF32 (ptr->m_incidentVertex);
				ptr->m_mark = mark;
				index[indexCount] = attribIndex;
				indexCount ++;
				ptr = ptr->m_next;
			} while (ptr != face);
			polygon.AddFace(indexCount, index);
		}
	}
	polygon.EndFace();

	dgPolyhedra leftOversOut;
	polygon.ConvexPartition (&m_attib[0].m_vertex.m_x, sizeof (dgVertexAtribute), &leftOversOut);
	HACD_ASSERT (leftOversOut.GetCount() == 0);

	RemoveAll();
	SetLRU (0);

	mark = polygon.IncLRU();
	BeginFace();
	dgPolyhedra::Iterator iter1 (polygon);
	for (iter1.Begin(); iter1; iter1 ++){
		dgEdge* const face = &(*iter1);
		if ((face->m_mark != mark) && (face->m_incidentFace > 0)) {
			hacd::HaI32	index[DG_MESH_EFFECT_POINT_SPLITED];
			hacd::HaI64	userData[DG_MESH_EFFECT_POINT_SPLITED];

			dgEdge* ptr = face;
			hacd::HaI32 indexCount = 0;
			do {
				ptr->m_mark = mark;
				index[indexCount] = hacd::HaI32 (m_attib[ptr->m_incidentVertex].m_vertex.m_w);

				userData[indexCount] = ptr->m_incidentVertex;
				indexCount ++;
				ptr = ptr->m_next;
			} while (ptr != face);
			AddFace(indexCount, index, userData);
		}
	}
	EndFace();

	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if (face->m_incidentFace > 0) {
			hacd::HaI32 attribIndex = hacd::HaI32 (face->m_userData);
			m_attib[attribIndex].m_vertex.m_w = m_points[face->m_incidentVertex].m_w;
		}
	}

	RepairTJoints (false);
}

void dgMeshEffect::RemoveUnusedVertices(hacd::HaI32* const vertexMap)
{
	dgPolyhedra polygon;
	dgStack<hacd::HaI32>attrbMap(m_atribCount);

	memset(&vertexMap[0], -1, m_pointCount * sizeof (int));
	memset(&attrbMap[0], -1, m_atribCount * sizeof (int));

	int attribCount = 0;
	int vertexCount = 0;

	dgStack<dgBigVector>points (m_pointCount);
	dgStack<dgVertexAtribute>atributes (m_atribCount);

	hacd::HaI32 mark = IncLRU();
	polygon.BeginFace();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if ((face->m_mark != mark) && (face->m_incidentFace > 0)) {
			hacd::HaI32	vertex[DG_MESH_EFFECT_POINT_SPLITED];
			hacd::HaI64	userData[DG_MESH_EFFECT_POINT_SPLITED];
			int indexCount = 0;
			dgEdge* ptr = face;
			do {
				ptr->m_mark = mark;

				int index = ptr->m_incidentVertex;
				if (vertexMap[index] == -1) {
					vertexMap[index] = vertexCount;
					points[vertexCount] = m_points[index];
					vertexCount ++;
				}
				vertex[indexCount] = vertexMap[index];

				index = int (ptr->m_userData);
				if (attrbMap[index] == -1) {
					attrbMap[index] = attribCount;
					atributes[attribCount] = m_attib[index];
					attribCount ++;
				}
				userData[indexCount] = attrbMap[index];
				indexCount ++;

				ptr = ptr->m_next;
			} while (ptr != face);
			polygon.AddFace(indexCount, vertex, userData);
		}
	}
	polygon.EndFace();

	m_pointCount = vertexCount;
	memcpy (&m_points[0].m_x, &points[0].m_x, m_pointCount * sizeof (dgBigVector));
	 
	m_atribCount = attribCount;
	memcpy (&m_attib[0].m_vertex.m_x, &atributes[0].m_vertex.m_x, m_atribCount * sizeof (dgVertexAtribute));


	RemoveAll();
	SetLRU (0);

	BeginFace();
	dgPolyhedra::Iterator iter1 (polygon);
	for (iter1.Begin(); iter1; iter1 ++){
		dgEdge* const face = &(*iter1);
		if ((face->m_mark != mark) && (face->m_incidentFace > 0)) {
			hacd::HaI32	index[DG_MESH_EFFECT_POINT_SPLITED];
			hacd::HaI64	userData[DG_MESH_EFFECT_POINT_SPLITED];

			dgEdge* ptr = face;
			hacd::HaI32 indexCount = 0;
			do {
				ptr->m_mark = mark;
				index[indexCount] = ptr->m_incidentVertex;
				userData[indexCount] = hacd::HaI64 (ptr->m_userData);
				indexCount ++;
				ptr = ptr->m_next;
			} while (ptr != face);
			AddFace(indexCount, index, userData);
		}
	}
	EndFace();
	PackVertexArrays ();
}

dgMatrix dgMeshEffect::CalculateOOBB (dgBigVector& size) const
{
	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED
	dgSphere sphere (CalculateSphere (&m_points[0].m_x, sizeof (dgBigVector), NULL));
	size = sphere.m_size;

	dgMatrix permuation (dgGetIdentityMatrix());
	permuation[0][0] = hacd::HaF32 (0.0f);
	permuation[0][1] = hacd::HaF32 (1.0f);
	permuation[1][1] = hacd::HaF32 (0.0f);
	permuation[1][2] = hacd::HaF32 (1.0f);
	permuation[2][2] = hacd::HaF32 (0.0f);
	permuation[2][0] = hacd::HaF32 (1.0f);

	while ((size.m_x < size.m_y) || (size.m_x < size.m_z)) {
		sphere = permuation * sphere;
		size = permuation.UnrotateVector(size);
	}

	return sphere;
#endif
	dgMatrix _emtpy;
	return _emtpy;
}

void dgMeshEffect::CalculateAABB (dgBigVector& minBox, dgBigVector& maxBox) const
{
	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED
	dgBigVector minP ( hacd::HaF64 (1.0e15f),  hacd::HaF64 (1.0e15f),  hacd::HaF64 (1.0e15f), hacd::HaF64 (0.0f)); 
	dgBigVector maxP (-hacd::HaF64 (1.0e15f), -hacd::HaF64 (1.0e15f), -hacd::HaF64 (1.0e15f), hacd::HaF64 (0.0f)); 

	dgPolyhedra::Iterator iter (*this);
	const dgBigVector* const points = &m_points[0];
	for (iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		const dgBigVector& p (points[edge->m_incidentVertex]);

		minP.m_x = GetMin (p.m_x, minP.m_x); 
		minP.m_y = GetMin (p.m_y, minP.m_y); 
		minP.m_z = GetMin (p.m_z, minP.m_z); 

		maxP.m_x = GetMax (p.m_x, maxP.m_x); 
		maxP.m_y = GetMax (p.m_y, maxP.m_y); 
		maxP.m_z = GetMax (p.m_z, maxP.m_z); 
	}

	minBox = minP;
	maxBox = maxP;
#endif
}

void dgMeshEffect::EnumerateAttributeArray (dgVertexAtribute* const attib)
{
	hacd::HaI32 index = 0;
	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		HACD_ASSERT (index < GetCount());
		attib[index] = m_attib[hacd::HaI32 (edge->m_userData)];
		edge->m_userData = hacd::HaU64 (index);
		index ++;
	}
}

void dgMeshEffect::ApplyAttributeArray (dgVertexAtribute* const attib)
{
	dgStack<hacd::HaI32>indexMap (GetCount());

	m_atribCount = dgVertexListToIndexList (&attib[0].m_vertex.m_x, sizeof (dgVertexAtribute), sizeof (dgVertexAtribute) / sizeof(hacd::HaF64), GetCount(), &indexMap[0], DG_VERTEXLIST_INDEXLIST_TOL);
	m_maxAtribCount = m_atribCount;

	HACD_FREE (m_attib);
	m_attib = (dgVertexAtribute*) HACD_ALLOC(hacd::HaI32 (m_atribCount * sizeof(dgVertexAtribute)));
	memcpy (m_attib, attib, m_atribCount * sizeof(dgVertexAtribute));

	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		hacd::HaI32 index = indexMap[hacd::HaI32 (edge->m_userData)];
		HACD_ASSERT (index >=0);
		HACD_ASSERT (index < m_atribCount);
		edge->m_userData = hacd::HaU64 (index);
	}
}

dgBigVector dgMeshEffect::GetOrigin ()const
{
	dgBigVector origin (hacd::HaF64 (0.0f), hacd::HaF64 (0.0f), hacd::HaF64 (0.0f), hacd::HaF64 (0.0f));	
	for (hacd::HaI32 i = 0; i < m_pointCount; i ++) {
		origin += m_points[i];
	}	
	return origin.Scale (hacd::HaF64 (1.0f) / m_pointCount);
}


void dgMeshEffect::FixCylindricalMapping (dgVertexAtribute* attribArray) const
{
	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		dgVertexAtribute& attrib0 = attribArray[hacd::HaI32 (edge->m_userData)];
		dgVertexAtribute& attrib1 = attribArray[hacd::HaI32 (edge->m_next->m_userData)];

		hacd::HaF64 error = fabs (attrib0.m_u0 - attrib1.m_u0);
		if (error > hacd::HaF32 (0.6f)) {
			if (attrib0.m_u0 < attrib1.m_u0) {
				attrib0.m_u0 += hacd::HaF32 (1.0f);
				attrib0.m_u1 = attrib0.m_u0;
			} else {
				attrib1.m_u0 += hacd::HaF32 (1.0f);
				attrib1.m_u1 = attrib1.m_u0;
			}
			
		}
	}

	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		dgVertexAtribute& attrib0 = attribArray[hacd::HaI32 (edge->m_userData)];
		dgVertexAtribute& attrib1 = attribArray[hacd::HaI32 (edge->m_next->m_userData)];

		hacd::HaF64 error = fabs (attrib0.m_u0 - attrib1.m_u0);
		if (error > hacd::HaF32 (0.6f)) {
			if (attrib0.m_u0 < attrib1.m_u0) {
				attrib0.m_u0 += hacd::HaF32 (1.0f);
				attrib0.m_u1 = attrib0.m_u0;
			} else {
				attrib1.m_u0 += hacd::HaF32 (1.0f);
				attrib1.m_u1 = attrib1.m_u0;
			}
		}
	}
}


void dgMeshEffect::SphericalMapping (hacd::HaI32 material)
{
	dgBigVector origin (GetOrigin());

	dgStack<dgBigVector>sphere (m_pointCount);
	for (hacd::HaI32 i = 0; i < m_pointCount; i ++) {
		dgBigVector point (m_points[i] - origin);
		point = point.Scale (1.0f / dgSqrt (point % point));

		hacd::HaF64 u = dgAsin (point.m_y);
		hacd::HaF64 v = dgAtan2 (point.m_x, point.m_z);

		u = (hacd::HaF64 (3.1416f/2.0f) - u) / hacd::HaF64 (3.1416f);
		v = (hacd::HaF64 (3.1416f) - v) / hacd::HaF64 (2.0f * 3.1416f);
		sphere[i].m_x = v;
		sphere[i].m_y = u;
	}


	dgStack<dgVertexAtribute>attribArray (GetCount());
	EnumerateAttributeArray (&attribArray[0]);

	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		dgVertexAtribute& attrib = attribArray[hacd::HaI32 (edge->m_userData)];
		attrib.m_u0 = sphere[edge->m_incidentVertex].m_x;
		attrib.m_v0 = sphere[edge->m_incidentVertex].m_y;
		attrib.m_u1 = sphere[edge->m_incidentVertex].m_x;
		attrib.m_v1 = sphere[edge->m_incidentVertex].m_y;
		attrib.m_material = material;
	}

	FixCylindricalMapping (&attribArray[0]);
	ApplyAttributeArray (&attribArray[0]);
}

void dgMeshEffect::CylindricalMapping (hacd::HaI32 cylinderMaterial, hacd::HaI32 capMaterial)
{
/*
	dgVector origin (GetOrigin());

	dgStack<dgVector>cylinder (m_pointCount);

	hacd::HaF32 xMax;
	hacd::HaF32 xMin;

	xMin= hacd::HaF32 (1.0e10f);
	xMax= hacd::HaF32 (-1.0e10f);
	for (hacd::HaI32 i = 0; i < m_pointCount; i ++) {
		cylinder[i] = m_points[i] - origin;
		xMin = GetMin (xMin, cylinder[i].m_x);
		xMax = GetMax (xMax, cylinder[i].m_x);
	}

	hacd::HaF32 xscale = hacd::HaF32 (1.0f)/ (xMax - xMin);
	for (hacd::HaI32 i = 0; i < m_pointCount; i ++) {
		hacd::HaF32 u;
		hacd::HaF32 v;
		hacd::HaF32 y;
		hacd::HaF32 z;
		y = cylinder[i].m_y;
		z = cylinder[i].m_z;
		u = dgAtan2 (z, y);
		if (u < hacd::HaF32 (0.0f)) {
			u += hacd::HaF32 (3.141592f * 2.0f);
		}
		v = (cylinder[i].m_x - xMin) * xscale;

		cylinder[i].m_x = hacd::HaF32 (1.0f) - u * hacd::HaF32 (1.0f / (2.0f * 3.141592f));
		cylinder[i].m_y = v;
	}

	dgStack<dgVertexAtribute>attribArray (GetCount());
	EnumerateAttributeArray (&attribArray[0]);

	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge;
		edge = &(*iter);
		dgVertexAtribute& attrib = attribArray[hacd::HaI32 (edge->m_userData)];
		attrib.m_u0 = cylinder[edge->m_incidentVertex].m_x;
		attrib.m_v0 = cylinder[edge->m_incidentVertex].m_y;
		attrib.m_u1 = cylinder[edge->m_incidentVertex].m_x;
		attrib.m_v1 = cylinder[edge->m_incidentVertex].m_y;
		attrib.m_material = cylinderMaterial;
	}

	FixCylindricalMapping (&attribArray[0]);

	hacd::HaI32 mark;
	mark = IncLRU();
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge;
		edge = &(*iter);
		if (edge->m_mark < mark){
			const dgVector& p0 = m_points[edge->m_incidentVertex];
			const dgVector& p1 = m_points[edge->m_next->m_incidentVertex];
			const dgVector& p2 = m_points[edge->m_prev->m_incidentVertex];

			edge->m_mark = mark;
			edge->m_next->m_mark = mark;
			edge->m_prev->m_mark = mark;

			dgVector e0 (p1 - p0);
			dgVector e1 (p2 - p0);
			dgVector n (e0 * e1);
			if ((n.m_x * n.m_x) > (hacd::HaF32 (0.99f) * (n % n))) {
				dgEdge* ptr;

				ptr = edge;
				do {
					dgVertexAtribute& attrib = attribArray[hacd::HaI32 (ptr->m_userData)];
					dgVector p (m_points[ptr->m_incidentVertex] - origin);
					p.m_x = hacd::HaF32 (0.0f);
					p = p.Scale (hacd::HaF32 (dgRsqrt(p % p)));
					attrib.m_u0 = hacd::HaF32 (0.5f) + p.m_y * hacd::HaF32 (0.5f);
					attrib.m_v0 = hacd::HaF32 (0.5f) + p.m_z * hacd::HaF32 (0.5f);
					attrib.m_u1 = hacd::HaF32 (0.5f) + p.m_y * hacd::HaF32 (0.5f);
					attrib.m_v1 = hacd::HaF32 (0.5f) + p.m_z * hacd::HaF32 (0.5f);
					attrib.m_material = capMaterial;

					ptr = ptr->m_next;
				}while (ptr !=  edge);
			}
		}
	}

	ApplyAttributeArray (&attribArray[0]);
*/

	dgBigVector origin (GetOrigin());
	dgStack<dgBigVector>cylinder (m_pointCount);

	dgBigVector pMin (hacd::HaF64 (1.0e10f), hacd::HaF64 (1.0e10f), hacd::HaF64 (1.0e10f), hacd::HaF64 (0.0f));
	dgBigVector pMax (hacd::HaF64 (-1.0e10f), hacd::HaF64 (-1.0e10f), hacd::HaF64 (-1.0e10f), hacd::HaF64 (0.0f));
	for (hacd::HaI32 i = 0; i < m_pointCount; i ++) {
		dgBigVector tmp (m_points[i] - origin);
		pMin.m_x = GetMin (pMin.m_x, tmp.m_x);
		pMax.m_x = GetMax (pMax.m_x, tmp.m_x);
		pMin.m_y = GetMin (pMin.m_y, tmp.m_y);
		pMax.m_y = GetMax (pMax.m_y, tmp.m_y);
		pMin.m_z = GetMin (pMin.m_z, tmp.m_z);
		pMax.m_z = GetMax (pMax.m_z, tmp.m_z);
	}

	dgBigVector scale (hacd::HaF64 (1.0f)/ (pMax.m_x - pMin.m_x), hacd::HaF64 (1.0f)/ (pMax.m_x - pMin.m_x), hacd::HaF64 (1.0f)/ (pMax.m_x - pMin.m_x), hacd::HaF64 (0.0f));
	for (hacd::HaI32 i = 0; i < m_pointCount; i ++) {
		dgBigVector point (m_points[i] - origin);
		hacd::HaF64 u = (point.m_x - pMin.m_x) * scale.m_x;

		point = point.Scale (1.0f / dgSqrt (point % point));
		hacd::HaF64 v = dgAtan2 (point.m_y, point.m_z);

		//u = (hacd::HaF64 (3.1416f/2.0f) - u) / hacd::HaF64 (3.1416f);
		v = (hacd::HaF64 (3.1416f) - v) / hacd::HaF64 (2.0f * 3.1416f);
		cylinder[i].m_x = v;
		cylinder[i].m_y = u;
	}


	dgStack<dgVertexAtribute>attribArray (GetCount());
	EnumerateAttributeArray (&attribArray[0]);

	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		dgVertexAtribute& attrib = attribArray[hacd::HaI32 (edge->m_userData)];
		attrib.m_u0 = cylinder[edge->m_incidentVertex].m_x;
		attrib.m_v0 = cylinder[edge->m_incidentVertex].m_y;
		attrib.m_u1 = cylinder[edge->m_incidentVertex].m_x;
		attrib.m_v1 = cylinder[edge->m_incidentVertex].m_y;
		attrib.m_material = cylinderMaterial;
	}

	FixCylindricalMapping (&attribArray[0]);

	// apply cap mapping
	hacd::HaI32 mark = IncLRU();
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		if (edge->m_mark < mark){
			const dgVector& p0 = m_points[edge->m_incidentVertex];
			const dgVector& p1 = m_points[edge->m_next->m_incidentVertex];
			const dgVector& p2 = m_points[edge->m_prev->m_incidentVertex];

			edge->m_mark = mark;
			edge->m_next->m_mark = mark;
			edge->m_prev->m_mark = mark;

			dgVector e0 (p1 - p0);
			dgVector e1 (p2 - p0);
			dgVector n (e0 * e1);
			if ((n.m_x * n.m_x) > (hacd::HaF32 (0.99f) * (n % n))) {
				dgEdge* ptr = edge;
				do {
					dgVertexAtribute& attrib = attribArray[hacd::HaI32 (ptr->m_userData)];
					dgVector p (m_points[ptr->m_incidentVertex] - origin);
					hacd::HaF64 u = (p.m_y - pMin.m_y) * scale.m_y;
					hacd::HaF64 v = (p.m_z - pMin.m_z) * scale.m_z;
					attrib.m_u0 = u;
					attrib.m_v0 = v;
					attrib.m_u1 = u;
					attrib.m_v1 = v;
					attrib.m_material = capMaterial;

					ptr = ptr->m_next;
				}while (ptr !=  edge);
			}
		}
	}

	ApplyAttributeArray (&attribArray[0]);
}

void dgMeshEffect::BoxMapping (hacd::HaI32 front, hacd::HaI32 side, hacd::HaI32 top)
{
	dgBigVector minVal;
	dgBigVector maxVal;
	hacd::HaI32 materialArray[3];

	GetMinMax (minVal, maxVal, &m_points[0][0], m_pointCount, sizeof (dgBigVector));
	dgBigVector dist (maxVal - minVal);
	dgBigVector scale (hacd::HaF64 (1.0f)/ dist[0], hacd::HaF64 (1.0f)/ dist[1], hacd::HaF64 (1.0f)/ dist[2], hacd::HaF64 (0.0f));

	dgStack<dgVertexAtribute>attribArray (GetCount());
	EnumerateAttributeArray (&attribArray[0]);

	materialArray[0] = front;
	materialArray[1] = side;
	materialArray[2] = top;

	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		if (edge->m_mark < mark){
			const dgBigVector& p0 = m_points[edge->m_incidentVertex];
			const dgBigVector& p1 = m_points[edge->m_next->m_incidentVertex];
			const dgBigVector& p2 = m_points[edge->m_prev->m_incidentVertex];

			edge->m_mark = mark;
			edge->m_next->m_mark = mark;
			edge->m_prev->m_mark = mark;

			dgBigVector e0 (p1 - p0);
			dgBigVector e1 (p2 - p0);
			dgBigVector n (e0 * e1);
			
			hacd::HaI32 index = 0;
			hacd::HaF64 maxProjection = hacd::HaF32 (0.0f);

			for (hacd::HaI32 i = 0; i < 3; i ++) {
				hacd::HaF64 proj = fabs (n[i]);
				if (proj > maxProjection) {
					index = i;
					maxProjection = proj;
				}
			}

			hacd::HaI32 u = (index + 1) % 3;
			hacd::HaI32 v = (u + 1) % 3;
			if (index == 1) {
				Swap (u, v);
			}
			dgEdge* ptr = edge;
			do {
				dgVertexAtribute& attrib = attribArray[hacd::HaI32 (ptr->m_userData)];
				dgBigVector p (scale.CompProduct(m_points[ptr->m_incidentVertex] - minVal));
				attrib.m_u0 = p[u];
				attrib.m_v0 = p[v];
				attrib.m_u1 = hacd::HaF64(0.0f);
				attrib.m_v1 = hacd::HaF64(0.0f);
				attrib.m_material = materialArray[index];

				ptr = ptr->m_next;
			}while (ptr !=  edge);
		}
	}

	ApplyAttributeArray (&attribArray[0]);
}

void dgMeshEffect::UniformBoxMapping (hacd::HaI32 material, const dgMatrix& textureMatrix)
{
	dgStack<dgVertexAtribute>attribArray (GetCount());
	EnumerateAttributeArray (&attribArray[0]);

	hacd::HaI32 mark = IncLRU();
	for (hacd::HaI32 i = 0; i < 3; i ++) {
		dgMatrix rotationMatrix (dgGetIdentityMatrix());
		if (i == 1) {
			rotationMatrix = dgYawMatrix(hacd::HaF32 (90.0f * 3.1416f / 180.0f));
		} else if (i == 2) {
			rotationMatrix = dgPitchMatrix(hacd::HaF32 (90.0f * 3.1416f / 180.0f));
		}

		dgPolyhedra::Iterator iter (*this);	

		for(iter.Begin(); iter; iter ++){
			dgEdge* const edge = &(*iter);
			if (edge->m_mark < mark){
				dgBigVector n (FaceNormal(edge, &m_points[0].m_x, sizeof (dgBigVector)));
				dgVector normal (rotationMatrix.RotateVector(dgVector (n.Scale (hacd::HaF64 (1.0f) / sqrt (n % n)))));
				normal.m_x = dgAbsf (normal.m_x);
				normal.m_y = dgAbsf (normal.m_y);
				normal.m_z = dgAbsf (normal.m_z);
				if ((normal.m_z >= (normal.m_x - hacd::HaF32 (1.0e-4f))) && (normal.m_z >= (normal.m_y - hacd::HaF32 (1.0e-4f)))) {
					dgEdge* ptr = edge;
					do {
						ptr->m_mark = mark;
						dgVertexAtribute& attrib = attribArray[hacd::HaI32 (ptr->m_userData)];
						dgVector p (textureMatrix.TransformVector(rotationMatrix.RotateVector(m_points[ptr->m_incidentVertex])));
						attrib.m_u0 = p.m_x;
						attrib.m_v0 = p.m_y;
						attrib.m_u1 = hacd::HaF32 (0.0f);
						attrib.m_v1 = hacd::HaF32 (0.0f);
						attrib.m_material = material;
						ptr = ptr->m_next;
					}while (ptr !=  edge);
				}
			}
		}
	}

	ApplyAttributeArray (&attribArray[0]);
}

void dgMeshEffect::CalculateNormals (hacd::HaF64 angleInRadians)
{
	dgStack<dgBigVector>faceNormal (GetCount());
	dgStack<dgVertexAtribute>attribArray (GetCount());
	EnumerateAttributeArray (&attribArray[0]);

	hacd::HaI32 faceIndex = 1;
	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);	
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		if ((edge->m_mark < mark) && (edge->m_incidentFace > 0)) {
			dgEdge* ptr = edge;
			do {
				ptr->m_incidentFace = faceIndex;
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr !=  edge);

			dgBigVector normal (FaceNormal (edge, &m_points[0].m_x, sizeof (m_points[0])));
			normal = normal.Scale (hacd::HaF32 (1.0f) / (sqrt(normal % normal) + hacd::HaF32(1.0e-16f)));
			faceNormal[faceIndex] = normal;
			faceIndex ++;
		}
	}


	hacd::HaF32 smoothValue = dgCos (angleInRadians); 
//smoothValue = -1;
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		if (edge->m_incidentFace > 0) {
			dgBigVector normal0 (faceNormal[edge->m_incidentFace]);

			dgEdge* startEdge = edge;
			for (dgEdge* ptr = edge->m_prev->m_twin ; (ptr != edge) && (ptr->m_incidentFace > 0); ptr = ptr->m_prev->m_twin) {
				const dgBigVector& normal1 (faceNormal[ptr->m_incidentFace]);
				hacd::HaF64 dot = normal0 % normal1;
				if (dot < smoothValue) {
					break;
				}
				startEdge = ptr;
				normal0 = normal1;
			}

			dgBigVector normal (faceNormal[startEdge->m_incidentFace]);
			normal0 = normal;
			for (dgEdge* ptr = startEdge->m_twin->m_next; (ptr != startEdge) && (ptr->m_incidentFace > 0); ptr = ptr->m_twin->m_next) { 
				
				const dgBigVector& normal1 (faceNormal[ptr->m_incidentFace]);
				hacd::HaF64 dot = normal0 % normal1;
				if (dot < smoothValue)  {
					break;
				}
				normal += normal1;
				normal0 = normal1;
			} 

			normal = normal.Scale (hacd::HaF32 (1.0f) / (sqrt(normal % normal) + hacd::HaF32(1.0e-16f)));
			hacd::HaI32 edgeIndex = hacd::HaI32 (edge->m_userData);
			dgVertexAtribute& attrib = attribArray[edgeIndex];
			attrib.m_normal_x = normal.m_x;
			attrib.m_normal_y = normal.m_y;
			attrib.m_normal_z = normal.m_z;
		}
	}

	ApplyAttributeArray (&attribArray[0]);
}


void dgMeshEffect::BeginPolygon ()
{
	m_pointCount = 0;
	m_atribCount = 0;
	RemoveAll();
	BeginFace();
}


void dgMeshEffect::AddAtribute (const dgVertexAtribute& attib)
{
	if (m_atribCount >= m_maxAtribCount) {
		m_maxAtribCount *= 2;
		dgVertexAtribute* const attibArray = (dgVertexAtribute*) HACD_ALLOC(hacd::HaI32 (m_maxAtribCount * sizeof(dgVertexAtribute)));
		memcpy (attibArray, m_attib, m_atribCount * sizeof(dgVertexAtribute));
		HACD_FREE(m_attib);
		m_attib = attibArray;
	}

	m_attib[m_atribCount] = attib;
	m_attib[m_atribCount].m_vertex.m_x = QuantizeCordinade(m_attib[m_atribCount].m_vertex.m_x);
	m_attib[m_atribCount].m_vertex.m_y = QuantizeCordinade(m_attib[m_atribCount].m_vertex.m_y);
	m_attib[m_atribCount].m_vertex.m_z = QuantizeCordinade(m_attib[m_atribCount].m_vertex.m_z);
	m_atribCount ++;
}

void dgMeshEffect::AddVertex(const dgBigVector& vertex)
{
	if (m_pointCount >= m_maxPointCount) {
		m_maxPointCount *= 2;
		dgBigVector* const points = (dgBigVector*) HACD_ALLOC(hacd::HaI32 (m_maxPointCount * sizeof(dgBigVector)));
		memcpy (points, m_points, m_pointCount * sizeof(dgBigVector));
		HACD_FREE(m_points);
		m_points = points;
	}
	
	m_points[m_pointCount].m_x = QuantizeCordinade(vertex[0]);
	m_points[m_pointCount].m_y = QuantizeCordinade(vertex[1]);
	m_points[m_pointCount].m_z = QuantizeCordinade(vertex[2]);
	m_points[m_pointCount].m_w = vertex.m_w;
	m_pointCount ++;
}


void dgMeshEffect::AddPoint(const hacd::HaF64* vertex, hacd::HaI32 material)
{
	dgVertexAtribute attib;
	AddVertex(dgBigVector (vertex[0], vertex[1], vertex[2], vertex[3]));
	
	attib.m_vertex.m_x = m_points[m_pointCount - 1].m_x;
	attib.m_vertex.m_y = m_points[m_pointCount - 1].m_y;
	attib.m_vertex.m_z = m_points[m_pointCount - 1].m_z;
	attib.m_vertex.m_w = m_points[m_pointCount - 1].m_w;

	attib.m_normal_x = vertex[4];
	attib.m_normal_y = vertex[5];
	attib.m_normal_z = vertex[6];
	attib.m_u0 = vertex[7];
	attib.m_v0 = vertex[8];
	attib.m_u1 = vertex[9];
	attib.m_v1 = vertex[10];
	attib.m_material = material;

	AddAtribute (attib);
}

void dgMeshEffect::PackVertexArrays ()
{
	if (m_maxPointCount > m_pointCount) {
		dgBigVector* const points = (dgBigVector*) HACD_ALLOC(hacd::HaI32 (m_pointCount * sizeof(dgBigVector)));
		memcpy (points, m_points, m_pointCount * sizeof(dgBigVector));
		HACD_FREE(m_points);
		m_points = points;
		m_maxPointCount = m_pointCount;
	}


	if (m_maxAtribCount > m_atribCount) {
		dgVertexAtribute* const attibArray = (dgVertexAtribute*) HACD_ALLOC(hacd::HaI32 (m_atribCount * sizeof(dgVertexAtribute)));
		memcpy (attibArray, m_attib, m_atribCount * sizeof(dgVertexAtribute));
		HACD_FREE(m_attib);
		m_attib = attibArray;
		m_maxAtribCount = m_atribCount;
	}
};


void dgMeshEffect::AddPolygon (hacd::HaI32 count, const hacd::HaF64* const vertexList, hacd::HaI32 strideIndBytes, hacd::HaI32 material)
{
	hacd::HaI32 stride = hacd::HaI32 (strideIndBytes / sizeof (hacd::HaF64));

	if (count > 3) {
		dgPolyhedra polygon;
		hacd::HaI32 indexList[256];
		HACD_ASSERT (count < hacd::HaI32 (sizeof (indexList)/sizeof(indexList[0])));
		for (hacd::HaI32 i = 0; i < count; i ++) {
			indexList[i] = i;
		}

		polygon.BeginFace();
		polygon.AddFace(count, indexList, NULL);
		polygon.EndFace();
		polygon.Triangulate(vertexList, strideIndBytes, NULL);

		hacd::HaI32 mark = polygon.IncLRU();
		dgPolyhedra::Iterator iter (polygon);
		for (iter.Begin(); iter; iter ++) {
			dgEdge* const edge = &iter.GetNode()->GetInfo();
			if ((edge->m_incidentFace > 0) && (edge->m_mark < mark)) {
				hacd::HaI32 i0 = edge->m_incidentVertex;
				hacd::HaI32 i1 = edge->m_next->m_incidentVertex;
				hacd::HaI32 i2 = edge->m_next->m_next->m_incidentVertex;
				edge->m_mark = mark;
				edge->m_next->m_mark = mark;
				edge->m_next->m_next->m_mark = mark;

//				#ifdef _DEBUG
//					dgBigVector p0_ (&vertexList[i0 * stride]);
//					dgBigVector p1_ (&vertexList[i1 * stride]);
//					dgBigVector p2_ (&vertexList[i2 * stride]);
//					dgBigVector e1_ (p1_ - p0_);
//					dgBigVector e2_ (p2_ - p0_);
//					dgBigVector n_ (e1_ * e2_);
//					hacd::HaF64 mag2_ = n_ % n_;
//					HACD_ASSERT (mag2_ > hacd::HaF32 (DG_MESH_EFFECT_PRECISION_SCALE_INV * DG_MESH_EFFECT_PRECISION_SCALE_INV)); 
//				#endif

				AddPoint(vertexList + i0 * stride, material);
				AddPoint(vertexList + i1 * stride, material);
				AddPoint(vertexList + i2 * stride, material);

				#ifdef _DEBUG
					const dgBigVector& p0 = m_points[m_pointCount - 3];
					const dgBigVector& p1 = m_points[m_pointCount - 2];
					const dgBigVector& p2 = m_points[m_pointCount - 1];
					dgBigVector e1 (p1 - p0);
					dgBigVector e2 (p2 - p0);
					dgBigVector n (e1 * e2);
					hacd::HaF64 mag3 = n % n;
					HACD_ASSERT (mag3 > hacd::HaF64 (DG_MESH_EFFECT_PRECISION_SCALE_INV * DG_MESH_EFFECT_PRECISION_SCALE_INV));
				#endif
			}
		}

	} else {

		AddPoint(vertexList, material);
		AddPoint(vertexList + stride, material);
		AddPoint(vertexList + stride + stride, material);

		const dgBigVector& p0 = m_points[m_pointCount - 3];
		const dgBigVector& p1 = m_points[m_pointCount - 2];
		const dgBigVector& p2 = m_points[m_pointCount - 1];
		dgBigVector e1 (p1 - p0);
		dgBigVector e2 (p2 - p0);
		dgBigVector n (e1 * e2);
		hacd::HaF64 mag3 = n % n;
		if (mag3 < hacd::HaF64 (DG_MESH_EFFECT_PRECISION_SCALE_INV * DG_MESH_EFFECT_PRECISION_SCALE_INV)) {
			m_pointCount -= 3;
			m_atribCount -= 3;
		}
	}
}


void dgMeshEffect::AddPolygon (hacd::HaI32 count, const hacd::HaF32* const vertexList, hacd::HaI32 strideIndBytes, hacd::HaI32 material)
{
	dgVertexAtribute points[256];
	HACD_ASSERT (count < hacd::HaI32 (sizeof (points)/sizeof (points[0])));

	hacd::HaI32 stride = strideIndBytes / sizeof (hacd::HaF32);
	for (hacd::HaI32 i = 0; i < count; i ++) 
	{
		points[i].m_vertex.m_x = vertexList[i * stride + 0];
		points[i].m_vertex.m_y = vertexList[i * stride + 1];
		points[i].m_vertex.m_z = vertexList[i * stride + 2];
		points[i].m_vertex.m_w = vertexList[i * stride + 3];

		points[i].m_normal_x = vertexList[i * stride + 4];
		points[i].m_normal_y = vertexList[i * stride + 5];
		points[i].m_normal_z = vertexList[i * stride + 6];

		points[i].m_u0 = vertexList[i * stride + 7];
		points[i].m_v0 = vertexList[i * stride + 8];

		points[i].m_u1 = vertexList[i * stride + 9];
		points[i].m_u1 = vertexList[i * stride + 10];
	}

	AddPolygon (count, &points[0].m_vertex.m_x, sizeof (dgVertexAtribute), material);
}

void dgMeshEffect::EndPolygon (hacd::HaF64 tol)
{
	dgStack<hacd::HaI32>indexMap(m_pointCount);
	dgStack<hacd::HaI32>attrIndexMap(m_atribCount);

#ifdef _DEBUG
	for (hacd::HaI32 i = 0; i < m_pointCount; i += 3) {
		dgBigVector p0 (m_points[i + 0]);
		dgBigVector p1 (m_points[i + 1]);
		dgBigVector p2 (m_points[i + 2]);
		dgBigVector e1 (p1 - p0);
		dgBigVector e2 (p2 - p0);
		dgBigVector n (e1 * e2);
		hacd::HaF64 mag2 = n % n;
//		HACD_ASSERT (mag2 > DG_MESH_EFFECT_TRIANGLE_MIN_AREA);
		HACD_ASSERT (mag2 > hacd::HaF32 (0.0f));
	}
#endif

	hacd::HaI32 triangCount = m_pointCount / 3;
	m_pointCount = dgVertexListToIndexList (&m_points[0].m_x, sizeof (dgBigVector), sizeof (dgBigVector)/sizeof (hacd::HaF64), m_pointCount, &indexMap[0], tol);
	m_atribCount = dgVertexListToIndexList (&m_attib[0].m_vertex.m_x, sizeof (dgVertexAtribute), sizeof (dgVertexAtribute)/sizeof (hacd::HaF64), m_atribCount, &attrIndexMap[0], tol);

	for (hacd::HaI32 i = 0; i < triangCount; i ++) {
		hacd::HaI32 index[3];
		hacd::HaI64 userdata[3];

		index[0] = indexMap[i * 3 + 0];
		index[1] = indexMap[i * 3 + 1];
		index[2] = indexMap[i * 3 + 2];


		dgBigVector e1 (m_points[index[1]] - m_points[index[0]]);
		dgBigVector e2 (m_points[index[2]] - m_points[index[0]]);

		dgBigVector n (e1 * e2);
		hacd::HaF64 mag2 = n % n;
		if (mag2 > hacd::HaF64 (1.0e-12f)) {
			userdata[0] = attrIndexMap[i * 3 + 0];
			userdata[1] = attrIndexMap[i * 3 + 1];
			userdata[2] = attrIndexMap[i * 3 + 2];
			dgEdge* const edge = AddFace (3, index, userdata);
			if (!edge) {
				HACD_ASSERT ((m_pointCount + 3) <= m_maxPointCount);

				m_points[m_pointCount + 0] = m_points[index[0]];
				m_points[m_pointCount + 1] = m_points[index[1]];
				m_points[m_pointCount + 2] = m_points[index[2]];

				index[0] = m_pointCount + 0;
				index[1] = m_pointCount + 1;
				index[2] = m_pointCount + 2;

				m_pointCount += 3;

				#ifdef _DEBUG
					dgEdge* test = AddFace (3, index, userdata);
					HACD_ASSERT (test);
				#else 
					AddFace (3, index, userdata);
				#endif
			}
		}
	}
	EndFace();

	RepairTJoints (true);


#ifdef _DEBUG
	dgPolyhedra::Iterator iter (*this);	
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if (face->m_incidentFace > 0) {
			dgBigVector p0 (m_points[face->m_incidentVertex]);
			dgBigVector p1 (m_points[face->m_next->m_incidentVertex]);
			dgBigVector p2 (m_points[face->m_next->m_next->m_incidentVertex]);
			dgBigVector e1 (p1 - p0);
			dgBigVector e2 (p2 - p0);
			dgBigVector n (e1 * e2);
//			HACD_ASSERT (mag2 > DG_MESH_EFFECT_TRIANGLE_MIN_AREA);
			hacd::HaF64 mag2 = n % n;
			HACD_ASSERT (mag2 > hacd::HaF32 (0.0f));
		}
	}
#endif
}


void dgMeshEffect::BuildFromVertexListIndexList(
	hacd::HaI32 faceCount, const hacd::HaI32* const faceIndexCount, const hacd::HaI32* const faceMaterialIndex, 
	const hacd::HaF32* const vertex, hacd::HaI32 vertexStrideInBytes, const hacd::HaI32* const vertexIndex,
	const hacd::HaF32* const normal, hacd::HaI32  normalStrideInBytes, const hacd::HaI32* const normalIndex,
	const hacd::HaF32* const uv0, hacd::HaI32  uv0StrideInBytes, const hacd::HaI32* const uv0Index,
	const hacd::HaF32* const uv1, hacd::HaI32  uv1StrideInBytes, const hacd::HaI32* const uv1Index)
{
	BeginPolygon ();

	// calculate vertex Count
	hacd::HaI32 acc = 0;
	hacd::HaI32 vertexCount = 0;
	for (hacd::HaI32 j = 0; j < faceCount; j ++) {
		int count = faceIndexCount[j];
		for (int i = 0; i < count; i ++) {
			vertexCount = GetMax(vertexCount, vertexIndex[acc + i] + 1);
		}
		acc += count;
	}
		
	hacd::HaI32 layerCountBase = 0;
	hacd::HaI32 vertexStride = hacd::HaI32 (vertexStrideInBytes / sizeof (hacd::HaF32));
	for (int i = 0; i < vertexCount; i ++) {
		int index = i * vertexStride;
		AddVertex (dgBigVector (vertex[index + 0], vertex[index + 1], vertex[index + 2], vertex[index + 3]));
		layerCountBase += (vertex[index + 3]) > hacd::HaF32(layerCountBase);
	}


	acc = 0;
	hacd::HaI32 normalStride = hacd::HaI32 (normalStrideInBytes / sizeof (hacd::HaF32));
	hacd::HaI32 uv0Stride = hacd::HaI32 (uv0StrideInBytes / sizeof (hacd::HaF32));
	hacd::HaI32 uv1Stride = hacd::HaI32 (uv1StrideInBytes / sizeof (hacd::HaF32));
	for (hacd::HaI32 j = 0; j < faceCount; j ++) {
		hacd::HaI32 indexCount = faceIndexCount[j];
		hacd::HaI32 materialIndex = faceMaterialIndex[j];
		for (hacd::HaI32 i = 0; i < indexCount; i ++) {
			dgVertexAtribute point;
			hacd::HaI32 index = vertexIndex[acc + i];
			point.m_vertex = m_points[index];
			
			index = normalIndex[(acc + i)] * normalStride;
			point.m_normal_x =  normal[index + 0];
			point.m_normal_y =  normal[index + 1];
			point.m_normal_z =  normal[index + 2];

			index = uv0Index[(acc + i)] * uv0Stride;
			point.m_u0 = uv0[index + 0];
			point.m_v0 = uv0[index + 1];
			
			index = uv1Index[(acc + i)] * uv1Stride;
			point.m_u1 = uv1[index + 0];
			point.m_v1 = uv1[index + 1];

			point.m_material = materialIndex;
			AddAtribute(point);
		}
		acc += indexCount;
	}


	dgStack<hacd::HaI32>attrIndexMap(m_atribCount);
	m_atribCount = dgVertexListToIndexList (&m_attib[0].m_vertex.m_x, sizeof (dgVertexAtribute), sizeof (dgVertexAtribute) / sizeof (hacd::HaF64), m_atribCount, &attrIndexMap[0], DG_VERTEXLIST_INDEXLIST_TOL);

	bool hasFaces = true;
	dgStack<hacd::HaI8> faceMark (faceCount);
	memset (&faceMark[0], 1, size_t (faceMark.GetSizeInBytes()));
	
	hacd::HaI32 layerCount = 0;
	while (hasFaces) {
		acc = 0;
		hasFaces = false;
		hacd::HaI32 vertexBank = layerCount * vertexCount;
		for (hacd::HaI32 j = 0; j < faceCount; j ++) {
			hacd::HaI32 index[256];
			hacd::HaI64 userdata[256];

			int indexCount = faceIndexCount[j];
			HACD_ASSERT (indexCount < hacd::HaI32 (sizeof (index) / sizeof (index[0])));

			if (faceMark[j]) {
				for (int i = 0; i < indexCount; i ++) {
					index[i] = vertexIndex[acc + i] + vertexBank;
					userdata[i] = attrIndexMap[acc + i];
				}
				dgEdge* const edge = AddFace (indexCount, index, userdata);
				if (edge) {
					faceMark[j] = 0;
				} else {
					// check if the face is not degenerated
					bool degeneratedFace = false;
					for (int i = 0; i < indexCount - 1; i ++) {
						for (int k = i + 1; k < indexCount; k ++) {
							if (index[i] == index[k]) {
								degeneratedFace = true;		
							}
						}
					}
					if (degeneratedFace) {
						faceMark[j] = 0;
					} else {
						hasFaces = true;
					}
				}
			}
			acc += indexCount;
		}
		if (hasFaces) {
			layerCount ++;
			for (int i = 0; i < vertexCount; i ++) {
				int index = i * vertexStride;
				AddVertex (dgBigVector (vertex[index + 0], vertex[index + 1], vertex[index + 2], hacd::HaF64 (layerCount + layerCountBase)));
			}
		}
	}

	EndFace();
	PackVertexArrays ();
//	RepairTJoints (true);
}


hacd::HaI32 dgMeshEffect::GetTotalFaceCount() const
{
	return GetFaceCount();
}

hacd::HaI32 dgMeshEffect::GetTotalIndexCount() const
{
	Iterator iter (*this);
	hacd::HaI32 count = 0;
	hacd::HaI32 mark = IncLRU();
	for (iter.Begin(); iter; iter ++) {
		dgEdge* const edge = &(*iter);
		if (edge->m_mark == mark) {
			continue;
		}

		if (edge->m_incidentFace < 0) {
			continue;
		}
		
		dgEdge* ptr = edge;
		do {
			count ++;
			ptr->m_mark = mark;
			ptr = ptr->m_next;
		} while (ptr != edge);
	}
	return count;
}

void dgMeshEffect::GetFaces (hacd::HaI32* const facesIndex, hacd::HaI32* const materials, void** const faceNodeList) const
{
	Iterator iter (*this);

	hacd::HaI32 faces = 0;
	hacd::HaI32 indexCount = 0;
	hacd::HaI32 mark = IncLRU();
	for (iter.Begin(); iter; iter ++) {
		dgEdge* const edge = &(*iter);
		if (edge->m_mark == mark) {
			continue;
		}

		if (edge->m_incidentFace < 0) {
			continue;
		}

		hacd::HaI32 faceCount = 0;
		dgEdge* ptr = edge;
		do {
//			indexList[indexCount] = hacd::HaI32 (ptr->m_userData);
			faceNodeList[indexCount] = GetNodeFromInfo (*ptr);
			indexCount ++;
			faceCount ++;
			ptr->m_mark = mark;
			ptr = ptr->m_next;
		} while (ptr != edge);

		facesIndex[faces] = faceCount;
		materials[faces] = dgFastInt(m_attib[hacd::HaI32 (edge->m_userData)].m_material);
		faces ++;
	}
}

void* dgMeshEffect::GetFirstVertex ()
{
	Iterator iter (*this);
	iter.Begin();

	dgTreeNode* node = NULL;
	if (iter) {
		hacd::HaI32 mark = IncLRU();
		node = iter.GetNode();

		dgEdge* const edge = &node->GetInfo();
		dgEdge* ptr = edge;
		do {
			ptr->m_mark = mark;
			ptr = ptr->m_twin->m_next;
		} while (ptr != edge);
	}
	return node; 
}

void* dgMeshEffect::GetNextVertex (const void* const vertex)
{
	dgTreeNode* node = (dgTreeNode*) vertex;
	hacd::HaI32 mark = node->GetInfo().m_mark;

	Iterator iter (*this);
	iter.Set (node);
	for (iter ++; iter; iter ++) {
		dgTreeNode* node = iter.GetNode();
		if (node->GetInfo().m_mark != mark) {
			dgEdge* const edge = &node->GetInfo();
			dgEdge* ptr = edge;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_twin->m_next;
			} while (ptr != edge);
			return node; 
		}
	}
	return NULL; 
}

hacd::HaI32 dgMeshEffect::GetVertexIndex (const void* const vertex) const
{
	dgTreeNode* const node = (dgTreeNode*) vertex;
	dgEdge* const edge = &node->GetInfo();
	return edge->m_incidentVertex;
}


void* dgMeshEffect::GetFirstPoint ()
{
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		dgTreeNode* const node = iter.GetNode();
		dgEdge* const edge = &node->GetInfo();
		if (edge->m_incidentFace > 0) {
			return node;
		}
	}
	return NULL; 
}

void* dgMeshEffect::GetNextPoint (const void* const point)
{
	Iterator iter (*this);
	iter.Set ((dgTreeNode*) point);
	for (iter ++; iter; iter ++) {
		dgTreeNode* const node = iter.GetNode();
		dgEdge* const edge = &node->GetInfo();
		if (edge->m_incidentFace > 0) {
			return node; 
		}
	}
	return NULL; 
}

hacd::HaI32 dgMeshEffect::GetPointIndex (const void* const point) const
{
	dgTreeNode* const node = (dgTreeNode*) point;
	dgEdge* const edge = &node->GetInfo();
	return int (edge->m_userData);
}

hacd::HaI32 dgMeshEffect::GetVertexIndexFromPoint (const void* const point) const
{
	return GetVertexIndex (point);
}


dgEdge* dgMeshEffect::ConectVertex (dgEdge* const e0, dgEdge* const e1)
{
	dgEdge* const edge = AddHalfEdge(e1->m_incidentVertex, e0->m_incidentVertex);
	dgEdge* const twin = AddHalfEdge(e0->m_incidentVertex, e1->m_incidentVertex);
	HACD_ASSERT ((edge && twin) || !(edge || twin));
	if (edge) {
		edge->m_twin = twin;
		twin->m_twin = edge;

		edge->m_incidentFace = e0->m_incidentFace;
		twin->m_incidentFace = e1->m_incidentFace;

		edge->m_userData = e1->m_userData;
		twin->m_userData = e0->m_userData;

		edge->m_next = e0;
		edge->m_prev = e1->m_prev;

		twin->m_next = e1;
		twin->m_prev = e0->m_prev;

		e0->m_prev->m_next = twin;
		e0->m_prev = edge;

		e1->m_prev->m_next = edge;
		e1->m_prev = twin;
	}

	return edge;
}


//hacd::HaI32 dgMeshEffect::GetVertexAttributeIndex (const void* vertex) const
//{
//	dgTreeNode* node = (dgTreeNode*) vertex;
//	dgEdge* const edge = &node->GetInfo();
//	return int (edge->m_userData);
//}


void* dgMeshEffect::GetFirstEdge ()
{
	Iterator iter (*this);
	iter.Begin();

	dgTreeNode* node = NULL;
	if (iter) {
		hacd::HaI32 mark = IncLRU();

		node = iter.GetNode();

		dgEdge* const edge = &node->GetInfo();
		edge->m_mark = mark;
		edge->m_twin->m_mark = mark;
	}
	return node; 
}

void* dgMeshEffect::GetNextEdge (const void* const edge)
{
	dgTreeNode* node = (dgTreeNode*) edge;
	hacd::HaI32 mark = node->GetInfo().m_mark;

	Iterator iter (*this);
	iter.Set (node);
	for (iter ++; iter; iter ++) {
		dgTreeNode* node = iter.GetNode();
		if (node->GetInfo().m_mark != mark) {
			node->GetInfo().m_mark = mark;
			node->GetInfo().m_twin->m_mark = mark;
			return node; 
		}
	}
	return NULL; 
}

void dgMeshEffect::GetEdgeIndex (const void* const edge, hacd::HaI32& v0, hacd::HaI32& v1) const
{
	dgTreeNode* node = (dgTreeNode*) edge;
	v0 = node->GetInfo().m_incidentVertex;
	v1 = node->GetInfo().m_twin->m_incidentVertex;
}

//void dgMeshEffect::GetEdgeAttributeIndex (const void* edge, hacd::HaI32& v0, hacd::HaI32& v1) const
//{
//	dgTreeNode* node = (dgTreeNode*) edge;
//	v0 = int (node->GetInfo().m_userData);
//	v1 = int (node->GetInfo().m_twin->m_userData);
//}


void* dgMeshEffect::GetFirstFace ()
{
	Iterator iter (*this);
	iter.Begin();

	dgTreeNode* node = NULL;
	if (iter) {
		hacd::HaI32 mark = IncLRU();
		node = iter.GetNode();

		dgEdge* const edge = &node->GetInfo();
		dgEdge* ptr = edge;
		do {
			ptr->m_mark = mark;
			ptr = ptr->m_next;
		} while (ptr != edge);
	}

	return node;
}

void* dgMeshEffect::GetNextFace (const void* const face)
{
	dgTreeNode* node = (dgTreeNode*) face;
	hacd::HaI32 mark = node->GetInfo().m_mark;

	Iterator iter (*this);
	iter.Set (node);
	for (iter ++; iter; iter ++) {
		dgTreeNode* node = iter.GetNode();
		if (node->GetInfo().m_mark != mark) {
			dgEdge* const edge = &node->GetInfo();
			dgEdge* ptr = edge;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != edge);
			return node; 
		}
	}
	return NULL; 
}


hacd::HaI32 dgMeshEffect::IsFaceOpen (const void* const face) const
{
	dgTreeNode* node = (dgTreeNode*) face;
	dgEdge* const edge = &node->GetInfo();
	return (edge->m_incidentFace > 0) ? 0 : 1;
}

hacd::HaI32 dgMeshEffect::GetFaceMaterial (const void* const face) const
{
	dgTreeNode* const node = (dgTreeNode*) face;
	dgEdge* const edge = &node->GetInfo();
	return hacd::HaI32 (m_attib[edge->m_userData].m_material);
}

hacd::HaI32 dgMeshEffect::GetFaceIndexCount (const void* const face) const
{
	int count = 0;
	dgTreeNode* node = (dgTreeNode*) face;
	dgEdge* const edge = &node->GetInfo();
	dgEdge* ptr = edge;
	do {
		count ++;
		ptr = ptr->m_next;
	} while (ptr != edge);
	return count; 
}

void dgMeshEffect::GetFaceIndex (const void* const face, int* const indices) const
{
	int count = 0;
	dgTreeNode* node = (dgTreeNode*) face;
	dgEdge* const edge = &node->GetInfo();
	dgEdge* ptr = edge;
	do {
		indices[count] =  ptr->m_incidentVertex;
		count ++;
		ptr = ptr->m_next;
	} while (ptr != edge);
}

void dgMeshEffect::GetFaceAttributeIndex (const void* const face, int* const indices) const
{
	int count = 0;
	dgTreeNode* node = (dgTreeNode*) face;
	dgEdge* const edge = &node->GetInfo();
	dgEdge* ptr = edge;
	do {
		indices[count] = int (ptr->m_userData);
		count ++;
		ptr = ptr->m_next;
	} while (ptr != edge);
}




/*
hacd::HaI32 GetTotalFaceCount() const;
{
	hacd::HaI32 mark;
	hacd::HaI32 count;
	hacd::HaI32 materialCount;
	hacd::HaI32 materials[256];
	hacd::HaI32 streamIndexMap[256];
	dgIndexArray* array; 

	count = 0;
	materialCount = 0;

	array = (dgIndexArray*) HACD_ALLOC (4 * sizeof (hacd::HaI32) * GetCount() + sizeof (dgIndexArray) + 2048);
	array->m_indexList = (hacd::HaI32*)&array[1];

	mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);	
	memset(streamIndexMap, 0, sizeof (streamIndexMap));
	for(iter.Begin(); iter; iter ++){

		dgEdge* const edge;
		edge = &(*iter);
		if ((edge->m_incidentFace >= 0) && (edge->m_mark != mark)) {
			dgEdge* ptr;
			hacd::HaI32 hashValue;
			hacd::HaI32 index0;
			hacd::HaI32 index1;

			ptr = edge;
			ptr->m_mark = mark;
			index0 = hacd::HaI32 (ptr->m_userData);

			ptr = ptr->m_next;
			ptr->m_mark = mark;
			index1 = hacd::HaI32 (ptr->m_userData);

			ptr = ptr->m_next;
			do {
				ptr->m_mark = mark;

				array->m_indexList[count * 4 + 0] = index0;
				array->m_indexList[count * 4 + 1] = index1;
				array->m_indexList[count * 4 + 2] = hacd::HaI32 (ptr->m_userData);
				array->m_indexList[count * 4 + 3] = m_attib[hacd::HaI32 (edge->m_userData)].m_material;
				index1 = hacd::HaI32 (ptr->m_userData);

				hashValue = array->m_indexList[count * 4 + 3] & 0xff;
				streamIndexMap[hashValue] ++;
				materials[hashValue] = array->m_indexList[count * 4 + 3];
				count ++;

				ptr = ptr->m_next;
			} while (ptr != edge);
		}
	}
*/




void dgMeshEffect::GetVertexStreams (hacd::HaI32 vetexStrideInByte, hacd::HaF32* const vertex, 
									 hacd::HaI32 normalStrideInByte, hacd::HaF32* const normal, 
									 hacd::HaI32 uvStrideInByte0, hacd::HaF32* const uv0, 
									 hacd::HaI32 uvStrideInByte1, hacd::HaF32* const uv1)
{
	uvStrideInByte0 /= sizeof (hacd::HaF32);
	uvStrideInByte1 /= sizeof (hacd::HaF32);
	vetexStrideInByte /= sizeof (hacd::HaF32);
	normalStrideInByte /= sizeof (hacd::HaF32);
	for (hacd::HaI32 i = 0; i < m_atribCount; i ++)	{
		hacd::HaI32 j = i * vetexStrideInByte;
		vertex[j + 0] = hacd::HaF32 (m_attib[i].m_vertex.m_x);
		vertex[j + 1] = hacd::HaF32 (m_attib[i].m_vertex.m_y);
		vertex[j + 2] = hacd::HaF32 (m_attib[i].m_vertex.m_z);

		j = i * normalStrideInByte;
		normal[j + 0] = hacd::HaF32 (m_attib[i].m_normal_x);
		normal[j + 1] = hacd::HaF32 (m_attib[i].m_normal_y);
		normal[j + 2] = hacd::HaF32 (m_attib[i].m_normal_z);

		j = i * uvStrideInByte1;
		uv1[j + 0] = hacd::HaF32 (m_attib[i].m_u1);
		uv1[j + 1] = hacd::HaF32 (m_attib[i].m_v1);

		j = i * uvStrideInByte0;
		uv0[j + 0] = hacd::HaF32 (m_attib[i].m_u0);
		uv0[j + 1] = hacd::HaF32 (m_attib[i].m_v0);
	}
}


void dgMeshEffect::GetIndirectVertexStreams(
	hacd::HaI32 vetexStrideInByte, hacd::HaF64* const vertex, hacd::HaI32* const vertexIndices, hacd::HaI32* const vertexCount,
	hacd::HaI32 normalStrideInByte, hacd::HaF64* const normal, hacd::HaI32* const normalIndices, hacd::HaI32* const normalCount,
	hacd::HaI32 uvStrideInByte0, hacd::HaF64* const uv0, hacd::HaI32* const uvIndices0, hacd::HaI32* const uvCount0,
	hacd::HaI32 uvStrideInByte1, hacd::HaF64* const uv1, hacd::HaI32* const uvIndices1, hacd::HaI32* const uvCount1)
{
/*
	GetVertexStreams (vetexStrideInByte, vertex, normalStrideInByte, normal, uvStrideInByte0, uv0, uvStrideInByte1, uv1);

	*vertexCount = dgVertexListToIndexList(vertex, vetexStrideInByte, vetexStrideInByte, 0, m_atribCount, vertexIndices, hacd::HaF32 (0.0f));
	*normalCount = dgVertexListToIndexList(normal, normalStrideInByte, normalStrideInByte, 0, m_atribCount, normalIndices, hacd::HaF32 (0.0f));

	dgTriplex* const tmpUV = (dgTriplex*) HACD_ALLOC (hacd::HaI32 (sizeof (dgTriplex) * m_atribCount));
	hacd::HaI32 stride = hacd::HaI32 (uvStrideInByte1 /sizeof (hacd::HaF32));
	for (hacd::HaI32 i = 0; i < m_atribCount; i ++){
		tmpUV[i].m_x = uv1[i * stride + 0];
		tmpUV[i].m_y = uv1[i * stride + 1];
		tmpUV[i].m_z = hacd::HaF32 (0.0f);
	}

	hacd::HaI32 count = dgVertexListToIndexList(&tmpUV[0].m_x, sizeof (dgTriplex), sizeof (dgTriplex), 0, m_atribCount, uvIndices1, hacd::HaF32 (0.0f));
	for (hacd::HaI32 i = 0; i < count; i ++){
		uv1[i * stride + 0] = tmpUV[i].m_x;
		uv1[i * stride + 1] = tmpUV[i].m_y;
	}
	*uvCount1 = count;

	stride = hacd::HaI32 (uvStrideInByte0 /sizeof (hacd::HaF32));
	for (hacd::HaI32 i = 0; i < m_atribCount; i ++){
		tmpUV[i].m_x = uv0[i * stride + 0];
		tmpUV[i].m_y = uv0[i * stride + 1];
		tmpUV[i].m_z = hacd::HaF32 (0.0f);
	}
	count = dgVertexListToIndexList(&tmpUV[0].m_x, sizeof (dgTriplex), sizeof (dgTriplex), 0, m_atribCount, uvIndices0, hacd::HaF32 (0.0f));
	for (hacd::HaI32 i = 0; i < count; i ++){
		uv0[i * stride + 0] = tmpUV[i].m_x;
		uv0[i * stride + 1] = tmpUV[i].m_y;
	}
	*uvCount0 = count;

	HACD_FREE (tmpUV);
*/
}

dgMeshEffect::dgIndexArray* dgMeshEffect::MaterialGeometryBegin()
{
	hacd::HaI32 materials[256];
	hacd::HaI32 streamIndexMap[256];

	hacd::HaI32 count = 0;
	hacd::HaI32 materialCount = 0;
	
	dgIndexArray* const array = (dgIndexArray*) HACD_ALLOC (hacd::HaI32 (4 * sizeof (hacd::HaI32) * GetCount() + sizeof (dgIndexArray) + 2048));
	array->m_indexList = (hacd::HaI32*)&array[1];
	
	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);	
	memset(streamIndexMap, 0, sizeof (streamIndexMap));
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		if ((edge->m_incidentFace >= 0) && (edge->m_mark != mark)) {
			dgEdge* ptr = edge;
			ptr->m_mark = mark;
			hacd::HaI32 index0 = hacd::HaI32 (ptr->m_userData);

			ptr = ptr->m_next;
			ptr->m_mark = mark;
			hacd::HaI32 index1 = hacd::HaI32 (ptr->m_userData);

			ptr = ptr->m_next;
			do {
				ptr->m_mark = mark;

				array->m_indexList[count * 4 + 0] = index0;
				array->m_indexList[count * 4 + 1] = index1;
				array->m_indexList[count * 4 + 2] = hacd::HaI32 (ptr->m_userData);
				array->m_indexList[count * 4 + 3] = hacd::HaI32 (m_attib[hacd::HaI32 (edge->m_userData)].m_material);
				index1 = hacd::HaI32 (ptr->m_userData);

				hacd::HaI32 hashValue = array->m_indexList[count * 4 + 3] & 0xff;
				streamIndexMap[hashValue] ++;
				materials[hashValue] = array->m_indexList[count * 4 + 3];
				count ++;

				ptr = ptr->m_next;
			} while (ptr != edge);
		}
	}

	array->m_indexCount = count;
	array->m_materialCount = materialCount;

	count = 0;
	for (hacd::HaI32 i = 0; i < 256;i ++) {
		if (streamIndexMap[i]) {
			array->m_materials[count] = materials[i];
			array->m_materialsIndexCount[count] = streamIndexMap[i] * 3;
			count ++;
		}
	}

	array->m_materialCount = count;

	return array;
}

void dgMeshEffect::MaterialGeomteryEnd(dgIndexArray* const handle)
{
	HACD_FREE (handle);
}


hacd::HaI32 dgMeshEffect::GetFirstMaterial (dgIndexArray* const handle)
{
	return GetNextMaterial (handle, -1);
}

hacd::HaI32 dgMeshEffect::GetNextMaterial (dgIndexArray* const handle, hacd::HaI32 materialId)
{
	materialId ++;
	if(materialId >= handle->m_materialCount) {
		materialId = -1;
	}
	return materialId;
}

void dgMeshEffect::GetMaterialGetIndexStream (dgIndexArray* const handle, hacd::HaI32 materialHandle, hacd::HaI32* const indexArray)
{
	hacd::HaI32 index;
	hacd::HaI32 textureID;

	index = 0;
	textureID = handle->m_materials[materialHandle];
	for (hacd::HaI32 j = 0; j < handle->m_indexCount; j ++) {
		if (handle->m_indexList[j * 4 + 3] == textureID) {
			indexArray[index + 0] = handle->m_indexList[j * 4 + 0];
			indexArray[index + 1] = handle->m_indexList[j * 4 + 1];
			indexArray[index + 2] = handle->m_indexList[j * 4 + 2];

			index += 3;
		}
	}
}

void dgMeshEffect::GetMaterialGetIndexStreamShort (dgIndexArray* const handle, hacd::HaI32 materialHandle, hacd::HaI16* const indexArray)
{
	hacd::HaI32 index;
	hacd::HaI32 textureID;

	index = 0;
	textureID = handle->m_materials[materialHandle];
	for (hacd::HaI32 j = 0; j < handle->m_indexCount; j ++) {
		if (handle->m_indexList[j * 4 + 3] == textureID) {
			indexArray[index + 0] = (hacd::HaI16)handle->m_indexList[j * 4 + 0];
			indexArray[index + 1] = (hacd::HaI16)handle->m_indexList[j * 4 + 1];
			indexArray[index + 2] = (hacd::HaI16)handle->m_indexList[j * 4 + 2];
			index += 3;
		}
	}
}

/*
hacd::HaI32 dgMeshEffect::GetEffectiveVertexCount() const
{
	hacd::HaI32 mark;
	hacd::HaI32 count;

	count = 0;
	mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		dgEdge* vertex;
	
		vertex = &(*iter);
		if (vertex->m_mark != mark) {
			dgEdge* ptr;

			ptr = vertex;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_twin->m_next;
			} while (ptr != vertex);
			count ++;
		}
	}
	return count;
}
*/

dgConvexHull3d * dgMeshEffect::CreateConvexHull(hacd::HaF64 tolerance,hacd::HaI32 maxVertexCount) const
{
	dgConvexHull3d *ret = NULL;

	dgStack<dgBigVector> poolPtr(m_pointCount * 2); 
	dgBigVector* const pool = &poolPtr[0];

	hacd::HaI32 count = 0;
	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++)
	{
		dgEdge* const vertex = &(*iter);
		if (vertex->m_mark != mark) 
		{
			dgEdge* ptr = vertex;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_twin->m_next;
			} while (ptr != vertex);

			if (count < hacd::HaI32 (poolPtr.GetElementsCount())) 
			{
				const dgBigVector p = m_points[vertex->m_incidentVertex];
				pool[count] = p;
				count++;
			}
		}
	}

	ret = HACD_NEW(dgConvexHull3d)((const hacd::HaF64 *)pool,sizeof(dgBigVector),count,tolerance,maxVertexCount);

	return ret;
}


dgCollision* dgMeshEffect::CreateConvexCollision(hacd::HaF64 tolerance, hacd::HaI32 shapeID, const dgMatrix& srcMatrix) const
{
//	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED
	dgStack<dgVector> poolPtr (m_pointCount * 2); 
	dgVector* const pool = &poolPtr[0];

	dgBigVector minBox;
	dgBigVector maxBox;
	CalculateAABB (minBox, maxBox);
	dgVector com ((minBox + maxBox).Scale (hacd::HaF32 (0.5f)));

	hacd::HaI32 count = 0;
	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		dgEdge* const vertex = &(*iter);
		if (vertex->m_mark != mark) {
			dgEdge* ptr = vertex;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_twin->m_next;
			} while (ptr != vertex);

			if (count < hacd::HaI32 (poolPtr.GetElementsCount())) {
				const dgBigVector p = m_points[vertex->m_incidentVertex];
				pool[count] = dgVector (p) - com;
				count ++;
			}
		}
	}

	dgMatrix matrix (srcMatrix);
	matrix.m_posit += matrix.RotateVector(com);
	matrix.m_posit.m_w = hacd::HaF32 (1.0f);

	dgStack<hacd::HaI32> buffer(hacd::HaI32 (2 + 3 * count + sizeof (dgMatrix) / sizeof (hacd::HaI32)));  

	memset (&buffer[0], 0, size_t (buffer.GetSizeInBytes()));
	buffer[0] = m_convexHullCollision;
	buffer[1] = shapeID;
	for (hacd::HaI32 i = 0; i < count; i ++) {
		buffer[2 + i * 3 + 0] = hacd::HaI32 (dgCollision::Quantize (pool[i].m_x));
		buffer[2 + i * 3 + 1] = hacd::HaI32 (dgCollision::Quantize (pool[i].m_y));
		buffer[2 + i * 3 + 2] = hacd::HaI32 (dgCollision::Quantize (pool[i].m_z));
	}
	memcpy (&buffer[2 + count * 3], &matrix, sizeof (dgMatrix));
	hacd::HaU32 crc = dgCollision::MakeCRC(&buffer[0], buffer.GetSizeInBytes());

	dgCollisionConvexHull* collision = HACD_NEW(dgCollisionConvexHull) (crc, count, sizeof (dgVector), hacd::HaF32 (tolerance), &pool[0].m_x, matrix);
	if (!collision->GetVertexCount()) {
		collision->Release();
		collision = NULL;
	} else {
		collision->SetUserDataID (hacd::HaU32 (shapeID));
	}

	return collision;
#else
	return NULL;
#endif
}


/*
dgEdge* dgMeshEffect::InsertFaceVertex (dgEdge* const face, const dgVector& point)
{
	hacd::HaI32 v0;
	hacd::HaI32 v1;
	hacd::HaI32 v2;
	hacd::HaI32 vertexIndex;
	hacd::HaI32 attibuteIndex;
	hacd::HaF32 va;
	hacd::HaF32 vb;
	hacd::HaF32 vc;
	hacd::HaF32 den;
	hacd::HaF32 alpha0;
	hacd::HaF32 alpha1;
	hacd::HaF32 alpha2;
	hacd::HaF32 alpha3;
	hacd::HaF32 alpha4;
	hacd::HaF32 alpha5;
	hacd::HaF32 alpha6;
	dgVertexAtribute attribute;
	dgEdge* face0;
	dgEdge* face1;
	dgEdge* face2;
	dgEdge* const edge0;
	dgEdge* twin0;
	dgEdge* const edge1;
	dgEdge* twin1;
	dgEdge* const edge2;
	dgEdge* twin2;


	v0 = face->m_incidentVertex;
	v1 = face->m_next->m_incidentVertex;
	v2 = face->m_prev->m_incidentVertex;

	const dgVector& p0 = m_points[v0];
	const dgVector& p1 = m_points[v1];
	const dgVector& p2 = m_points[v2];

	dgVector p10 (p1 - p0);
	dgVector p20 (p2 - p0);
	dgVector p_p0 (point - p0);
	dgVector p_p1 (point - p1);
	dgVector p_p2 (point - p2);

	alpha1 = p10 % p_p0;
	alpha2 = p20 % p_p0;
	alpha3 = p10 % p_p1;
	alpha4 = p20 % p_p1;
	alpha5 = p10 % p_p2;
	alpha6 = p20 % p_p2;

	HACD_ASSERT (!((alpha1 <= hacd::HaF32 (0.0f)) && (alpha2 <= hacd::HaF32 (0.0f))));
	HACD_ASSERT (!((alpha6 >= hacd::HaF32 (0.0f)) && (alpha5 <= alpha6)));
	HACD_ASSERT (!((alpha3 >= hacd::HaF32 (0.0f)) && (alpha4 <= alpha3)));

	vc = alpha1 * alpha4 - alpha3 * alpha2;
	vb = alpha5 * alpha2 - alpha1 * alpha6;
	va = alpha3 * alpha6 - alpha5 * alpha4;

	HACD_ASSERT (!((vc <= hacd::HaF32 (0.0f)) && (alpha1 >= hacd::HaF32 (0.0f)) && (alpha3 <= hacd::HaF32 (0.0f))));
	HACD_ASSERT (!((vb <= hacd::HaF32 (0.0f)) && (alpha2 >= hacd::HaF32 (0.0f)) && (alpha6 <= hacd::HaF32 (0.0f))));
	HACD_ASSERT (!((va <= hacd::HaF32 (0.0f)) && ((alpha4 - alpha3) >= hacd::HaF32 (0.0f)) && ((alpha5 - alpha6) >= hacd::HaF32 (0.0f))));

	den = float(hacd::HaF32 (1.0f)) / (va + vb + vc);

	alpha0 = va * den;
	alpha1 = vb * den;
	alpha2 = vc * den;


	//dgVector p (p0.Scale (alpha0) + p1.Scale (alpha1) + p2.Scale (alpha2));
	//alpha3 *= 1;


	const dgVertexAtribute& attr0 = m_attib[face->m_userData];
	const dgVertexAtribute& attr1 = m_attib[face->m_next->m_userData];
	const dgVertexAtribute& attr2 = m_attib[face->m_prev->m_userData];
	dgVector normal (attr0.m_normal.m_x * alpha0 + attr1.m_normal.m_x * alpha1 + attr0.m_normal.m_x * alpha2,
					 attr0.m_normal.m_y * alpha0 + attr1.m_normal.m_y * alpha1 + attr0.m_normal.m_y * alpha2,
					 attr0.m_normal.m_z * alpha0 + attr1.m_normal.m_z * alpha1 + attr0.m_normal.m_z * alpha2, hacd::HaF32 (0.0f));
	normal = normal.Scale (dgRsqrt (normal % normal));

	attribute.m_vertex.m_x = point.m_x;
	attribute.m_vertex.m_y = point.m_y;
	attribute.m_vertex.m_z = point.m_z;
	attribute.m_normal.m_y = normal.m_y;
	attribute.m_normal.m_z = normal.m_z;
	attribute.m_normal.m_x = normal.m_x;
	attribute.m_normal.m_y = normal.m_y;
	attribute.m_normal.m_z = normal.m_z;
	attribute.m_u = attr0.m_u * alpha0 +  attr1.m_u * alpha1 + attr2.m_u * alpha2;
	attribute.m_v = attr0.m_v * alpha0 +  attr1.m_v * alpha1 + attr2.m_v * alpha2;
	HACD_ASSERT (attr0.m_material == attr1.m_material);
	HACD_ASSERT (attr0.m_material == attr2.m_material);
	AddVertex (&attribute.m_vertex.m_x, attr0.m_material);

	vertexIndex = m_pointCount - 1;
	attibuteIndex = m_atribCount - 1;

	face0 = face;
	face1 = face->m_next;
	face2 = face->m_prev;

	edge0 = AddHalfEdge(vertexIndex, v0);
	twin0 = AddHalfEdge(v0, vertexIndex);

	edge1 = AddHalfEdge(vertexIndex, v1);
	twin1 = AddHalfEdge(v1, vertexIndex);

	edge2 = AddHalfEdge(vertexIndex, v2);
	twin2 = AddHalfEdge(v2, vertexIndex);

	edge0->m_incidentFace = face->m_incidentFace;
	twin0->m_incidentFace = face->m_incidentFace; 

	edge1->m_incidentFace = face->m_incidentFace;
	twin1->m_incidentFace = face->m_incidentFace;

	edge2->m_incidentFace = face->m_incidentFace;
	twin2->m_incidentFace = face->m_incidentFace;

	edge0->m_userData = attibuteIndex;
	edge1->m_userData = attibuteIndex;
	edge2->m_userData = attibuteIndex;

	twin0->m_userData = face0->m_userData;
	edge1->m_userData = face1->m_userData;
	edge2->m_userData = face2->m_userData;

	edge0->m_twin = twin0;
	twin0->m_twin = edge0;

	edge1->m_twin = twin1;
	twin1->m_twin = edge1;

	edge2->m_twin = twin2;
	twin2->m_twin = edge2;

	edge0->m_next = face0;
	edge1->m_next = face1;
	edge2->m_next = face2;

	edge0->m_prev = twin1;
	edge1->m_prev = twin2;
	edge2->m_prev = twin0;

	twin0->m_next = edge2;
	twin1->m_next = edge0;
	twin2->m_next = edge1;

	twin0->m_prev = face2;
	twin1->m_prev = face0;
	twin2->m_prev = face1;

	face0->m_next = twin1;
	face1->m_next = twin2;
	face2->m_next = twin0;

	face0->m_prev = edge0;
	face1->m_prev = edge1;
	face2->m_prev = edge2;

	return edge0;
}


hacd::HaI32 dgMeshEffect::RayIntersection (hacd::HaF32& p0p1, const dgVector& p0, const dgVector& p1, hacd::HaF32& q0q1, const dgVector& q0, const dgVector& q1) const
{
	hacd::HaI32 ret;
	hacd::HaF64 a;
	hacd::HaF64 b;
	hacd::HaF64 c;
	hacd::HaF64 d;
	hacd::HaF64 e;
	hacd::HaF64 D;
	dgBigVector ray_p0 (p0);
	dgBigVector ray_p1 (p1);
	dgBigVector ray_q0 (q0);
	dgBigVector ray_q1 (q1);
	dgBigVector u (ray_p1 - ray_p0);
	dgBigVector v (ray_q1 - ray_q0);

	a = u % u;        // always >= 0
	b = u % v;
	c = v % v;        // always >= 0
	D = a*c - b*b;   // always >= 0

	ret = 0;
	if (D > hacd::HaF64 (1.0e-8f)) { // the lines are almost parallel
		hacd::HaF64 sN;
		hacd::HaF64 tN;
		hacd::HaF64 fracsN;
		hacd::HaF64 fractN;
		dgBigVector w (ray_p0 - ray_q0);

		ret = 1;

		d = u % w;
		e = v % w;
		sN = (b*e - c*d) / D;
		tN = (a*e - b*d) / D;
		fracsN = DG_QUANTIZE_TOLERANCE / sqrt (a);
		fractN = DG_QUANTIZE_TOLERANCE / sqrt (c);

		if (sN < -fracsN) {
			ret = 0;
		} else if (sN < fracsN) {
			sN = hacd::HaF64 (0.0f);
		}
		if (sN > (hacd::HaF64 (1.0f) + fracsN)) {
			ret = 0;
		} else if (sN > (hacd::HaF64 (1.0f) - fracsN)) {
			sN = hacd::HaF64 (1.0f);
		}

		if (tN < -fractN) {
			ret = 0;
		} else if (tN < fractN) {
			tN = hacd::HaF64 (0.0f);
		}
		if (tN > (hacd::HaF64 (1.0f) + fractN)) {
			ret = 0;
		} else if (tN > (hacd::HaF64 (1.0f) - fractN)) {
			tN = hacd::HaF64 (1.0f);
		}

		if (ret) {
			dgBigVector p (ray_p0 + u.Scale (sN));
			dgBigVector q (ray_q0 + v.Scale (tN));
			dgBigVector dist (p - q);

			d = dist % dist;
			if (d > (hacd::HaF32 (16.0f) * DG_QUANTIZE_TOLERANCE * DG_QUANTIZE_TOLERANCE)) {
				ret = 0;
			}
		}

		p0p1 = hacd::HaF32 (sN);
		q0q1 = hacd::HaF32 (tN);
	}
	return ret;
}
*/




void dgMeshEffect::TransformMesh (const dgMatrix& matrix)
{
	dgMatrix normalMatrix (matrix);
	normalMatrix.m_posit = dgVector (hacd::HaF32 (0.0f), hacd::HaF32 (0.0f), hacd::HaF32 (0.0f), hacd::HaF32 (1.0f));

	matrix.TransformTriplex (&m_points->m_x, sizeof (dgBigVector), &m_points->m_x, sizeof (dgBigVector), m_pointCount);
	matrix.TransformTriplex (&m_attib[0].m_vertex.m_x, sizeof (dgVertexAtribute), &m_attib[0].m_vertex.m_x, sizeof (dgVertexAtribute), m_atribCount);
	normalMatrix.TransformTriplex (&m_attib[0].m_normal_x, sizeof (dgVertexAtribute), &m_attib[0].m_normal_x, sizeof (dgVertexAtribute), m_atribCount);
}


dgMeshEffect::dgVertexAtribute dgMeshEffect::InterpolateEdge (dgEdge* const edge, hacd::HaF64 param) const
{
	dgVertexAtribute attrEdge;
	hacd::HaF64 t1 = param;
	hacd::HaF64 t0 = hacd::HaF64 (1.0f) - t1;
	HACD_ASSERT (t1 >= hacd::HaF64(0.0f));
	HACD_ASSERT (t1 <= hacd::HaF64(1.0f));

	const dgVertexAtribute& attrEdge0 = m_attib[edge->m_userData];
	const dgVertexAtribute& attrEdge1 = m_attib[edge->m_next->m_userData];

	attrEdge.m_vertex.m_x = attrEdge0.m_vertex.m_x * t0 + attrEdge1.m_vertex.m_x * t1;
	attrEdge.m_vertex.m_y = attrEdge0.m_vertex.m_y * t0 + attrEdge1.m_vertex.m_y * t1;
	attrEdge.m_vertex.m_z = attrEdge0.m_vertex.m_z * t0 + attrEdge1.m_vertex.m_z * t1;
	attrEdge.m_vertex.m_w = hacd::HaF32(0.0f);
	attrEdge.m_normal_x = attrEdge0.m_normal_x * t0 +  attrEdge1.m_normal_x * t1; 
	attrEdge.m_normal_y = attrEdge0.m_normal_y * t0 +  attrEdge1.m_normal_y * t1; 
	attrEdge.m_normal_z = attrEdge0.m_normal_z * t0 +  attrEdge1.m_normal_z * t1; 
	attrEdge.m_u0 = attrEdge0.m_u0 * t0 +  attrEdge1.m_u0 * t1;
	attrEdge.m_v0 = attrEdge0.m_v0 * t0 +  attrEdge1.m_v0 * t1;
	attrEdge.m_u1 = attrEdge0.m_u1 * t0 +  attrEdge1.m_u1 * t1;
	attrEdge.m_v1 = attrEdge0.m_v1 * t0 +  attrEdge1.m_v1 * t1;
	attrEdge.m_material = attrEdge0.m_material;
	return attrEdge;
}

bool dgMeshEffect::Sanity () const
{
	HACD_ASSERT (0);
return false;
/*
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		const dgEdge* const edge = &iter.GetNode()->GetInfo();
		if (edge->m_incidentFace > 0) {
			const dgVertexAtribute& attrEdge0 = m_attib[edge->m_userData];
			dgVector p0 (m_points[edge->m_incidentVertex]);
			dgVector q0 (attrEdge0.m_vertex);
			dgVector delta0 (p0 - q0);
			hacd::HaF32 error0 = delta0 % delta0;
			if (error0 > hacd::HaF32 (1.0e-15f)) {
				return false;
			}

			const dgVertexAtribute& attrEdge1 = m_attib[edge->m_next->m_userData];
			dgVector p1 (m_points[edge->m_next->m_incidentVertex]);
			dgVector q1 (attrEdge1.m_vertex);
			dgVector delta1 (p1 - q1);
			hacd::HaF32 error1 = delta1 % delta1;
			if (error1 > hacd::HaF32 (1.0e-15f)) {
				return false;
			}
		}
	}
	return true;
*/
}


dgEdge* dgMeshEffect::InsertEdgeVertex (dgEdge* const edge, hacd::HaF64 param)
{

	dgEdge* const twin = edge->m_twin;
	dgVertexAtribute attrEdge (InterpolateEdge (edge, param));
	dgVertexAtribute attrTwin (InterpolateEdge (twin, hacd::HaF32 (1.0f) - param));

	attrTwin.m_vertex = attrEdge.m_vertex;
	AddPoint(&attrEdge.m_vertex.m_x, dgFastInt (attrEdge.m_material));
	AddAtribute (attrTwin);

	hacd::HaI32 edgeAttrV0 = hacd::HaI32 (edge->m_userData);
	hacd::HaI32 twinAttrV0 = hacd::HaI32 (twin->m_userData);

	dgEdge* const faceA0 = edge->m_next;
	dgEdge* const faceA1 = edge->m_prev;
	dgEdge* const faceB0 = twin->m_next;
	dgEdge* const faceB1 = twin->m_prev;

//	SpliteEdgeAndTriangulate (m_pointCount - 1, edge);
	SpliteEdge (m_pointCount - 1, edge);

	faceA0->m_prev->m_userData = hacd::HaU64 (m_atribCount - 2);
	faceA1->m_next->m_userData = hacd::HaU64 (edgeAttrV0);

	faceB0->m_prev->m_userData = hacd::HaU64 (m_atribCount - 1);
	faceB1->m_next->m_userData = hacd::HaU64 (twinAttrV0);

	return faceA1->m_next;
}



dgMeshEffect::dgVertexAtribute dgMeshEffect::InterpolateVertex (const dgBigVector& srcPoint, dgEdge* const face) const
{
	//this should use Googol extended precision floats, because some face coming from Voronoi decomposition and booleans
	//clipping has extreme aspect ratios, for now just use float64
	const dgBigVector point (srcPoint);

	dgVertexAtribute attribute;
	memset (&attribute, 0, sizeof (attribute));
	hacd::HaF64 tol = hacd::HaF32 (1.0e-4f);
	for (hacd::HaI32 i = 0; i < 4; i ++) {
		dgEdge* ptr = face;
		dgEdge* const edge0 = ptr;
		dgBigVector q0 (m_points[ptr->m_incidentVertex]);

		ptr = ptr->m_next;
		const dgEdge* edge1 = ptr;
		dgBigVector q1 (m_points[ptr->m_incidentVertex]);

		ptr = ptr->m_next;
		const dgEdge* edge2 = ptr;
		do {
			const dgBigVector q2 (m_points[ptr->m_incidentVertex]);

			dgBigVector p10 (q1 - q0);
			dgBigVector p20 (q2 - q0);
#ifdef	_DEBUG		
			hacd::HaF64 dot = p20 % p10;
			hacd::HaF64 mag1 = p10 % p10;
			hacd::HaF64 mag2 = p20 % p20;
			hacd::HaF64 collinear = dot * dot - mag2 * mag1;
			HACD_ASSERT (fabs (collinear) > hacd::HaF64 (1.0e-8f));
#endif

			dgBigVector p_p0 (point - q0);
			dgBigVector p_p1 (point - q1);
			dgBigVector p_p2 (point - q2);

			hacd::HaF64 alpha1 = p10 % p_p0;
			hacd::HaF64 alpha2 = p20 % p_p0;
			hacd::HaF64 alpha3 = p10 % p_p1;
			hacd::HaF64 alpha4 = p20 % p_p1;
			hacd::HaF64 alpha5 = p10 % p_p2;
			hacd::HaF64 alpha6 = p20 % p_p2;

			hacd::HaF64 vc = alpha1 * alpha4 - alpha3 * alpha2;
			hacd::HaF64 vb = alpha5 * alpha2 - alpha1 * alpha6;
			hacd::HaF64 va = alpha3 * alpha6 - alpha5 * alpha4;
			hacd::HaF64 den = va + vb + vc;
			hacd::HaF64 minError = den * (-tol);
			hacd::HaF64 maxError = den * (hacd::HaF32 (1.0f) + tol);
			if ((va > minError) && (vb > minError) && (vc > minError) && (va < maxError) && (vb < maxError) && (vc < maxError)) {
				edge2 = ptr;

				den = hacd::HaF64 (1.0f) / (va + vb + vc);

				hacd::HaF64 alpha0 = hacd::HaF32 (va * den);
				hacd::HaF64 alpha1 = hacd::HaF32 (vb * den);
				hacd::HaF64 alpha2 = hacd::HaF32 (vc * den);

				const dgVertexAtribute& attr0 = m_attib[edge0->m_userData];
				const dgVertexAtribute& attr1 = m_attib[edge1->m_userData];
				const dgVertexAtribute& attr2 = m_attib[edge2->m_userData];
				dgBigVector normal (attr0.m_normal_x * alpha0 + attr1.m_normal_x * alpha1 + attr2.m_normal_x * alpha2,
									attr0.m_normal_y * alpha0 + attr1.m_normal_y * alpha1 + attr2.m_normal_y * alpha2,
									attr0.m_normal_z * alpha0 + attr1.m_normal_z * alpha1 + attr2.m_normal_z * alpha2, hacd::HaF32 (0.0f));
				normal = normal.Scale (hacd::HaF64 (1.0f) / sqrt (normal % normal));

	#ifdef _DEBUG
				dgBigVector testPoint (attr0.m_vertex.m_x * alpha0 + attr1.m_vertex.m_x * alpha1 + attr2.m_vertex.m_x * alpha2,
									   attr0.m_vertex.m_y * alpha0 + attr1.m_vertex.m_y * alpha1 + attr2.m_vertex.m_y * alpha2,
									   attr0.m_vertex.m_z * alpha0 + attr1.m_vertex.m_z * alpha1 + attr2.m_vertex.m_z * alpha2, hacd::HaF32 (0.0f));
				HACD_ASSERT (fabs (testPoint.m_x - point.m_x) < hacd::HaF32 (1.0e-2f));
				HACD_ASSERT (fabs (testPoint.m_y - point.m_y) < hacd::HaF32 (1.0e-2f));
				HACD_ASSERT (fabs (testPoint.m_z - point.m_z) < hacd::HaF32 (1.0e-2f));
	#endif


				attribute.m_vertex.m_x = point.m_x;
				attribute.m_vertex.m_y = point.m_y;
				attribute.m_vertex.m_z = point.m_z;
				attribute.m_vertex.m_w = point.m_w;
				attribute.m_normal_x = normal.m_x;
				attribute.m_normal_y = normal.m_y;
				attribute.m_normal_z = normal.m_z;
				attribute.m_u0 = attr0.m_u0 * alpha0 +  attr1.m_u0 * alpha1 + attr2.m_u0 * alpha2;
				attribute.m_v0 = attr0.m_v0 * alpha0 +  attr1.m_v0 * alpha1 + attr2.m_v0 * alpha2;
				attribute.m_u1 = attr0.m_u1 * alpha0 +  attr1.m_u1 * alpha1 + attr2.m_u1 * alpha2;
				attribute.m_v1 = attr0.m_v1 * alpha0 +  attr1.m_v1 * alpha1 + attr2.m_v1 * alpha2;

				attribute.m_material = attr0.m_material;
				HACD_ASSERT (attr0.m_material == attr1.m_material);
				HACD_ASSERT (attr0.m_material == attr2.m_material);
				return attribute; 
			}
			
			q1 = q2;
			edge1 = ptr;

			ptr = ptr->m_next;
		} while (ptr != face);
		tol *= hacd::HaF64 (2.0f);
	}
	// this should never happens
	HACD_ASSERT (0);
	return attribute;
}




void dgMeshEffect::MergeFaces (const dgMeshEffect* const source)
{
	hacd::HaI32 mark = source->IncLRU();
	dgPolyhedra::Iterator iter (*source);
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		if ((edge->m_incidentFace > 0) && (edge->m_mark < mark)) {
			dgVertexAtribute face[DG_MESH_EFFECT_POINT_SPLITED];

			hacd::HaI32 count = 0;
			dgEdge* ptr = edge;
			do {
				ptr->m_mark = mark;
				face[count] = source->m_attib[ptr->m_userData];
				count ++;
				HACD_ASSERT (count < hacd::HaI32 (sizeof (face) / sizeof (face[0])));
				ptr = ptr->m_next;
			} while (ptr != edge);
			AddPolygon(count, &face[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (face[0].m_material));
		}
	}
}


void dgMeshEffect::ReverseMergeFaces (dgMeshEffect* const source)
{
	hacd::HaI32 mark = source->IncLRU();
	dgPolyhedra::Iterator iter (*source);
	for(iter.Begin(); iter; iter ++){
		dgEdge* const edge = &(*iter);
		if ((edge->m_incidentFace > 0) && (edge->m_mark < mark)) {
			dgVertexAtribute face[DG_MESH_EFFECT_POINT_SPLITED];

			hacd::HaI32 count = 0;
			dgEdge* ptr = edge;
			do {
				ptr->m_mark = mark;
				face[count] = source->m_attib[ptr->m_userData];
				face[count].m_normal_x *= hacd::HaF32 (-1.0f);
				face[count].m_normal_y *= hacd::HaF32 (-1.0f);
				face[count].m_normal_z *= hacd::HaF32 (-1.0f);
				count ++;
				HACD_ASSERT (count < hacd::HaI32 (sizeof (face) / sizeof (face[0])));
				ptr = ptr->m_prev;
			} while (ptr != edge);
			AddPolygon(count, &face[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (face[0].m_material));
		}
	}
}



void dgMeshEffect::FilterCoplanarFaces (const dgMeshEffect* const coplanarFaces, hacd::HaF32 sign)
{
	const hacd::HaF64 tol = hacd::HaF64 (1.0e-5f);
	const hacd::HaF64 tol2 = tol * tol;

	hacd::HaI32 mark = IncLRU();
	Iterator iter (*this);
	for (iter.Begin(); iter; ) {
		dgEdge* const face = &(*iter);
		iter ++;
		if ((face->m_mark != mark) && (face->m_incidentFace > 0)) {
			dgEdge* ptr = face;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face);

			dgBigVector normal (FaceNormal(face, &m_points[0].m_x, sizeof (dgBigVector)));
			normal = normal.Scale (sign);
			dgBigVector origin (m_points[face->m_incidentVertex]);

			hacd::HaF64 error2 = (normal % normal) * tol2;
			hacd::HaI32 capMark = coplanarFaces->IncLRU();

			Iterator capIter (*coplanarFaces);
			for (capIter.Begin (); capIter; capIter ++) {
				dgEdge* const capFace = &(*capIter);
				if ((capFace->m_mark != capMark) && (capFace->m_incidentFace > 0)) {
					dgEdge* ptr = capFace;
					do {
						ptr->m_mark = capMark;
						ptr = ptr->m_next;
					} while (ptr != capFace);

					dgBigVector capNormal (coplanarFaces->FaceNormal(capFace, &coplanarFaces->m_points[0].m_x, sizeof (dgBigVector)));

					if ((capNormal % normal) > hacd::HaF64 (0.0f)) {
						dgBigVector capOrigin (coplanarFaces->m_points[capFace->m_incidentVertex]);
						hacd::HaF64 dist = normal % (capOrigin - origin);
						if ((dist * dist) < error2) {
							DeleteFace(face);
							iter.Begin();
							break;
						}
					}
				}
			}
		}
	}
}


dgMeshEffect* dgMeshEffect::Union (const dgMatrix& matrix, const dgMeshEffect* const clipMesh) const
{
	dgMeshEffect clipper (*clipMesh);
	clipper.TransformMesh (matrix);

	DG_MESG_EFFECT_BOOLEAN_INIT();

	ClipMesh (&clipper, &leftMeshSource, &rightMeshSource, &sourceCoplanar);
	clipper.ClipMesh (this, &leftMeshClipper, &rightMeshClipper, &clipperCoplanar);
	if (rightMeshSource || rightMeshClipper) {
			result = HACD_NEW(dgMeshEffect)(true);
			result->BeginPolygon();

		if (rightMeshSource) {
			result->MergeFaces(rightMeshSource);
	}

		if (rightMeshClipper) {
			result->MergeFaces(rightMeshClipper);
	}

		if (clipperCoplanar && sourceCoplanar) {
			//sourceCoplanar->FilterCoplanarFaces (clipperCoplanar, hacd::HaF32 (-1.0f));
			//result->MergeFaces(sourceCoplanar);
			clipperCoplanar->FilterCoplanarFaces (sourceCoplanar, hacd::HaF32 (-1.0f));
			result->MergeFaces(clipperCoplanar);
	}

		result->EndPolygon(hacd::HaF64 (1.0e-5f));
		if (!result->GetCount()) {
			result->Release();
			result = NULL;
	}
	}

	DG_MESG_EFFECT_BOOLEAN_FINISH();
	return result;
}

dgMeshEffect* dgMeshEffect::Intersection (const dgMatrix& matrix, const dgMeshEffect* const clipMesh) const
{
	dgMeshEffect clipper (*clipMesh);
	clipper.TransformMesh (matrix);

	DG_MESG_EFFECT_BOOLEAN_INIT();

	ClipMesh (&clipper, &leftMeshSource, &rightMeshSource, &sourceCoplanar);
	clipper.ClipMesh (this, &leftMeshClipper, &rightMeshClipper, &clipperCoplanar);
	if (leftMeshSource || leftMeshClipper) {
			result = HACD_NEW(dgMeshEffect)(true);
		result->BeginPolygon();

		if (leftMeshSource) {
			result->MergeFaces(leftMeshSource);
	}

		if (leftMeshClipper) {
			result->MergeFaces(leftMeshClipper);
	}

		if (clipperCoplanar && sourceCoplanar) {
			sourceCoplanar->FilterCoplanarFaces (clipperCoplanar, hacd::HaF32 (-1.0f));
			result->MergeFaces(sourceCoplanar);
	}

		result->EndPolygon(hacd::HaF64 (1.0e-5f));
		if (!result->GetCount()) {
			result->Release();
			result = NULL;
	}
	}

	DG_MESG_EFFECT_BOOLEAN_FINISH();
	return result;
}


														
dgMeshEffect* dgMeshEffect::Difference (const dgMatrix& matrix, const dgMeshEffect* const clipMesh) const
{
	dgMeshEffect clipper (*clipMesh);
	clipper.TransformMesh (matrix);

	DG_MESG_EFFECT_BOOLEAN_INIT();

	ClipMesh (&clipper, &leftMeshSource, &rightMeshSource, &sourceCoplanar);
	if (rightMeshSource) {
			result = HACD_NEW(dgMeshEffect)(true);
			result->BeginPolygon();
		if (rightMeshSource) {
			result->MergeFaces(rightMeshSource);
		}

		clipper.ClipMesh (this, &leftMeshClipper, &rightMeshClipper, &clipperCoplanar);
		if (leftMeshClipper || clipperCoplanar) {
			if (leftMeshClipper) {
				result->ReverseMergeFaces(leftMeshClipper);
	}
			if (clipperCoplanar && sourceCoplanar) {
				HACD_ASSERT (sourceCoplanar);
				clipperCoplanar->FilterCoplanarFaces (sourceCoplanar, hacd::HaF32 (1.0f));
				result->ReverseMergeFaces(clipperCoplanar);
	}
	}

		result->EndPolygon(hacd::HaF64 (1.0e-5f));
		if (!result->GetCount()) {
			result->Release();
			result = NULL;
	}
	}

	DG_MESG_EFFECT_BOOLEAN_FINISH();
	return result;
}




void dgMeshEffect::ClipMesh (const dgMatrix& matrix, const dgMeshEffect* const clipMesh, dgMeshEffect** const back, dgMeshEffect** const front) const
{
	HACD_ASSERT (0);
/*
	dgMeshEffect clipper (*clipMesh);
	clipper.TransformMesh (matrix);

	dgMeshEffect* backMeshSource = NULL;
	dgMeshEffect* frontMeshSource = NULL;
	dgMeshEffect* backMeshClipper = NULL;
	dgMeshEffect* frontMeshClipper = NULL;

	ClipMesh (&clipper, &backMeshSource, &frontMeshSource);
	if (backMeshSource && frontMeshSource) {
		clipper.ClipMesh (this, &backMeshClipper, &frontMeshClipper);
		if (backMeshSource && frontMeshSource) {

			dgMeshEffect* backMesh;
			dgMeshEffect* frontMesh;

			backMesh = new (GetAllocator()) dgMeshEffect (GetAllocator(), true);
			frontMesh = new (GetAllocator()) dgMeshEffect (GetAllocator(), true);

			backMesh->BeginPolygon();
			frontMesh->BeginPolygon();

			backMesh->MergeFaces(backMeshSource);
			backMesh->MergeFaces(backMeshClipper);

			frontMesh->MergeFaces(frontMeshSource);
			frontMesh->ReverseMergeFaces(backMeshClipper);

			backMesh->EndPolygon(hacd::HaF64 (1.0e-5f));
			frontMesh->EndPolygon(hacd::HaF64 (1.0e-5f));

			*back = backMesh;
			*front = frontMesh;
		}
	}

	if (backMeshClipper) {
		delete backMeshClipper;
	}

	if (frontMeshClipper) {
		delete frontMeshClipper;
	}

	if (backMeshSource) {
		delete backMeshSource;
	}

	if (frontMeshSource) {
		delete frontMeshSource;
	}
*/
}







dgMeshEffectSolidTree* dgMeshEffect::CreateSolidTree() const
{
	dgMeshEffectSolidTree* tree = NULL;
	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED
	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if ((face->m_incidentFace > 0) && (face->m_mark != mark)) {
			dgEdge* ptr = face;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face);

			if (ptr->m_next->m_next->m_next == ptr) {
				if (!tree) {
					dgBigVector normal (FaceNormal (face, &m_points[0][0], sizeof (dgBigVector)));
					hacd::HaF64 mag2 = normal % normal;
					if (mag2 > hacd::HaF32 (1.0e-10f)) {
						tree = HACD_NEW(dgMeshEffectSolidTree)(*this, face);
					}
				} else {
					tree->AddFace (*this, face);
				}
			} else {
				dgMeshEffect flatFace (true);
				hacd::HaI32 count = 0;
				dgVertexAtribute points[256];

				flatFace.BeginPolygon();
				dgEdge* ptr = face;
				do {
					points[count] = m_attib[ptr->m_userData];
					count ++;
					ptr = ptr->m_next;
				} while (ptr != face);
				flatFace.AddPolygon(count, &points[0].m_vertex.m_x, sizeof (dgVertexAtribute), 0);
				flatFace.EndPolygon(hacd::HaF64 (1.0e-5f));

				hacd::HaI32 flatMark = flatFace.IncLRU();
				dgPolyhedra::Iterator flatIter (flatFace);
				for (flatIter.Begin(); flatIter; flatIter ++){
					dgEdge* const face = &(*flatIter);
					if ((face->m_incidentFace > 0) && (face->m_mark != flatMark)) {
						dgEdge* ptr = face;
						do {
							ptr->m_mark = flatMark;
							ptr = ptr->m_next;
						} while (ptr != face);

						if (!tree) {
							dgBigVector normal (flatFace.FaceNormal (face, &flatFace.m_points[0][0], sizeof (dgBigVector)));
							hacd::HaF64 mag2 = normal % normal;
							if (mag2 > hacd::HaF32 (1.0e-10f)) {
								tree = HACD_NEW(dgMeshEffectSolidTree)(face);
							}
						} else {
							tree->AddFace (flatFace, face);
						}
					}
				}
			}
		}
	}
	HACD_ASSERT (tree);
#endif
	return tree;
}

void dgMeshEffect::DestroySolidTree (dgMeshEffectSolidTree* const tree)
{
	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED

	delete tree;
#endif
}



void dgMeshEffect::ClipMesh (const dgMeshEffect* const clipMesh, dgMeshEffect** const left, dgMeshEffect** const right, dgMeshEffect** const coplanar) const
{
	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED
	const dgMeshEffectSolidTree* const clipper = clipMesh->CreateSolidTree();
	HACD_ASSERT (clipper);
	ClipMesh (clipper, left, right, coplanar);
	delete clipper;
#endif
}


bool dgMeshEffect::CheckIntersection (const dgMeshEffectSolidTree* const solidTree, hacd::HaF64 scale) const
{
HACD_ASSERT (0);
return false;
/*

	if (solidTree) {
		hacd::HaI32 mark;
		hacd::HaI32 count;
		dgVector center (hacd::HaF32 (0.0f), hacd::HaF32 (0.0f), hacd::HaF32 (0.0f), hacd::HaF32 (0.0f));

		count = 0;
		mark = IncLRU();
		dgPolyhedra::Iterator iter (*this);
		for (iter.Begin(); iter; iter ++){
			dgEdge* face;
			face = &(*iter);
			if (face->m_mark != mark) {
				dgEdge* ptr;
				ptr = face;
				do {
					ptr->m_mark = mark;
					ptr = ptr->m_twin->m_next;
				} while (ptr != face);
				count ++;
				center += m_points[face->m_incidentVertex];
			}
		}
		center = center.Scale (hacd::HaF32 (1.0f) / hacd::HaF32(count));

		dgMatrix matrix (dgGetIdentityMatrix());
		matrix[0][0] = scale;
		matrix[1][1] = scale;
		matrix[2][2] = scale;
		matrix.m_posit = center - matrix.RotateVector(center);
		matrix[3][3] = hacd::HaF32 (1.0f);

		mark = IncLRU();
		for (iter.Begin(); iter; iter ++){
			dgEdge* face;
			face = &(*iter);
			if (face->m_incidentFace > 0) {
				if (face->m_mark != mark) {

					hacd::HaI32 stack;	
					hacd::HaI32 frontCount;
					hacd::HaI32 backCount;
					dgEdge* ptr;
					dgMeshTreeCSGFace* meshFace;
					dgMeshTreeCSGPointsPool points;
					dgMeshTreeCSGFace* faceOnStack[DG_MESH_EFFECT_BOLLEAN_STACK];
					const dgMeshEffectSolidTree* stackPool[DG_MESH_EFFECT_BOLLEAN_STACK];

					backCount = 0;
					frontCount = 0;
					meshFace = new (GetAllocator()) dgMeshTreeCSGFace(GetAllocator());
					ptr = face;
					do {
						hacd::HaI32 index;
						index = points.AddPoint (matrix.TransformVector(m_points[ptr->m_incidentVertex]));
						meshFace->AddPoint (index);

						ptr->m_mark = mark;
						ptr = ptr->m_next;
					} while (ptr != face);


					stack = 1;
					stackPool[0] = solidTree;
					faceOnStack[0] = meshFace;
					meshFace->AddRef();

					while (stack) {
						dgMeshTreeCSGFace* rootFace; 
						dgMeshTreeCSGFace* backFace; 
						dgMeshTreeCSGFace* frontFace;
						const dgMeshEffectSolidTree* root;

						stack --;
						root = stackPool[stack];
						rootFace = faceOnStack[stack];

						ClipFace (root->m_plane, rootFace, &backFace, &frontFace, points);
						rootFace->Release();

						if (frontFace) {
							HACD_ASSERT (frontFace->CheckConvex(this, face, points));

							if (root->m_front) {
								stackPool[stack] = root->m_front;
								faceOnStack[stack] = frontFace;
								stack ++;
								HACD_ASSERT (stack < sizeof (stackPool) / sizeof (stackPool[0]));
							} else {
								frontFace->Release();
								frontCount ++;
							}
						}

						if (backFace) {
							HACD_ASSERT (backFace->CheckConvex(this, face, points));
							if (root->m_back) {
								stackPool[stack] = root->m_back;
								faceOnStack[stack] = backFace;
								stack ++;
								HACD_ASSERT (stack < sizeof (stackPool) / sizeof (stackPool[0]));
							} else {
								backFace->Release();
								backCount ++;
							}
						}
					}

					meshFace->Release();
					if (backCount) {
						return true;
					}
				}
			}
		}
	}
	return false;
*/
}



void dgMeshEffect::PlaneClipMesh (const dgMeshEffect* planeMesh, dgMeshEffect** back, dgMeshEffect** front) const
{
HACD_ASSERT (0);
/*

	dgEdge* face;
	dgMeshEffect* backMesh;
	dgMeshEffect* frontMesh;

	HACD_ASSERT (planeMesh->m_isFlagFace);
	face = &planeMesh->GetRoot()->GetInfo();
	if (face->m_incidentFace < 0) {
		face = face->m_twin;
	}
	HACD_ASSERT (face->m_incidentFace > 0);
	dgVector normal (planeMesh->FaceNormal (face, &planeMesh->m_points[0][0], sizeof (dgVector)));
	normal = normal.Scale (dgRsqrt (normal % normal));
	const dgVector& point = planeMesh->m_points[face->m_incidentVertex];
	dgPlane plane (normal, -(point % normal));

	dgMeshEffect tmp (*this);
	tmp.PlaneClipMesh (plane, left, right);

//	HACD_ASSERT (tmp.CheckSingleMesh());

	backMesh = *left;
	frontMesh = *right;

	if (backMesh && frontMesh) {
		HACD_ASSERT (backMesh->GetCount());
		HACD_ASSERT (frontMesh->GetCount());
		if (!(backMesh->PlaneApplyCap (planeMesh, plane) && frontMesh->PlaneApplyCap (planeMesh, plane.Scale (hacd::HaF32 (-1.0f))))) {
			backMesh->Release();
			frontMesh->Release();
			*left = NULL;
			*right = NULL;
		} else {
			backMesh->Triangulate ();
			frontMesh->Triangulate ();
//			HACD_ASSERT (frontMesh->CheckSingleMesh());
//			HACD_ASSERT (backMesh->CheckSingleMesh());
		}
	}
*/
}


void dgMeshEffect::PlaneClipMesh (const dgMatrix& planeMatrix, const dgMatrix& planeTextMatrix, hacd::HaI32 planeMaterial, dgMeshEffect** const left, dgMeshEffect** const right) const 
{
	*left = NULL;
	*right = NULL;

	dgTree<dgFlatClipEdgeAttr, dgEdge*> leftFilter;
	dgTree<dgFlatClipEdgeAttr, dgEdge*> rightFilter;
	dgStack<hacd::HaI8> vertexSidePool(GetCount() * 2 + 256);
	hacd::HaI8* const vertexSide = &vertexSidePool[0];

	dgBigPlane plane (planeMatrix.m_front, - (planeMatrix.m_front % planeMatrix.m_posit));
	plane = plane.Scale (hacd::HaF64 (1.0) / sqrt (plane % plane));

	dgMeshEffect mesh (*this);

	hacd::HaI32 backCount = 0;
	hacd::HaI32 frontCount = 0;

	
	dgPolyhedra::Iterator iter (mesh);
	hacd::HaI32 mark = mesh.IncLRU();
	for (iter.Begin(); iter; iter ++){
		dgEdge* const vertex = &(*iter);
		if (vertex->m_mark != mark) {
			dgEdge* ptr = vertex;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_twin->m_next;
			} while (ptr != vertex);

			hacd::HaF64 test = plane.Evalue(mesh.m_points[vertex->m_incidentVertex]);
			if (test >= hacd::HaF32 (1.0e-3f)) {
				frontCount ++;
				vertexSide[vertex->m_incidentVertex] = 1;
			} else if (test <= hacd::HaF32 (-1.0e-3f)) {
				backCount ++;
				vertexSide[vertex->m_incidentVertex] = -2;
			} else {
				vertexSide[vertex->m_incidentVertex] = 0;
			}
			mesh.m_points[vertex->m_incidentVertex].m_w = test;
		}
	}

	if ((frontCount == 0) || (backCount == 0)) {
		return;
	}

	mark = mesh.IncLRU();
	for (iter.Begin(); iter; ){
		dgEdge* const edge = &(*iter);

		iter ++;
		if (&(*iter) == edge->m_twin) {
			iter ++;
		}

		if (edge->m_mark != mark) {
			edge->m_mark = mark;
			edge->m_twin->m_mark = mark;
			if (vertexSide[edge->m_incidentVertex] * vertexSide[edge->m_twin->m_incidentVertex] < 0) {

				hacd::HaF64 test0 = mesh.m_points[edge->m_incidentVertex].m_w;

				dgBigVector dp (mesh.m_points[edge->m_twin->m_incidentVertex] - mesh.m_points[edge->m_incidentVertex]);
				hacd::HaF64 param = -test0 / (plane % dp);

				dgEdge* const ptr = mesh.InsertEdgeVertex (edge, param);
				ptr->m_mark = mark;
				ptr->m_next->m_mark = mark;
				ptr->m_twin->m_mark = mark;
				ptr->m_twin->m_prev->m_mark = mark;

				vertexSide[mesh.m_pointCount - 1] = 0;
			}
		}
	}

	mark = mesh.IncLRU();
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);


		if ((face->m_incidentFace > 0) && (face->m_mark != mark) && (vertexSide[face->m_incidentVertex] == 0) && (vertexSide[face->m_next->m_incidentVertex] < 0)) {
			dgEdge* ptr = face;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face);

			hacd::HaI32 side = 0;
			ptr = face->m_next;
			do {
				side |= vertexSide[ptr->m_incidentVertex];
				if (vertexSide[ptr->m_incidentVertex] == 0) {
					HACD_ASSERT (side != -1);
					HACD_ASSERT (side <= 0);
					if (side < 0) {
						if (ptr->m_next != face) {
							dgEdge* const back = mesh.AddHalfEdge(ptr->m_incidentVertex, face->m_incidentVertex);
							dgEdge* const front = mesh.AddHalfEdge(face->m_incidentVertex, ptr->m_incidentVertex);
							HACD_ASSERT (back);
							HACD_ASSERT (front);

							back->m_mark = mark;
							front->m_mark = mark;

							back->m_incidentFace = face->m_incidentFace;
							front->m_incidentFace = face->m_incidentFace;

							back->m_userData = ptr->m_userData;
							front->m_userData = face->m_userData;

							back->m_twin = front;
							front->m_twin = back;

							back->m_next = face;
							front->m_next = ptr;

							back->m_prev = ptr->m_prev;
							front->m_prev = face->m_prev;

							ptr->m_prev->m_next = back;
							ptr->m_prev = front;

							face->m_prev->m_next = front;
							face->m_prev = back;
						} else {
							//dgEdge* const back = ptr;
							HACD_ASSERT (ptr);
							dgEdge* const front = ptr->m_twin;
							HACD_ASSERT (front);
							dgEdge* ptr1 = front;
							do {
								ptr1->m_mark = mark;
								ptr1 = ptr1->m_next;
							} while (ptr1 != front);
						}
					}
					break;
				}
				ptr = ptr->m_next;

			} while (ptr != face);
		}
	}

	dgMeshEffect* backMesh = HACD_NEW(dgMeshEffect)(true);
	dgMeshEffect* frontMesh = HACD_NEW(dgMeshEffect)(true);

	mark = mesh.IncLRU();
	backMesh->BeginPolygon();
	frontMesh->BeginPolygon(); 
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if ((face->m_incidentFace > 0) && (face->m_mark != mark) && (vertexSide[face->m_incidentVertex] != 0)) {
			dgVertexAtribute att[128];

			hacd::HaI32 count = 0;
			dgEdge* ptr = face;
			do {
				att[count] = mesh.m_attib[ptr->m_userData];
				count ++;
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face);

			if (vertexSide[face->m_incidentVertex] > 0) {
				frontMesh->AddPolygon(count, &att[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (att[0].m_material));
			} else {
				backMesh->AddPolygon(count, &att[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (att[0].m_material));
			}
		}
	}

	backMesh->EndPolygon(hacd::HaF64 (1.0e-5f));
	frontMesh->EndPolygon(hacd::HaF64 (1.0e-5f)); 

	if (!(backMesh->GetCount() && frontMesh->GetCount())) {
		backMesh->Release();
		frontMesh->Release();
		frontMesh = NULL;
		backMesh = NULL;
	}

	if (backMesh && frontMesh) {
		HACD_ASSERT (backMesh->GetCount());
		HACD_ASSERT (frontMesh->GetCount());

		dgBigVector min;
		dgBigVector max;
		CalculateAABB (min, max);
		max -= min;
		hacd::HaF64 size = GetMax (max.m_x, max.m_y, max.m_z);
		dgMeshEffect planeMesh (planeMatrix, hacd::HaF32(size), hacd::HaF32(size), planeMaterial, planeTextMatrix, planeTextMatrix);

		if (!(backMesh->PlaneApplyCap (&planeMesh, plane) && frontMesh->PlaneApplyCap (&planeMesh, plane.Scale (hacd::HaF32 (-1.0f))))) {
			backMesh->Release();
			frontMesh->Release();
			*left = NULL;
			*right = NULL;
//		} else {
//			backMesh->Triangulate ();
//			frontMesh->Triangulate ();
		}
	}

	*left = backMesh;
	*right = frontMesh;
}


bool dgMeshEffect::CheckSingleMesh() const
{
HACD_ASSERT (0);
return false;
 /*

	bool ret;
	dgPolyhedra firstSegment(GetAllocator());
	dgPolyhedra secundSegment(GetAllocator());

	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		hacd::HaF32 err2;
		dgEdge* vertex;
		vertex = &(*iter);
		if (vertex->m_incidentFace >= 0) {
			dgVector p (m_attib[vertex->m_userData].m_vertex.m_x, m_attib[vertex->m_userData].m_vertex.m_y, m_attib[vertex->m_userData].m_vertex.m_z, hacd::HaF32 (0.0f));
			dgVector err (m_points[vertex->m_incidentVertex] - p);
			err2 = err % err;
			HACD_ASSERT (err2 < hacd::HaF32 (1.0e-10f));
		} 
	}

	BeginConectedSurface();
	GetConectedSurface (firstSegment);
	GetConectedSurface (secundSegment);
	EndConectedSurface();
	ret = (firstSegment.GetCount() > 0) & (secundSegment.GetCount() == 0);
	return ret;
*/
}

hacd::HaI32 dgMeshEffect::PlaneApplyCap (const dgMeshEffect* planeMesh, const dgBigPlane& faceNormal)
{
	dgEdge* plane = &planeMesh->GetRoot()->GetInfo();
	if (plane->m_incidentFace < 0) {
		plane = plane->m_twin;
	}
	HACD_ASSERT (plane->m_incidentFace > 0);

	hacd::HaI32 ret = 0;
	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; ) {
		dgEdge* face = &(*iter);

		iter++;

		if ((face->m_incidentFace < 0) && (face->m_mark != mark)) {
			hacd::HaF64 maxDist = hacd::HaF32 (0.0f);
			dgEdge* ptr = face;
			do {
				maxDist = GetMax (maxDist, fabs (faceNormal.Evalue(m_points[ptr->m_incidentVertex])));
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face); 

			if (maxDist <= hacd::HaF32 (1.5e-3f)) {
				bool haveColinear = true;
				ptr = face;
				while (haveColinear) {
					haveColinear = false;
					do {
						if (ptr->m_next->m_twin->m_next->m_twin != ptr) {
							dgBigVector e0 (m_points[ptr->m_next->m_incidentVertex] - m_points[ptr->m_incidentVertex]);
							dgBigVector e1 (m_points[ptr->m_next->m_next->m_incidentVertex] - m_points[ptr->m_next->m_incidentVertex]);

							hacd::HaF64 mag00 = e0 % e0;
							hacd::HaF64 mag11 = e1 % e1;
							hacd::HaF64 mag01 = e0 % e1;

							hacd::HaF64 epsilon = hacd::HaF64 (1.0e-6f) * mag00 * mag11;
							hacd::HaF64 err = mag01 * mag01 - mag00 * mag11;
							if (fabs (err) < epsilon) {
								HACD_ASSERT (ptr->m_twin->m_incidentFace >= 0);

								dgBigVector normal0 (FaceNormal(ptr->m_twin, &m_points[0].m_x, sizeof (dgBigVector)));
								mag00 = normal0 % normal0;
								dgEdge* ptr1 = ptr->m_twin->m_prev->m_twin;
								do {
									dgBigVector normal1 (FaceNormal(ptr1->m_twin, &m_points[0].m_x, sizeof (dgBigVector)));
								
									mag11 = normal1 % normal1;
									mag01 = normal0 % normal1;
									epsilon = hacd::HaF64 (1.0e-6f) * mag00 * mag11;
									err = mag01 * mag01 - mag00 * mag11;
									if (fabs (err) < epsilon) {
										if (iter && ((&(*iter) == ptr1) || (&(*iter) == ptr1->m_twin))) {
											iter --;
										}
										if (iter && ((&(*iter) == ptr1) || (&(*iter) == ptr1->m_twin))) {
											iter --;
										}
										haveColinear = true;
										DeleteEdge(ptr1);
										ptr1 = ptr->m_twin;
										
									}

									ptr1 = ptr1->m_prev->m_twin;

								} while (ptr1 != ptr->m_next);

								if (ptr->m_next->m_twin->m_next->m_twin == ptr) {
									if (iter && ((&(*iter) == ptr->m_next) || (&(*iter) == ptr->m_next->m_twin))) {
										iter --;
										if (iter && ((&(*iter) == ptr->m_next) || (&(*iter) == ptr->m_next->m_twin))) {
											iter --;
										}
									}

									if (ptr->m_next == face) {
										face = face->m_prev;
									}
									
									ptr->m_twin->m_userData = ptr->m_next->m_twin->m_userData;
									ptr->m_twin->m_incidentVertex = ptr->m_next->m_twin->m_incidentVertex;

									dgEdge* const next = ptr->m_next;
									ptr->m_next->m_next->m_prev = ptr;
									ptr->m_next = ptr->m_next->m_next;
									ptr->m_twin->m_prev->m_prev->m_next = ptr->m_twin;
									ptr->m_twin->m_prev = ptr->m_twin->m_prev->m_prev;

									next->m_next = next->m_twin;
									next->m_prev = next->m_twin;
									next->m_twin->m_next = next;
									next->m_twin->m_prev = next;
									DeleteEdge (next);

									dgTreeNode* node = GetNodeFromInfo(*ptr);
									dgPairKey key0 (ptr->m_incidentVertex, ptr->m_twin->m_incidentVertex);
									if (Find(key0.GetVal())) {
										return 0;
									}
									node = ReplaceKey (node, key0.GetVal());

									node = GetNodeFromInfo(*ptr->m_twin);
									dgPairKey key1 (ptr->m_twin->m_incidentVertex, ptr->m_incidentVertex);
									if (Find(key1.GetVal())) {
										return 0;
									}
									node = ReplaceKey (node, key1.GetVal());
								}
							}
						}

						ptr = ptr->m_next;
					} while (ptr != face) ; 
				}


				ptr = face;
				do {
					dgVertexAtribute attrib (planeMesh->InterpolateVertex (m_points[ptr->m_incidentVertex], plane));
					attrib.m_normal_x = faceNormal.m_x;
					attrib.m_normal_y = faceNormal.m_y;
					attrib.m_normal_z = faceNormal.m_z;

					AddAtribute(attrib);

					ptr->m_userData = m_atribCount - 1;
					ptr->m_incidentFace = 1;

					ptr = ptr->m_next;
				} while (ptr != face); 
//				dgVector normal;
//				TriangulateFace (face, &m_points[0].m_x, sizeof (dgVector), normal);
				ret = 1;
			}
		}
	}
	return ret;
}

bool dgMeshEffect::HasOpenEdges () const
{
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		dgEdge* const face = &(*iter);
		if (face->m_incidentFace < 0){
			return true;
		}
	}
	return false;
}




hacd::HaF64 dgMeshEffect::CalculateVolume () const
{
	HACD_ASSERT (0);
	return 0;
	/*

	dgPolyhedraMassProperties localData;

	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);
	for (iter.Begin(); iter; iter ++){
		hacd::HaI32 count;
		dgEdge* ptr;
		dgEdge* face;
		dgVector points[256];
		
		face = &(*iter);
		if ((face->m_incidentFace > 0) && (face->m_mark != mark)) {
			count = 0;
			ptr = face;
			do {
				points[count] = m_points[ptr->m_incidentVertex];
				count ++;
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face);
			localData.AddCGFace (count, points);
		}
	}

	hacd::HaF32 volume;
	dgVector p0;
	dgVector p1;
	dgVector com;
	dgVector inertia;
	dgVector crossInertia;
	volume = localData.MassProperties (com, inertia, crossInertia);
	return volume;
*/
}




bool dgMeshEffect::SeparateDuplicateLoops (dgEdge* const face)
{
	for (dgEdge* ptr0 = face; ptr0 != face->m_prev; ptr0 = ptr0->m_next) {
		hacd::HaI32 index = ptr0->m_incidentVertex;

		dgEdge* ptr1 = ptr0->m_next; 
		do {
			if (ptr1->m_incidentVertex == index) {
				dgEdge* const ptr00 = ptr0->m_prev;
				dgEdge* const ptr11 = ptr1->m_prev;

				ptr00->m_next = ptr1;
				ptr1->m_prev = ptr00;

				ptr11->m_next = ptr0;
				ptr0->m_prev = ptr11;

				return true;
			}

			ptr1 = ptr1->m_next;
		} while (ptr1 != face);
	}

	return false;
}


dgMeshEffect* dgMeshEffect::GetNextLayer (hacd::HaI32 mark)
{
	Iterator iter(*this);
	dgEdge* edge = NULL;
	for (iter.Begin (); iter; iter ++) {
		edge = &(*iter);
		if ((edge->m_mark < mark) && (edge->m_incidentFace > 0)) {
			break;
		}
	}

	if (!edge) {
		return NULL;
	}

	hacd::HaI32 layer = hacd::HaI32 (m_points[edge->m_incidentVertex].m_w);
	dgPolyhedra polyhedra;

	polyhedra.BeginFace ();
	for (iter.Begin (); iter; iter ++) {
		dgEdge* const edge = &(*iter);
		if ((edge->m_mark < mark) && (edge->m_incidentFace > 0)) {
			hacd::HaI32 thislayer = hacd::HaI32 (m_points[edge->m_incidentVertex].m_w);
			if (thislayer == layer) {
				dgEdge* ptr = edge;
				hacd::HaI32 count = 0;
				hacd::HaI32 faceIndex[256];
				hacd::HaI64 faceDataIndex[256];
				do {
					ptr->m_mark = mark;
					faceIndex[count] = ptr->m_incidentVertex;
					faceDataIndex[count] = ptr->m_userData;
					count ++;
					HACD_ASSERT (count < hacd::HaI32 (sizeof (faceIndex)/ sizeof(faceIndex[0])));
					ptr = ptr->m_next;
				} while (ptr != edge);
				polyhedra.AddFace (count, &faceIndex[0], &faceDataIndex[0]);
			}
		}
	}
	polyhedra.EndFace ();

	dgMeshEffect* solid = NULL;
	if (polyhedra.GetCount()) {
		solid = HACD_NEW(dgMeshEffect)(polyhedra, *this);
		solid->SetLRU(mark);
	}
	return solid;
}


void dgMeshEffect::ClipMesh (const dgMeshEffectSolidTree* const clipper, dgMeshEffect** const left, dgMeshEffect** const right, dgMeshEffect** const coplanar) const
{
	HACD_ALWAYS_ASSERT(); // not implemented in the reduced source version
#if 0  // UNUSED

	dgMeshEffect mesh (dgMeshEffect (*this));
	mesh.Triangulate();

	dgMeshEffect* const backMesh = HACD_NEW(dgMeshEffect)(true);
	dgMeshEffect* const frontMesh = HACD_NEW(dgMeshEffect)(true);
	dgMeshEffect* const meshCoplanar = HACD_NEW(dgMeshEffect)(true);

	backMesh->BeginPolygon();
	frontMesh->BeginPolygon();
	meshCoplanar->BeginPolygon();

	hacd::HaI32 mark = mesh.IncLRU();
	dgPolyhedra::Iterator iter (mesh);
	for (iter.Begin(); iter; iter ++) {
		dgEdge* const face = &(*iter);
		if ((face->m_incidentFace > 0) && (face->m_mark != mark)) {
			dgEdge* ptr = face;
			do {
				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face);


			dgList<dgMeshTreeCSGFace*> faceList;
			dgMeshTreeCSGFace* faceOnStack[DG_MESH_EFFECT_BOLLEAN_STACK];
			const dgMeshEffectSolidTree* stackPool[DG_MESH_EFFECT_BOLLEAN_STACK];
			hacd::HaI32 stack = 1;
			dgMeshTreeCSGFace* const originalFace = HACD_NEW(dgMeshTreeCSGFace)(mesh, face);
			faceOnStack[0] = originalFace;
			stackPool[0] = clipper;

			hacd::HaI32 backCount = 0;
			hacd::HaI32 frontCount = 0;
			bool hasCoplanar = false;

			originalFace->AddRef();


//xxx ++;
//dgMatrix xxxx (originalFace->DebugMatrix());
//if (xxx == 1582) {
//originalFace->Trace(xxxx);
//}



			while (stack) {

				stack --;
				dgMeshTreeCSGFace* const treeFace = faceOnStack[stack];
				const dgMeshEffectSolidTree* const root = stackPool[stack];

				HACD_ASSERT (root->m_planeType == dgMeshEffectSolidTree::m_divider);


				dgMeshTreeCSGFace* backFace; 
				dgMeshTreeCSGFace* frontFace;
				treeFace->Clip(root->m_plane, &backFace, &frontFace);
				treeFace->Release();

				if (!(frontFace || backFace)) {
					HACD_ASSERT (0);
/*
					hasCoplanar = true;
					if (!((root->m_front->m_planeType == dgMeshEffectSolidTree::m_divider) || (root->m_back->m_planeType == dgMeshEffectSolidTree::m_divider))) {
						HACD_ASSERT (face->DetermineSide(clipper) != 0);
						faceList.Append(face);
					} else {
						//HACD_ASSERT (!(root->m_front && root->m_back));
						if (root->m_front->m_planeType == dgMeshEffectSolidTree::m_divider) {
							stackPool[stack] = root->m_front;
							faceOnStack[stack] = face;
							stack ++;
							HACD_ASSERT (stack < hacd::HaI32 (sizeof (stackPool) / sizeof (stackPool[0])));
						} else {
							//if (root->m_back) {
							HACD_ASSERT (root->m_back->m_planeType == dgMeshEffectSolidTree::m_divider);
							stackPool[stack] = root->m_back;
							faceOnStack[stack] = face;
							stack ++;
							HACD_ASSERT (stack < hacd::HaI32 (sizeof (stackPool) / sizeof (stackPool[0])));
						}
					}
*/
				} else {
					if (frontFace) {
						if (root->m_front->m_planeType == dgMeshEffectSolidTree::m_divider) {
							stackPool[stack] = root->m_front;
							faceOnStack[stack] = frontFace;
							stack ++;
							HACD_ASSERT (stack < hacd::HaI32 (sizeof (stackPool) / sizeof (stackPool[0])));
						} else {

//if (xxx == 485){
//frontFace->Trace(xxxx);
//}


							frontCount ++;
							frontFace->m_side = dgMeshEffectSolidTree::m_empty;
							HACD_ASSERT (clipper->GetFaceSide(frontFace) == dgMeshEffectSolidTree::m_empty);
							faceList.Append (frontFace);
						}
					}

					if (backFace) {
						if (root->m_back->m_planeType == dgMeshEffectSolidTree::m_divider) {
							stackPool[stack] = root->m_back;
							faceOnStack[stack] = backFace;
							stack ++;
							HACD_ASSERT (stack < hacd::HaI32 (sizeof (stackPool) / sizeof (stackPool[0])));
						} else {

//if (xxx == 485){
//backFace->Trace(xxxx);
//}

							backCount ++;
							backFace->m_side = dgMeshEffectSolidTree::m_solid;
							HACD_ASSERT (clipper->GetFaceSide(backFace) == dgMeshEffectSolidTree::m_solid);
							faceList.Append (backFace);
						}
					}
				}
			}

			HACD_ASSERT (faceList.GetCount());
			if (!hasCoplanar && ((backCount == 0) || (frontCount == 0))) {
				hacd::HaI32 count = 0;
				dgMeshEffect::dgVertexAtribute facePoints[256];
				for (dgMeshTreeCSGFace::dgListNode* node = originalFace->GetFirst(); node; node = node->GetNext()) {
					//facePoints[count] = node->GetInfo().GetPoint();
					dgBigVector p (node->GetInfo().m_x.GetAproximateValue(), node->GetInfo().m_y.GetAproximateValue(), node->GetInfo().m_z.GetAproximateValue(), hacd::HaF64 (0.0));
					facePoints[count] = mesh.InterpolateVertex(p, face);
					facePoints[count].m_vertex = p;
					count ++;
				}

				if (frontCount) {
					#ifdef _DEBUG
					for (dgList<dgMeshTreeCSGFace*>::dgListNode* node1 = faceList.GetFirst(); node1; node1 = node1->GetNext()) {
						dgMeshTreeCSGFace* const face = node1->GetInfo();
						HACD_ASSERT (clipper->GetFaceSide(face) == dgMeshEffectSolidTree::m_empty);
					}
					#endif
					frontMesh->AddPolygon(count, &facePoints[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (facePoints[0].m_material));
				} else {
					#ifdef _DEBUG
					for (dgList<dgMeshTreeCSGFace*>::dgListNode* node1 = faceList.GetFirst(); node1; node1 = node1->GetNext()) {
						dgMeshTreeCSGFace* const face = node1->GetInfo();
						HACD_ASSERT (clipper->GetFaceSide(face) == dgMeshEffectSolidTree::m_solid);
					}
					#endif
					backMesh->AddPolygon(count, &facePoints[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (facePoints[0].m_material));
				}
			} else {

				for (dgList<dgMeshTreeCSGFace*>::dgListNode* node = faceList.GetFirst(); node->GetNext(); node = node->GetNext()) {
					dgMeshTreeCSGFace* const face0 = node->GetInfo();
					for (dgList<dgMeshTreeCSGFace*>::dgListNode* node1 = node->GetNext(); node1; node1 = node1->GetNext()) {
						dgMeshTreeCSGFace* const face1 = node1->GetInfo();
						face0->MergeMissingVertex (face1);
						face1->MergeMissingVertex (face0);
					}
				}

				for (dgList<dgMeshTreeCSGFace*>::dgListNode* node1 = faceList.GetFirst(); node1; node1 = node1->GetNext()) {
					dgMeshTreeCSGFace* const treFace = node1->GetInfo();

//xxx1 ++;
//if (xxx1 == 24310)
//face->Trace(xxxx);

					hacd::HaI32 count = 0;
					dgVertexAtribute facePoints[256];
					for (dgMeshTreeCSGFace::dgListNode* node = treFace->GetFirst(); node; node = node->GetNext()) {
						dgBigVector p (node->GetInfo().m_x.GetAproximateValue(), node->GetInfo().m_y.GetAproximateValue(), node->GetInfo().m_z.GetAproximateValue(), hacd::HaF64 (0.0));
						facePoints[count] = mesh.InterpolateVertex(p, face);
						facePoints[count].m_vertex = p;
						count ++;
					}

					switch (treFace->m_side) 
					{
						case dgMeshEffectSolidTree::m_divider:
						{
							HACD_ASSERT (0);
							meshCoplanar->AddPolygon(count, &facePoints[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (facePoints[0].m_material));
							break;
						}
						case dgMeshEffectSolidTree::m_solid:
						{
							backMesh->AddPolygon(count, &facePoints[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (facePoints[0].m_material));							
							break;
						}

						case dgMeshEffectSolidTree::m_empty:
						{
							frontMesh->AddPolygon(count, &facePoints[0].m_vertex.m_x, sizeof (dgVertexAtribute), dgFastInt (facePoints[0].m_material));
							break;
						}
					}
				}
			}

			originalFace->Release();
			for (dgList<dgMeshTreeCSGFace*>::dgListNode* node = faceList.GetFirst(); node; node = node->GetNext()) {
				dgMeshTreeCSGFace* const face = node->GetInfo();
				face->Release();
			}
		}
	}


	backMesh->EndPolygon(hacd::HaF64 (hacd::HaF64 (1.0e-5f)));
	frontMesh->EndPolygon(hacd::HaF64 (hacd::HaF64 (1.0e-5f)));
	meshCoplanar->EndPolygon(hacd::HaF64 (hacd::HaF64 (1.0e-5f)));

	if (backMesh->GetCount() && frontMesh->GetCount()) {
		*left = backMesh;
		*right = frontMesh;
	} else if (frontMesh->GetCount()){
		*left = NULL;
		*right = frontMesh;
		backMesh->Release();
	} else if (backMesh->GetCount()){
		*right = NULL;
		*left = backMesh;
		frontMesh->Release();
	} else {
		HACD_ASSERT (0);
		*right = NULL;
		*left =  NULL;
		backMesh->Release();
		frontMesh->Release();
	}

	*coplanar = NULL;
	if (meshCoplanar->GetCount()) {
		*coplanar = meshCoplanar;
	} else {
		meshCoplanar->Release();
	}
#endif
}

void dgMeshEffect::RepairTJoints (bool triangulate)
{
	hacd::HaI32 mark = IncLRU();
	dgPolyhedra::Iterator iter (*this);
#ifdef _DEBUG
	for (iter.Begin(); iter; iter ++) {
		dgEdge* const face = &(*iter);
		if ((face->m_incidentFace < 0) && (face->m_mark != mark)) {
			for (dgEdge* ptr = face; ptr != face->m_prev; ptr = ptr->m_next) {
				dgBigVector p0 (m_points[ptr->m_incidentVertex]);
				for (dgEdge* ptr1 = ptr->m_next; ptr1 != face; ptr1 = ptr1->m_next) {
					if (ptr->m_incidentVertex != ptr1->m_incidentVertex) {
						dgBigVector p1 (m_points[ptr1->m_incidentVertex]);
						dgBigVector dp (p1 - p0);
						hacd::HaF64 err2 (dp % dp);
						if (err2 < hacd::HaF64 (1.0e-16f)) {
//							HACD_ASSERT (0);
						}
					}
				} 
			}
		}
	}
	mark = IncLRU();
#endif



	for (iter.Begin(); iter; ) {
		dgEdge* const face = &(*iter);
		iter ++;


		if ((face->m_incidentFace < 0) && (face->m_mark != mark)) {
			// vertices project 

			while (SeparateDuplicateLoops (face));

			dgBigVector dir (hacd::HaF64 (0.0f), hacd::HaF64 (0.0f), hacd::HaF64 (0.0f), hacd::HaF64 (0.0f));
			hacd::HaF64 lengh2 = hacd::HaF64 (0.0f);
			dgEdge* ptr = face;
			do {
				dgBigVector dir1 (m_points[ptr->m_next->m_incidentVertex] - m_points[ptr->m_incidentVertex]);
				hacd::HaF64 val = dir1 % dir1;
				if (val > lengh2) {
					lengh2 = val;
					dir = dir1;
				}
				ptr = ptr->m_next;
			} while (ptr != face);

			HACD_ASSERT (lengh2 > hacd::HaF32 (0.0f));

			dgEdge* lastEdge = NULL;
			dgEdge* firstEdge = NULL;
			hacd::HaF64 minVal = hacd::HaF64 (-1.0e10f);
			hacd::HaF64 maxVal = hacd::HaF64 (-1.0e10f);
			ptr = face;
			do {
				const dgBigVector& p = m_points[ptr->m_incidentVertex];
				hacd::HaF64 val = p % dir;
				if (val > maxVal) {
					maxVal = val;
					lastEdge = ptr;
				}
				val *= hacd::HaF64 (-1.0f);
				if (val > minVal) {
					minVal = val;
					firstEdge = ptr;
				}

				ptr->m_mark = mark;
				ptr = ptr->m_next;
			} while (ptr != face);

			HACD_ASSERT (firstEdge);
			HACD_ASSERT (lastEdge);

			bool isTJoint = true;
			dgBigVector p0 (m_points[firstEdge->m_incidentVertex]);
			dgBigVector p1 (m_points[lastEdge->m_incidentVertex]);
			dgBigVector p1p0 (p1 - p0);
			hacd::HaF64 den = p1p0 % p1p0;
			ptr = firstEdge->m_next;
			do {
				dgBigVector p2 (m_points[ptr->m_incidentVertex]);
				hacd::HaF64 num = (p2 - p0) % p1p0;
				dgBigVector q (p0 + p1p0.Scale (num / den));
				dgBigVector dist (p2 - q);
				hacd::HaF64 err2 = dist % dist;
				isTJoint &= (err2 < (hacd::HaF64 (1.0e-4f) * hacd::HaF64 (1.0e-4f)));
				ptr = ptr->m_next;
			} while (isTJoint && (ptr != firstEdge));

			if (isTJoint) {
				do {
					dgEdge* next = NULL;

					const dgBigVector p0 = m_points[firstEdge->m_incidentVertex];
					const dgBigVector p1 = m_points[firstEdge->m_next->m_incidentVertex];
					const dgBigVector p2 = m_points[firstEdge->m_prev->m_incidentVertex];

					dgBigVector p1p0 (p1 - p0);
					dgBigVector p2p0 (p2 - p0);
					hacd::HaF64 dist10 = p1p0 % p1p0;
					hacd::HaF64 dist20 = p2p0 % p2p0;

					dgEdge* begin = NULL;
					dgEdge* last = NULL;
					if (dist20 > dist10) {
						hacd::HaF64 t = (p1p0 % p2p0) / dist20;
						HACD_ASSERT (t > hacd::HaF32 (0.0f));
						HACD_ASSERT (t < hacd::HaF32 (1.0f));

						if (firstEdge->m_next->m_next->m_next != firstEdge) {
							ConectVertex (firstEdge->m_prev, firstEdge->m_next);
							next = firstEdge->m_next->m_twin->m_next;
						}
						HACD_ASSERT (firstEdge->m_next->m_next->m_next == firstEdge);

#ifdef _DEBUG
						dgEdge* tmp = firstEdge->m_twin;
						do {
							HACD_ASSERT (tmp->m_incidentFace > 0);
							tmp = tmp->m_next;
						} while (tmp != firstEdge->m_twin); 
#endif

						begin = firstEdge->m_next;
						last = firstEdge;
						firstEdge->m_userData = firstEdge->m_prev->m_twin->m_userData;
						firstEdge->m_incidentFace = firstEdge->m_prev->m_twin->m_incidentFace;
						dgVertexAtribute attrib (InterpolateEdge (firstEdge->m_prev->m_twin, t));
						attrib.m_vertex = m_points[firstEdge->m_next->m_incidentVertex];
						AddAtribute(attrib);
						firstEdge->m_next->m_incidentFace = firstEdge->m_prev->m_twin->m_incidentFace;
						firstEdge->m_next->m_userData = hacd::HaU64 (m_atribCount - 1);

						bool restart = false;
						if ((firstEdge->m_prev == &(*iter)) || (firstEdge->m_prev->m_twin == &(*iter))) {
							restart = true;
						}
						DeleteEdge(firstEdge->m_prev);
						if (restart) {
							iter.Begin();
						}

					} else {

						HACD_ASSERT (dist20 < dist10);

						hacd::HaF64 t = (p1p0 % p2p0) / dist10;
						HACD_ASSERT (t > hacd::HaF32 (0.0f));
						HACD_ASSERT (t < hacd::HaF32 (1.0f));

						if (firstEdge->m_next->m_next->m_next != firstEdge) {
							ConectVertex (firstEdge->m_next, firstEdge->m_prev);
							next = firstEdge->m_next->m_twin;
						}
						HACD_ASSERT (firstEdge->m_next->m_next->m_next == firstEdge);

#ifdef _DEBUG
						dgEdge* tmp = firstEdge->m_twin;
						do {
							HACD_ASSERT (tmp->m_incidentFace > 0);
							tmp = tmp->m_next;
						} while (tmp != firstEdge->m_twin); 
#endif

						begin = firstEdge->m_prev;
						last = firstEdge->m_next;
						firstEdge->m_next->m_userData = firstEdge->m_twin->m_userData;
						firstEdge->m_next->m_incidentFace = firstEdge->m_twin->m_incidentFace;
						dgVertexAtribute attrib (InterpolateEdge (firstEdge->m_twin, hacd::HaF64 (1.0f) - t));
						attrib.m_vertex = m_points[firstEdge->m_prev->m_incidentVertex];
						AddAtribute(attrib);
						firstEdge->m_prev->m_incidentFace = firstEdge->m_twin->m_incidentFace;
						firstEdge->m_prev->m_userData = hacd::HaU64 (m_atribCount - 1);

						bool restart = false;
						if ((firstEdge == &(*iter)) || (firstEdge->m_twin == &(*iter))) {
							restart = true;
						}
						DeleteEdge(firstEdge);
						if (restart) {
							iter.Begin();
						}
					}

					if (triangulate) {
						HACD_ASSERT (begin);
						HACD_ASSERT (last);
						for (dgEdge* ptr = begin->m_next->m_next; ptr != last; ptr = ptr->m_next) {
							dgEdge* const e = AddHalfEdge (begin->m_incidentVertex, ptr->m_incidentVertex);
							dgEdge* const t = AddHalfEdge (ptr->m_incidentVertex, begin->m_incidentVertex);
							if (e && t) {
								HACD_ASSERT (e);
								HACD_ASSERT (t);
								e->m_twin = t;
								t->m_twin = e;

								e->m_incidentFace = ptr->m_incidentFace;
								t->m_incidentFace = ptr->m_incidentFace;

								e->m_userData = last->m_next->m_userData;
								t->m_userData = ptr->m_userData;

								t->m_prev = ptr->m_prev;
								ptr->m_prev->m_next = t;
								e->m_next = ptr;
								ptr->m_prev = e;
								t->m_next = last->m_next;
								e->m_prev = last;
								last->m_next->m_prev = t;
								last->m_next = e;
							}
						}
					}

					firstEdge = next;
				} while (firstEdge);
			}
		}
	}

	DeleteDegenerateFaces(&m_points[0].m_x, sizeof (m_points[0]), hacd::HaF64 (1.0e-7f));
}
