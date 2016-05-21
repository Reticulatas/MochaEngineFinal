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

  // Transform to world space space.
  vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
  // Transform to homogeneous clip space.
  vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

  vout.NormalW = mul(vin.NormalL, gWorldInvTranspose);
  vout.TangentW = mul(vin.TangentL, gWorldInvTranspose);
  vout.BinormalW = mul(vin.BinormalL, gWorldInvTranspose);

  // Modify range to 0,1 for better inteporlation
  vout.NormalW = normalize(vout.NormalW);
  vout.NormalW = vout.NormalW * .5f + .5f;
  vout.TangentW = normalize(vout.TangentW);
  vout.TangentW = vout.TangentW * .5f + .5f;
  vout.BinormalW = normalize(vout.BinormalW);
  vout.BinormalW = vout.BinormalW * .5f + .5f;

  vout.Color = gTintColor;
	//vout.UV = vin.UV;
  vout.UV = mul(float4(vin.UV, 0.0f, 1.0f), gUVTransform).xy;

  return vout;
}
