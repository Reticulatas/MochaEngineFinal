#include "MochaShader.hlsl"

// Vertex Shader
VertexOutLightStage VS(VertexIn vin)
{
  VertexOutLightStage vout;

  // Transform to homogeneous clip space.
  vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

  // Calculate the view space vertex position
  // gWorld = World * View matrix here - Point/Spotlights
  // gWorld = InvProj - Directional
  vout.PosV = mul(float4(vin.PosL, 1.0f), gWorld);

  // Use to calculate tex coord
  vout.PosP = vout.PosH;
  
  // Just pass vertex color into the pixel shader.
  vout.Color = vin.Color;

  vout.UV = vin.UV;
  
  return vout;
}