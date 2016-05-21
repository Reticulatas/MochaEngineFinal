#include "MochaShader.hlsl"

cbuffer cbPerMat : register(b3)
{
  float4 gTintColor;
  matrix gUVTransform;
};

cbuffer cbPerFrame : register(b4)
{
  matrix animMatrix[80];
};

// Input
struct SkinnedVertexIn
{
  float3 PosL         : POSITION;
  float4 Color        : COLOR;
  float2 UV 	        : TEXCOORD_ONE;
  float3 NormalL      : NORMAL;
  float3 TangentL     : TANGENT;
  float3 BinormalL    : BINORMAL;
  float4 BlendWeights : BLENDWEIGHTS;
  uint4  BlendIndices : BLENDINDICES;
};

// Vertex Shader
VertexOut VS(SkinnedVertexIn vin)
{
  VertexOut vout;

  //loat4 pos = float4(vin.PosL, 1.0f);
  float4 pos = float4(0, 0, 0, 0);
  float4 normal = float4(0, 0, 0, 0);
  float4 tangent = float4(0, 0, 0, 0);
  float4 binormal = float4(0, 0, 0, 0);
  
  for (int i = 0; i < 4; ++i)
  {
    pos += mul(float4(vin.PosL, 1.0f), animMatrix[vin.BlendIndices[i]]) * vin.BlendWeights[i];
    normal += mul(float4(vin.NormalL, 0.0f), animMatrix[vin.BlendIndices[i]]) * vin.BlendWeights[i];
    tangent += mul(float4(vin.TangentL, 0.0f), animMatrix[vin.BlendIndices[i]]) * vin.BlendWeights[i];
    binormal += mul(float4(vin.BinormalL, 0.0f), animMatrix[vin.BlendIndices[i]]) * vin.BlendWeights[i];
  }
  normal = normalize(normal);

  // Transform to world space space.
  vout.PosW = mul(pos, gWorld).xyz;
  vout.NormalW = mul(normal.xyz, (float3x3)gWorldInvTranspose);
  vout.TangentW = mul(tangent.xyz, (float3x3)gWorldInvTranspose);
  vout.BinormalW = mul(binormal.xyz, (float3x3)gWorldInvTranspose);

  // Modify range to 0,1 for better inteporlation
  vout.NormalW = normalize(vout.NormalW);
  vout.NormalW = vout.NormalW * .5f + .5f;
  vout.TangentW = normalize(vout.TangentW);
  vout.TangentW = vout.TangentW * .5f + .5f;
  vout.BinormalW = normalize(vout.BinormalW);
  vout.BinormalW = vout.BinormalW * .5f + .5f;

  // Transform to homogeneous clip space.
  vout.PosH = mul(pos, gWorldViewProj);

  // Tranform prev wvp
  vout.CurrPosH = vout.PosH;
  vout.PrevPosH = mul(pos, gPrevWorldViewProj);

  // Tintcolor + uv offsets.
  vout.Color = gTintColor;
  vout.UV = mul(float4(vin.UV, 0.0f, 1.0f), gUVTransform).xy;

  return vout;
}