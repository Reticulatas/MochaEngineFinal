/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CWidget_Transform.h"
#include "CWidget_TransformAxis.h"
#include "EditorSubsystem.h"
#include "CTransform.h"
#include "ColorNode.h"
#include "EditorSubsystem_Interface.h"
#include "Renderer.h"
#include "ObjectFactory.h"
#include <list>

void CWidget_Transform::OnInit()
{
    gx = gy = gz = gbase = 0;
	gameObject->transform->SetPosition(0,0,0);
}

void CWidget_Transform::OnStart()
{
	BaseState* editorState = GSM::getInstance()->getStateAtIndex(0);
	{
		gx = ObjectFactory::getInstance()->MakeNewGameObject(std::string("WX"), editorState);
		Widget_TransformAxis* tx = new Widget_TransformAxis(Widget_TransformAxis::X, Widget_TransformAxis::TRANSLATE, this);
		gx->AddComponent(tx, "Widget_TransformAxis");
		AddSubWidget(tx);
	}
	{
		gy = ObjectFactory::getInstance()->MakeNewGameObject(std::string("WY"), editorState);
		Widget_TransformAxis* ty = new Widget_TransformAxis(Widget_TransformAxis::Y, Widget_TransformAxis::TRANSLATE, this);
		gy->AddComponent(ty, "Widget_TransformAxis");
		AddSubWidget(ty);
	}
	{
		gz = ObjectFactory::getInstance()->MakeNewGameObject(std::string("WZ"), editorState);
		Widget_TransformAxis* tz = new Widget_TransformAxis(Widget_TransformAxis::Z, Widget_TransformAxis::TRANSLATE, this);
		gz->AddComponent(tz, "Widget_TransformAxis");
		AddSubWidget(tz);
	}
    {
        gbase = ObjectFactory::getInstance()->MakeNewGameObject(std::string("BASE"), editorState);
        Widget_TransformAxis* tbase = new Widget_TransformAxis(Widget_TransformAxis::BASE, Widget_TransformAxis::TRANSLATE, this);
        gbase->AddComponent(tbase, "Widget_TransformAxis");
        gbase->transform->SetScale(0.5f, 0.5f, 0.5f);
        AddSubWidget(tbase);
    }
}

void CWidget_Transform::OnUpdate()
{
	if (!selectedGameObjects.empty())
	{
		//move widget to obj
		if (lastAttachedObjPos != selectedGameObjects[0].first->transform->position())
		{
			lastAttachedObjPos = selectedGameObjects[0].first->transform->position();
			gameObject->transform->SetPosition(lastAttachedObjPos);
		}
	}

    if (Input::GetTriggered(0, "Escape"))
    {
        GlobalClearAttachedObjects();
    }
}

void CWidget_Transform::OnDraw()
{
}

void CWidget_Transform::OnFree()
{
	CWidget::OnFree();
}

void CWidget_Transform::OnMouseClick(CCamera* cam)
{
}

void CWidget_Transform::SetVisible( bool v )
{
	if (gx)
        gx->GetComponent<CDiffuseRenderer>()->setEnabled(v);
	if (gy)
        gy->GetComponent<CDiffuseRenderer>()->setEnabled(v);
	if (gz)
        gz->GetComponent<CDiffuseRenderer>()->setEnabled(v);
    if (gbase)
        gbase->GetComponent<CDiffuseRenderer>()->setEnabled(v);
}

bool CWidget_Transform::GetIsDragging()
{
	if (gx && gx->GetComponent<Widget_TransformAxis>()->IsDragging())
		return true;
	if (gy && gy->GetComponent<Widget_TransformAxis>()->IsDragging())
		return true;
	if (gz && gz->GetComponent<Widget_TransformAxis>()->IsDragging())
		return true;
    if (gbase && gbase->GetComponent<Widget_TransformAxis>()->IsDragging())
        return true;
	return false;
}

void CWidget_Transform::SetAttachedObjects(std::vector<GameObject*>& gobjptrs)
{
	ClearAttachedObjects();
	if (gobjptrs.size() == 0)
		return;
	for (auto g : gobjptrs)
	{
		GameObjectTransformPack p;
		p.position = g->transform->GetGlobalPosition();
    p.rotation = g->transform->GetRotation();
		p.scale = g->transform->GetScale();

		selectedGameObjects.push_back(std::pair<GameObject*, CWidget_Transform::GameObjectTransformPack>(g, p));
	}
	SetVisible(true);
	gameObject->transform->SetPosition(selectedGameObjects[0].first->transform->position());
	gx->GetComponent<Widget_TransformAxis>()->Rescale(selectedGameObjects[0].first);
	gy->GetComponent<Widget_TransformAxis>()->Rescale(selectedGameObjects[0].first);
	gz->GetComponent<Widget_TransformAxis>()->Rescale(selectedGameObjects[0].first);
    gbase->GetComponent<Widget_TransformAxis>()->Rescale(selectedGameObjects[0].first);
}

void CWidget_Transform::ClearAttachedObjects()
{

	selectedGameObjects.clear();
	SetVisible(false);
}
void CWidget_Transform::GlobalClearAttachedObjects()
{
    for (auto &it : selectedGameObjects)
    {
        it.first->mIsSelected = false;
    }

    selectedGameObjects.clear();
    SetVisible(false);
}

std::vector<std::pair<GameObject*, CWidget_Transform::GameObjectTransformPack>>& CWidget_Transform::GetAttachedObjects()
{
	return selectedGameObjects;
}

void CWidget_Transform::SetLocalSpaceMode()
{
	gx->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::LOCAL);
	gy->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::LOCAL);
	gz->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::LOCAL);
    gbase->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::LOCAL);
}

void CWidget_Transform::SetGlobalSpaceMode()
{
	gx->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::GLOBAL);
	gy->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::GLOBAL);
	gz->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::GLOBAL);
    gbase->GetComponent<Widget_TransformAxis>()->SetSpace(Widget_TransformAxis::GLOBAL);
}

meta_define(CWidget_Transform);

