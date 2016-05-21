#pragma  once

#include "Mesh.h"

#define MAXLINES 50000

class LineMesh : public Mesh
{
public:

  LineMesh(std::string Name = "", bool isDynamic = false);
  ~LineMesh();

  virtual void BindMesh(ID3D11DeviceContext* context);
  virtual void DrawMesh(ID3D11DeviceContext* context);		
  virtual bool GenerateBuffers(ID3D11Device*  device);
  virtual void UpdateBuffer(ID3D11Device* device);  

  void AddLineVertex(const LineVertex& v);
  std::vector<LineVertex>& GetLineVertices();
  unsigned GetLineCount() const;

private:

  std::vector<LineVertex> mLineVertices;
};