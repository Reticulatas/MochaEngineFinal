/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "CWidget.h"

class CCamera;
class CWidget_Transform : public CWidget
{
	Vector3 lastAttachedObjScale;
	Vector3 lastAttachedObjRot;
	Vector3 lastAttachedObjPos;
    GameObject *gx, *gy, *gz, *gbase;

	void SetVisible(bool v);

	virtual ::Component* Clone(GameObject*) const { throw std::exception("Object can not be cloned"); }
public:
	virtual void OnInit();
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFree();
	virtual void OnDraw();
	virtual void OnMouseClick(CCamera* cam);
	virtual void OnMouseEnter(CCamera* cam) { }

	void SetLocalSpaceMode();
	void SetGlobalSpaceMode();
	void SetAttachedObjects(std::vector<GameObject*>& gobjptrs);
	void ClearAttachedObjects();
    void GlobalClearAttachedObjects();

	struct GameObjectTransformPack {
		Vector3 position;
		Vector3 scale;
		Vector3 rotation;
	};
	std::vector<std::pair<GameObject*, GameObjectTransformPack>>& GetAttachedObjects();
	std::vector<std::pair<GameObject*, GameObjectTransformPack>> selectedGameObjects;

	//is this widget currently being dragged by the user?
	bool GetIsDragging();

	metadef(CWidget_Transform)
		endmetadef;
};
