#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
  // Look up the original image color.
 float BloomThreshold = gData1.z;
 float4 c = origTexture.Sample(samAnisotropic, pin.UV);
 c.a = 1;
 
 // Adjust it to keep only values brighter than the specified threshold.
 return saturate((c - BloomThreshold) / (1 - BloomThreshold));
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
