/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "Collidar.h"
#include <string>
#include "AdjancencyInfo.h"
struct MeshColliderDef3D
	: public ColliderDef
{
	friend class MeshColliderGeometry3D;
public:

	MeshColliderDef3D(void)
		: ColliderDef(ColliderType::MESH)
	{ }

	MeshColliderDef3D(const std::string &file)
		: ColliderDef(ColliderType::MESH)
	{
		Load(file);
	}
	void Load(const std::string &file);

	// for STL containers
	template <typename T1, typename T2>
	MeshColliderDef3D(const T1 &vertLists, const T2 &faceLists)
		: ColliderDef(ColliderType::MESH)
	{
		int i;

		i = 0;
		for (const auto &verts : vertLists)
			++i;
		adjacencies.resize(i);

		i = 0;
		for (const auto &verts : vertLists)
		{
			auto &adj = adjacencies[i++];
			for (const Math::Vector3 &v : verts)
				adj.AddVert(v);
		} // end for each vert list

		i = 0;
		for (const auto &faces : faceLists)
		{
			auto &adj = adjacencies[i++];
			unsigned faceVertIndices[3];
			int j = 0;
			for (auto index : faces)
			{
				faceVertIndices[j++] = index;
				if (j == 3)
				{
					adj.AddFace(faceVertIndices[0], faceVertIndices[1], faceVertIndices[2]);
					j = 0;
				}
			} // end for each vert index
		} // end for each face list
	}

private:

	std::vector<AdjacencyInfo> adjacencies;
};

class MeshColliderGeometry3D
	: public ColliderGeometry
{
public:
	MeshColliderGeometry3D(const MeshColliderDef3D &def)
		: ColliderGeometry(ColliderType::MESH)
		, m_adjacencies(def.adjacencies)
	{ }
	void DrawMesh(Color& color);
	virtual const Vector3& UpdateSize(const Vector3 &scale);
	virtual void Init(void);
	virtual void Dispose(void);
	virtual const Math::Vector3 Support(const Math::Vector3 &dir) const;
	virtual bool RayCast(const Ray3 &ray, float maxDistance, float &t, Math::Vector3 &n) const;
	virtual bool Collides(const Math::Vector3 &pos) const;
	virtual void UpdateMassAndLocalCentroid(const ResolutionMaterial &material, float &mass, Math::Matrix3 &unitInertiaTensor, Math::Vector3 &localCentroid);
	virtual void UpdateProxy(const RigidBody &body, Proxy &proxy);
	virtual void Draw(PhysicsDebugDraw &debugDraw, unsigned lineColor, unsigned fillColor, float fillAlpha) const;

	Math::Vector3 mScale;
	std::vector<Collider *> m_colliders;
private:
	std::vector<AdjacencyInfo> m_adjacencies;
	
};