#include "stdafx.h"
#include "EditorSubsystem_WidgetManager.h"
#include "ObjectFactory.h"
#include "EditorSubsystem.h"
#include "CForwardRenderer.h"
#include "CDiffuseRenderer.h"

void EditorSubsystemTools::WidgetManager::RegisterWidget(GameObject* _attachedObj, WidgetType _type)
{
	if (_attachedObj->GetState() == dynamic_cast<BaseState*>(EditorSubsystem::getInstance()->GetEditorState()))
		return;	//skip objects in editor state

	widgets.emplace_back();
	widgets.back().MakeWidget(_attachedObj, _type);
}

void EditorSubsystemTools::WidgetManager::DeRegisterWidget(GameObject* _attachedObj)
{
	if (_attachedObj->GetState() == dynamic_cast<BaseState*>(EditorSubsystem::getInstance()->GetEditorState()))
		return;

	for (Widget& widg : widgets)
	{
		if (widg.attachedObj == _attachedObj)
		{
			widg.Destroy();
			return;
		}
	}
}

void EditorSubsystemTools::WidgetManager::Tick()
{
	//deferred deletes
	for (Widget* delwidg : widgetsToDelete)
	{
		::Destroy(delwidg->widgetObj);
		delwidg->widgetObj = 0;
		widgets.remove(*delwidg);
	}
	widgetsToDelete.clear();

	//updates
	for (Widget& widg : widgets)
	{
		if (Input::GetHeld(0, "Widget_Toggle") > 0.0f)
			widg.widgetObj->GetComponent<CDiffuseRenderer>()->setEnabled(false);
		else
			widg.widgetObj->GetComponent<CDiffuseRenderer>()->setEnabled(true);

		widg.Tick();
	}
}

GameObject* EditorSubsystemTools::WidgetManager::GetAttachedObjFromWidgetObj(GameObject* widgetObj)
{
	if (widgetObj == 0)
		return 0;

	for (Widget& widg : widgets)
	{
		if (widg.widgetObj == widgetObj)
			return widg.attachedObj;
	}
	return 0;
}

void EditorSubsystemTools::WidgetManager::Widget::MakeWidget(GameObject* _attacheObj, WidgetType _type)
{
	this->attachedObj = _attacheObj;
	this->_type = _type;
	widgetObj = ObjectFactory::getInstance()->MakeNewGameObject(_attacheObj->GetName() + "Widget", EditorSubsystem::getInstance()->GetEditorState());
	CDiffuseRenderer* rend = widgetObj->AddComponent<CDiffuseRenderer>();
	rend->mDrawOnTop = true;
	rend->SetMesh(AssetManager::getInstance()->GetAsset("Quad.bean"));
	rend->gameObject->SetFlag("ArtWidget");
	switch (_type)
	{
	case CAMERA:
		rend->mMaterial.SetTexture("Widget_Camera.png");
		break;
	case LIGHT:
		rend->mMaterial.SetTexture("Widget_Light.png");
		break;
	case SOUND:
		rend->mMaterial.SetTexture("Widget_Sound.png");
		break;
	}
}

void EditorSubsystemTools::WidgetManager::Widget::Tick()
{
	widgetObj->transform->SetPosition(attachedObj->transform->position());
	widgetObj->transform->LookAt(&EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->position());
	float distance = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition().Distance(&widgetObj->transform->position());
	float distscalefactor = Math::Clamp(distance / 10.0f, 0.5f, 1000.0f);
	widgetObj->transform->scale.Set(distscalefactor, distscalefactor, distscalefactor);
}

bool EditorSubsystemTools::WidgetManager::Widget::operator==(const Widget& other) const
{
	return (attachedObj == other.attachedObj && widgetObj == other.widgetObj && _type == other._type);
}

void EditorSubsystemTools::WidgetManager::Widget::Destroy()
{
	EditorSubsystem::getInstance()->widgetManager.widgetsToDelete.push_back(this);
	attachedObj = 0;
}
