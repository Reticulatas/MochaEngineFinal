#include "stdafx.h"
#include "IZone.h"

void IZone::DrawBounds(void)
{
	Renderer::getInstance()->DrawBoxPerma(GetCenter(), Vector3(width*0.5f, height*0.5f, depth*0.5f), COLOR::CORNFLOWER_BLUE);
}

bool IZone::IsPointWithin(Math::Vector3 point)
{
	if (point.x > GetLeftBound() && point.x < GetRightBound())
		if (point.y > GetBottomBound() && point.y < GetTopBound())
			if (point.z > GetFrontBound() && point.z < GetBackBound())
				return true;
	return false;
}

Math::Vector3 IZone::GetDiagonalCornerOne()
{
	return Math::Vector3(GetRightBound(), GetTopBound(), GetBackBound());
}

Math::Vector3 IZone::GetDiagonalCornerZero()
{
	return Math::Vector3(GetLeftBound(), GetBottomBound(), GetFrontBound());
}

float IZone::GetBackBound()
{
	return GetCenter().z + depth * 0.5f;
}

float IZone::GetFrontBound()
{
	return GetCenter().z - depth * 0.5f;
}

float IZone::GetTopBound()
{
	return GetCenter().y + height * 0.5f;
}

float IZone::GetBottomBound()
{
	return GetCenter().y - height * 0.5f;
}

float IZone::GetRightBound()
{
	return GetCenter().x + width * 0.5f;
}

float IZone::GetLeftBound()
{
	return GetCenter().x - width * 0.5f;
}

Math::Vector3 IZone::GetCenter()
{
	if (attachedObj == 0)
		throw MochaException("IZone Attached Obj is not initialized, did you forget to re-init after copy?");
	return attachedObj->transform->GetGlobalPosition();
}

void IZone::ClampObjectToBounds(GameObject* gobj)
{
	gobj->transform->SetPositionX(Math::Clamp(gobj->transform->position().x, GetLeftBound(), GetRightBound()));
	gobj->transform->SetPositionY(Math::Clamp(gobj->transform->position().y, GetBottomBound(), GetTopBound()));
	gobj->transform->SetPositionZ(Math::Clamp(gobj->transform->position().z, GetFrontBound(), GetBackBound()));
}

std::vector<GameObject*> IZone::GetObjectsInBounds()
{
	std::vector<GameObject*> objs_in_bounds;
	for (CTransform* trans : *(attachedObj->GetState()->GetListOfComponentsOfType<CTransform>()))
	{
		if (trans->GetParent() != 0)
			continue;
		if (IsPointWithin(trans->position()))
			objs_in_bounds.push_back(trans->gameObject);
	}

	return objs_in_bounds;
}

void IZone::Init(GameObject* g)
{
	attachedObj = g;
}

IZone::IZone(IZone& zone)
{
	width = zone.width;
	height = zone.height;
	depth = zone.depth;
	attachedObj = 0;
}

IZone::IZone()
{
	width = 0;
	depth = 0;
	height = 0;
	attachedObj = 0;
}

//original author: Nicholas Fuller