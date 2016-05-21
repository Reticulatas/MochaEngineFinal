#include "stdafx.h"
#include "CStreamMarker.h"

void CStreamMarker::OnStart(void)
{

}

void CStreamMarker::OnInit(void)
{

}

void CStreamMarker::OnUpdate(void)
{

}

void CStreamMarker::OnAlwaysUpdate(void)
{

}

void CStreamMarker::OnFree(void)
{

}

void CStreamMarker::OnDraw(void)
{

}

void CStreamMarker::OnEditorUpdate(void)
{
	Renderer::getInstance()->DrawBoxPerma(gameObject->transform->GetGlobalPosition(), Vector3(0.5f, 0.5f, 0.5f), COLOR::YELLOW);
}

___COMP_REGISTER(CStreamMarker);
meta_define(CStreamMarker);

//Nicholas Fuller