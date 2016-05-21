#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

M_TEXTURE2D velocityTexture : register(t1);

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
	float4 mask = velocityTexture.Sample(pointSampler, pin.UV);
  float4 color = origTexture.Sample(samAnisotropic, pin.UV);

  if (mask.z > 0)
  {
    float pixelBlur = 30.0f;
	float2 texDim = gData1.xy;
	float2 uv = pin.UV * texDim;

    float2 modul = uv % float2(pixelBlur, pixelBlur);
	float2 normalizedCord = float2(uv - modul) / texDim;
    color = origTexture.Sample(samAnisotropic, normalizedCord);
  }
  
  return color;
}



/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
