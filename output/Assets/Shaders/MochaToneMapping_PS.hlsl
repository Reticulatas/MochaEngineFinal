#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
  float4 texColor = origTexture.Sample(samAnisotropic, pin.UV);

  //texColor *= 16;
  //return GammaCorrectedToLinear(texColor);
  //return pow(texColor,1.0 / 2.0);
  texColor = GammaCorrection(texColor);
  texColor.a = dot(texColor.rgb, float3(0.299, 0.587, 0.114));
  return texColor;
}

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change
