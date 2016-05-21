/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#pragma once

#ifndef NOPCH

//#include <ft2build.h>
//#include FT_FREETYPE_H

//non changing files only
#include "CTransform.h"
#include "Engine.h"
#include "MathIncludes.h"
#include "GraphicsInclude.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "InputLayout.h"
#include "ConstantBuffer.h"
#include "StringUtils.h"
#include "GSM.h"
#include "Material.h"
#include "BaseState.h"
#include "BlankState.h"
#include "Input.h"
#include "Console.h"
#include "Log.h"
#include "FrameController.h"
#include "SubShader.h"
#include "Shader.h"
#include "Task.h"
#include "MochaString.h"
#include "StateSerializer.h"
#include "MathIncludes.h"
#include "CTemplate.h"
#include "Color.h"
#include "Vertex.h"
#include "D3DUtils.h"
#include "GenericUtils.h"
#include "ThreadUtils.h"
#include "UniqueFileID.h"
#include "Timer.h"
#include "MemDebug.h"
#include "MochaException.h"
#include "Barrista.h"
#include "IBarristaOperation.h"
#include "Config.h"
#include "AudioExt.h"
#include "MathUtilities.h"
#include "Asset.h"
#include "AssetEnumerator.h"
#include "AnimationSubSystem.h"
#include "AssetHandle.h"
#include "InputManager.h"
#include "IZone.h"
#endif

//system files
#include <string>
#include <iostream>
#include <list>
#include <vector>

//thanks cryengine
#define DISTANCE_TO_THE_MOON 238855
#define WIN32_LEAN_AND_MEAN
//thanks unreal
void MAssert(bool cond, std::string msg);
//thanks unity
//for not giving us the source so we can see how bad they abuse the pch