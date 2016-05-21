#include "stdafx.h"
#include "CMapZone.h"

void CMapZone::OnEditorUpdate(void)
{
	zone.DrawBounds();
}

void CMapZone::OnDraw(void)
{
}

void CMapZone::OnFree(void)
{
}

void CMapZone::OnAlwaysUpdate(void)
{
}

void CMapZone::OnUpdate(void)
{
}

void CMapZone::OnInit(void)
{
	id = "Zone";
}

void CMapZone::OnStart(void)
{
	zone.Init(gameObject);
}

CMapZone::CMapZone()
{

}

___COMP_REGISTER(CMapZone);
meta_define(CMapZone);

//original author: Nicholas Fuller