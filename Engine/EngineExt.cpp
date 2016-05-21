#include "stdafx.h"
#include "EngineExt.h"

bool EngineExt::IsFullScreen()
{
  return Engine::getInstance()->GetGameWnd()->IsFullScreen();
}

void EngineExt::ToggleFullScreen()
{
  Engine::getInstance()->GetGameWnd()->SetFullScreen(!Engine::getInstance()->GetGameWnd()->IsFullScreen());
}

void EngineExt::ChangeResolution(unsigned width, unsigned height)
{
  Engine::getInstance()->GetGameWnd()->FixedAspectRatio((float)width / height);
  Engine::getInstance()->GetGameWnd()->ChangeResolution(width, height);
  Engine::getInstance()->GetGameWnd()->SetFullScreen(Engine::getInstance()->GetGameWnd()->IsFullScreen());
}
