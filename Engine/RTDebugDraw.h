#pragma once
#include "GraphicsInclude.h"
#include "MathIncludes.h"

using namespace Math;
// Render Target Debug Window
class RTDebug
{
private:
  struct VertexType
  {
    Vector3 position;
    Vector2 texture;
  };

public:
  RTDebug();
  RTDebug(const RTDebug&);
  ~RTDebug();

  bool Initialize(ID3D11Device*, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight);
  void Shutdown();
  bool Render(ID3D11DeviceContext*, int positionX, int positionY);

  int GetIndexCount();

private:
  bool InitializeBuffers(ID3D11Device*);
  void ShutdownBuffers();
  bool UpdateBuffers(ID3D11DeviceContext*, int positionX, int positionY);
  void RenderBuffers(ID3D11DeviceContext*);

private:
  ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
  int m_vertexCount, m_indexCount;
  int m_screenWidth, m_screenHeight;
  int m_bitmapWidth, m_bitmapHeight;
  int m_previousPosX, m_previousPosY;
};