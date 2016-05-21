#include "MochaShader.hlsl"

M_TEXTURE2D diffuseTexture : register(t0);

// Pixel Shader
float4 PS(VertexOut pin) : SV_Target
{
  return diffuseTexture.Sample(clampAnisotropic, pin.UV) * pin.Color;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
