#include "stdafx.h"
#include "CPostProcess.h"
#include "ConstantBuffer.h"

CPostProcess::CPostProcess() : Component()
{
}

CPostProcess::~CPostProcess()
{

}

void CPostProcess::OnFree(void)
{
}

void CPostProcess::OnInit(void)
{

}

void CPostProcess::OnStart(void)
{
  Component::OnStart();
}

void CPostProcess::OnUpdate(void)
{

}

meta_define(CPostProcess);
