/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CConvexHull.h"
#include "ObjectFactory.h"
#include "MeshCollider.h"
#include "EditorSubsystem.h"
void CConvexHull::CopyFrom(const ::CConvexHull* c)
{

}

void CConvexHull::OnEditorUpdate(void)
{

}

void CConvexHull::OnFree(void)
{

}

void CConvexHull::OnAlwaysUpdate(void)
{

}

void CConvexHull::OnUpdate(void)
{
	if (Engine::getInstance()->GetIsEditorMode())
		if (!EditorSubsystem::getInstance()->IsTesting())
		return;
	if (mOnce)
			return;
	// find name of level mesh
	CMeshRenderer* cmr = reinterpret_cast<CMeshRenderer*>(gameObject->GetComponentByName("CMeshRenderer"));
	std::string meshName = cmr->GetMesh()->GetName();
	std::string key(".");
	std::size_t found = meshName.find_last_of(key);
	if (found != std::string::npos)
		meshName.replace(found, key.length() + 5, ".hull");


	// find convex hulls data from it
	AssetHandle hand = AssetManager::getInstance()->GetAsset(meshName);
	ConvexHullMesh* m = hand.GetAssetPtr<ConvexHullMesh>();

	// create multiple gameobject with mesh collider
	for (unsigned i = 0; i < m->hullLists.size(); ++i)
	{
		// set Parent
		std::string name("Children");
		std::string s = std::to_string(i);
		name += s;
		GameObject* go = ObjectFactory::getInstance()->MakeNewGameObject(name);
		go->transform->SetPosition(gameObject->transform->GetPosition());
		go->transform->rotation = gameObject->transform->GetRotation();
		go->transform->scale = gameObject->transform->GetScale();
		go->transform->UpdateWorldTransform();
		go->transform->SetParent(gameObject->transform);
		CPhysics* cphy = reinterpret_cast<CPhysics*>(go->RequireComponentByName("CPhysics"));
		CMeshColliderComponent* cmc = reinterpret_cast<CMeshColliderComponent*>(go->RequireComponentByName("CMeshColliderComponent"));
		cmc->mHasHull = true;
		cmc->mIndexOfHull = i;
		cmc->mHulls = hand;
		cmc->nameOfHull = meshName;
		int h = 0;
	}

	mOnce = true;
}

void CConvexHull::OnInit(void)
{

}

void CConvexHull::OnStart(void)
{
	mOnce = false;

}

___COMP_REGISTER(CConvexHull);
meta_define(CConvexHull);
//safety
