#include "MochaShader.hlsl"

cbuffer cbPerMat : register(b3)
{
  float4 gTintColor;
  matrix gUVTransform;
};

// Vertex Shader
VertexOut VS(VertexIn vin)
{
  VertexOut vout;

  // Transform to homogeneous clip space.
  vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);  
  vout.UV = mul(float4(vin.UV, 0.0f, 1.0f), gUVTransform).xy;
  vout.Color = gTintColor;

  return vout;
}
