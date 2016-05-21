%{
#include "Vertex.h"
%}

struct Vertex
{
	Vertex();
	~Vertex();
public:
	Vector3 mPosition;			// Vertex coordinates
	Color mColor;				    // Color
	Vector2 mTextureCoords;	// u,v coordinates for texture
	Vector3 mNormal;			  // normal
	Vector3 tangent;
	Vector3 binormal;
};

struct SkinnedVertex
{
	SkinnedVertex();
	~SkinnedVertex();
public:
	Vector3 mPosition;			// Vertex coordinates
	Color mColor;				    // Color
	Vector2 mTextureCoords;	// u,v coordinates for texture
	Vector3 mNormal;			  // normal
	Vector3 mTangent;
	Vector3 mBinormal;
	Vector4 mBlendWeights;
};