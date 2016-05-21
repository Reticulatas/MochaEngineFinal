/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------*/

#pragma once
#include "Color.h"
 
#define MAXBONES 4

struct Vertex
{
  Vertex(void);
  Vertex(Vector3 Position, Vector2 TextureCoords = Vector2(0, 0), Vector3 normal = Vector3(0, 0, 0), Color Color = COLOR::WHITE);
  Vertex(Vector3 Position, Color Color = COLOR::WHITE);	

  Vector3 mPosition;			// Vertex coordinates
	Color mColor;				    // Color
  Vector2 mTextureCoords;	// u,v coordinates for texture
  Vector3 mNormal;			  // normal
  Vector3 tangent;
  Vector3 binormal;


	___SERIALIZE_BEGIN___
	___DEFSER(mPosition, 1)
	___DEFSER(mColor, 1)
	___DEFSER(mTextureCoords, 1)
	___DEFSER(mNormal, 1)
	___SERIALIZE_END___
};
//___SERIALIZE_CLASS(Vertex, 1);

struct SkinnedVertex
{
  Vector3 mPosition;			// Vertex coordinates
  Color mColor;				    // Color
  Vector2 mTextureCoords;	// u,v coordinates for texture
  Vector3 mNormal;			  // normal
  Vector3 mTangent;
  Vector3 mBinormal;
  Vector4 mBlendWeights;
  unsigned mBlendIndices[MAXBONES];

  SkinnedVertex();
};

struct LineVertex
{
  Vector3 pos;
  Color color;

  LineVertex(){}
  LineVertex(const Vector3 &point, const Color& icolor)
    : pos(point), color(icolor) {}
};
/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change