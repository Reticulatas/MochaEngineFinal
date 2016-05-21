#include "stdafx.h"
#include "MeshCollider.h"
#include "MeshShape.h"
#include "MeshShapeData.h"
#include "CMeshRenderer.h"
const std::string CMeshColliderComponent::ID = "CMeshColliderComponent";

CMeshColliderComponent::CMeshColliderComponent(void) : mHasMesh(false), mHasHull(false)
{ 
	//SetMesh("E://cube.obj");
}

void CMeshColliderComponent::OnFree(void)
{
	ColliderComponent::OnFree();
}

void CMeshColliderComponent::OnInit(void)
{
	ColliderComponent::OnInit();
}

void CMeshColliderComponent::OnStart(void)
{
	if (!gameObject->HasComponent<CPhysics>())
	{
		mCphy = gameObject->AddComponent<CPhysics>();
		mCphy->collidarCom = this;
	}
	else
	{
		mCphy = static_cast<CPhysics*>(gameObject->GetComponentByName("CPhysics"));
		mCphy->collidarCom = this;
	}

	ColliderComponent::OnStart();
}

void CMeshColliderComponent::OnUpdate(void)
{
	ColliderComponent::OnUpdate();
}


CMeshColliderComponent &CMeshColliderComponent::SetMesh(const std::string &file)
{
	m_file = file;
	InvalidateCollider();
	return *this;
}

Collider *CMeshColliderComponent::CreateCollider(RigidBody &body)
{
	if (!mHasHull)// read in from mesh
	{
		
		std::vector<std::vector<Vec3>> vertLists;
		std::vector<std::vector<signed>> faceLists;
		vertLists.resize(1);
		faceLists.resize(1);

		CMeshRenderer* mesh = mCphy->gameObject->GetComponent<CMeshRenderer>();
		if (mesh == 0)
		{
			throw MochaException("MeshCollider has no CMeshRenderer to get mesh from - Kai problem");
		}

		std::vector<Vector3> temp = mesh->GetMesh()->GetUniqueVertices();
		for (unsigned j = 0; j < temp.size(); ++j)
		{
			Vec3 pos(temp[j].x, temp[j].y, temp[j].z);
			vertLists[0].push_back(pos);
		}
		faceLists[0] = mesh->GetMesh()->GetUnqiueIndices();

		return &body.CreateCollider(MeshColliderDef3D(vertLists, faceLists));
	}
	else // read in from ConvexHull
	{
		AssetHandle hand = AssetManager::getInstance()->GetAsset(nameOfHull);
		ConvexHullMesh* m = hand.GetAssetPtr<ConvexHullMesh>();
		std::vector<float> verts = m->hullLists[mIndexOfHull].verts;
		std::vector<int>   indic = m->hullLists[mIndexOfHull].indices;

		std::vector<std::vector<Vec3>> vertLists;
		std::vector<std::vector<signed>> faceLists;
		vertLists.resize(1);
		faceLists.resize(1);

		for (unsigned j = 0; j < verts.size() / 3; ++j)
		{
			Vec3 pos(verts[j * 3 + 0], verts[j * 3 + 1], verts[j * 3 + 2]);
			vertLists[0].push_back(pos);
		}
		faceLists[0] = indic;

		return &body.CreateCollider(MeshColliderDef3D(vertLists, faceLists));
	}
}

const Vector3& CMeshColliderComponent::UpdateSize(const Vector3 &size)
{
	// TODOKai
	return size;
}



meta_define(CMeshColliderComponent);
___COMP_REGISTER(CMeshColliderComponent);
