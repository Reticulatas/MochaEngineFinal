/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 

Vertex::Vertex()
{

}
Vertex::Vertex(Vector3 Position, Vector2 TextureCoords, Vector3 normal, Color Color)
{
	mPosition = Position;
	mColor = Color;
	mTextureCoords = TextureCoords;
	mNormal = normal;
}

Vertex::Vertex(Vector3 Position, Color Color )
{
  mPosition = Position;
  mColor = Color;
}

/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change

SkinnedVertex::SkinnedVertex()
{
  for (unsigned i = 0; i < MAXBONES; ++i)
    mBlendIndices[i] = 0;
}
