#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

M_TEXTURE2D bloomTexture : register(t1);

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
  float4 color = origTexture.Sample(samAnisotropic, pin.UV);
  color += bloomTexture.Sample(samAnisotropic, pin.UV);

  return saturate(color);
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
