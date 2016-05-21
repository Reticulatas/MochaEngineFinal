#include "stdafx.h"
#include "CZone.h"

void CZone::OnEditorUpdate(void)
{
	Renderer::getInstance()->DrawLinePerma(GetDiagonalCornerZero(), GetDiagonalCornerZero() + Math::Vector3(width, 0, 0), COLOR::CORNFLOWER_BLUE);
	Renderer::getInstance()->DrawLinePerma(GetDiagonalCornerZero(), GetDiagonalCornerZero() + Math::Vector3(0, height, 0), COLOR::CORNFLOWER_BLUE);
	Renderer::getInstance()->DrawLinePerma(GetDiagonalCornerZero(), GetDiagonalCornerZero() + Math::Vector3(0, 0, depth), COLOR::CORNFLOWER_BLUE);
	Renderer::getInstance()->DrawLinePerma(GetDiagonalCornerOne(), GetDiagonalCornerOne() - Math::Vector3(width, 0, 0), COLOR::CORNFLOWER_BLUE);
	Renderer::getInstance()->DrawLinePerma(GetDiagonalCornerOne(), GetDiagonalCornerOne() - Math::Vector3(0, height, 0), COLOR::CORNFLOWER_BLUE);
	Renderer::getInstance()->DrawLinePerma(GetDiagonalCornerOne(), GetDiagonalCornerOne() - Math::Vector3(0, 0, depth), COLOR::CORNFLOWER_BLUE);
}

void CZone::OnDraw(void)
{
}

void CZone::OnFree(void)
{
}

void CZone::OnAlwaysUpdate(void)
{
}

void CZone::OnUpdate(void)
{
}

void CZone::OnInit(void)
{
}

void CZone::OnStart(void)
{
}

bool CZone::IsPointWithin(Math::Vector3 point)
{
	if (point.x > GetLeftBound() && point.x < GetRightBound())
		if (point.y > GetBottomBound() && point.y < GetTopBound())
			if (point.z > GetFrontBound() && point.z < GetBackBound())
				return true;
	return false;
}

Math::Vector3 CZone::GetDiagonalCornerOne()
{
	return Math::Vector3(GetRightBound(), GetTopBound(), GetBackBound());
}

Math::Vector3 CZone::GetDiagonalCornerZero()
{
	return Math::Vector3(GetLeftBound(), GetBottomBound(), GetFrontBound());
}

float CZone::GetBackBound()
{
	return GetCenter().z + depth * 0.5f;
}

float CZone::GetFrontBound()
{
	return GetCenter().z - depth * 0.5f;
}

float CZone::GetTopBound()
{
	return GetCenter().y + height * 0.5f;
}

float CZone::GetBottomBound()
{
	return GetCenter().y - height * 0.5f;
}

float CZone::GetRightBound()
{
	return GetCenter().x + width * 0.5f;
}

float CZone::GetLeftBound()
{
	return GetCenter().x - width * 0.5f;
}

Math::Vector3& CZone::GetCenter()
{
	return gameObject->transform->GetGlobalPosition();
}

void CZone::ClampObjectToBounds(GameObject* gobj)
{
	gobj->transform->SetPositionX(Math::Clamp(gobj->transform->position().x, GetLeftBound(), GetRightBound()));
	gobj->transform->SetPositionY(Math::Clamp(gobj->transform->position().y, GetBottomBound(), GetTopBound()));
	gobj->transform->SetPositionZ(Math::Clamp(gobj->transform->position().z, GetFrontBound(), GetBackBound()));
}

___COMP_REGISTER(CZone);
meta_define(CZone);

//original author: Nicholas Fuller
