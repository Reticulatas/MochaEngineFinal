cbuffer cbPerCamera : register(b0)
{
  matrix gWorldViewProj;
};

// Input
struct VertexIn
{
  float3 PosL      : POSITION;
  float4 Color     : COLOR;
};

// Output
struct VertexOut
{
  float4 PosH		   : SV_POSITION;
  float4 Color		 : COLOR;
};

// Vertex Shader
VertexOut VS(VertexIn vin)
{
  VertexOut vout;
  
  // Transform to homogeneous clip space.
  vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
  // Just pass vertex color into the pixel shader.
  vout.Color = vin.Color;

  return vout;
}

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change
