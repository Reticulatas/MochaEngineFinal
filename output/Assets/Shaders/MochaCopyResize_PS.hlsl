#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
  float4 color = origTexture.Sample(samAnisotropic, pin.UV);

  return color;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
