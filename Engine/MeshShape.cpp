#include "stdafx.h"
#include "MeshShape.h"
#include "RigidBody.h"
#include "RigidBodyCom.h"
#include "Collidar.h"
#include "ColliderCom.h"
#include "PolyheShape.h"
void MeshColliderDef3D::Load(const std::string &file)
{
	std::vector<std::vector<Math::Vector3>> vertLists;
	std::vector<std::vector<unsigned>> faceLists;

	std::FILE *shapeFile = std::fopen(file.c_str(), "rb");
	if (shapeFile)
	{
		unsigned numShapes;
		std::fread(&numShapes, sizeof(unsigned), 1, shapeFile);
		vertLists.resize(numShapes);
		faceLists.resize(numShapes);

		// read verts
		for (unsigned s = 0; s < numShapes; ++s)
		{
			unsigned numVerts;
			std::fread(&numVerts, sizeof(unsigned), 1, shapeFile);
			auto &verts = vertLists[s];
			verts.reserve(numVerts);
			for (unsigned v = 0; v < numVerts; ++v)
			{
				Math::Vector3 vert;
				std::fread(vert.ToFloats(), sizeof(float)* 3, 1, shapeFile);
				float dummy;
				std::fread(&dummy, sizeof(float), 1, shapeFile); // don't need w component
				verts.push_back(vert);
			}
		}

		// read face vert indices
		for (unsigned s = 0; s < numShapes; ++s)
		{
			// read faces
			unsigned numIndices;
			std::fread(&numIndices, sizeof(unsigned), 1, shapeFile);
			auto &faces = faceLists[s];
			faces.resize(numIndices);
			std::fread(&faces[0], sizeof(unsigned)* numIndices, 1, shapeFile);
		}
	}
	else
	{
		GV_ASSERT_MESSAGE(shapeFile, "Unable to load shape file: " + m_file);
	}

	// hacky copy
	adjacencies = MeshColliderDef3D(vertLists, faceLists).adjacencies;
}

void MeshColliderGeometry3D::Init(void)
{
	auto &body = mParent->Parent();
	m_colliders.reserve(m_adjacencies.size());
	CMeshRenderer* mesh = static_cast<CMeshRenderer*>(mParent->Parent().mParent->cphy->gameObject->GetComponentByName("CMeshRenderer"));

	for (auto &adj : m_adjacencies)
	{
		auto &collider = body.CreateCollider(PolyhedronColliderDef(adj));
		m_colliders.push_back(&collider);
	}
}

void MeshColliderGeometry3D::Dispose(void)
{
	auto &body = mParent->Parent();
	for (auto collider : m_colliders)
		body.DestroyCollider(*collider);
}

const Math::Vector3 MeshColliderGeometry3D::Support(const Math::Vector3 &dir) const
{
	GV_ASSERT_MESSAGE(false, "This function should never be called.");
	return Math::Vector3();
}

bool MeshColliderGeometry3D::RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const
{
	// underlying polyhedron meshes should handle this
	return false;
}

bool MeshColliderGeometry3D::Collides(const Math::Vector3 &pos) const
{
	// underlying polyhedron meshes should handle this
	return false;
}

void MeshColliderGeometry3D::UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid)
{
	// underlying polyhedron meshes should handle this
}

void MeshColliderGeometry3D::UpdateProxy(const RigidBody &body, Proxy &proxy)
{
	// no one should collide with this proxy
	proxy.minPoint.Set(FLT_MAX, FLT_MAX, FLT_MAX);
	proxy.maxPoint.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

void MeshColliderGeometry3D::Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const
{
	// underlying polyhedron meshes should handle this
}

const Vector3& MeshColliderGeometry3D::UpdateSize(const Vector3 &scale)
{
	// TODOKai
	mScale = scale;
	return scale;
}

void MeshColliderGeometry3D::DrawMesh(Color& color)
{


	const RigidBody &body = mParent->Parent();
	for (unsigned i = 0; i < m_adjacencies.size(); ++i)
	{
		const auto &verts = m_adjacencies[i].Verts();
		const auto &edges = m_adjacencies[i].Edges();
		const auto &faces = m_adjacencies[i].Faces();

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

			Vector3 v0G = body.LocalToGlobal(v0);
			Vector3 v1G = body.LocalToGlobal(v1);
			Vector3 v2G = body.LocalToGlobal(v2);
			Renderer::getInstance()->DrawLine(v0G, v1G, color);
			Renderer::getInstance()->DrawLine(v1G, v2G, color);
			Renderer::getInstance()->DrawLine(v2G, v0G, color);
		}

		//for (auto &face : faces)
		//{
		//	if (!face.active	)
		//		continue;
		//
		//	int e = face.edge;
		//	const int startEdge = e;
		//	do
		//	{
		//		const auto &edge = edges[e];
		//		const auto &next = edges[edge.next];
		//
		//		Vector3 start = body.LocalToGlobal(verts[edge.vert].position);
		//		Vector3 end = body.LocalToGlobal(verts[next.vert].position);
		//
		//		Renderer::getInstance()->DrawLine(start, end, Color(1, 0, 0));
		//		e = edge.next;
		//	} while (e != startEdge);
		//}
	}

}

