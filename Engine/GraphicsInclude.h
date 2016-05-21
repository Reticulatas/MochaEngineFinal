/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#ifdef MOCHAEDITOR
#error "Cannot include Graphics Include in Editor"
#endif

#include <Windows.h>
#include <vector>
#include "Color.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#include <D3D11.h>
#include <DXGI.h>
#include <DxErr.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>
#include <D3DX11async.h>

#pragma comment( lib, "dxguid.lib")

#if defined(_DEBUG)
#pragma comment(lib,"d3dx11d.lib")
#else
#pragma comment(lib,"d3dx11.lib")
#endif

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"DxErr.lib")
#pragma comment(lib,"d3dcompiler.lib")

#ifndef HR
        #define HR(x){HRESULT hr=x;if(FAILED(hr)){DXTraceW(__FILE__,(DWORD)__LINE__,hr,L#x,true);TRUE;}}
#endif

enum PrimitiveType {
					TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
					TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
					LINE_STRIP	= D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
                    LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
                    POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
					};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change