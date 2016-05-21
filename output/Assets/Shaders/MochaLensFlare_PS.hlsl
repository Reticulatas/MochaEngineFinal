#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

M_TEXTURE2D dirtyLensMaskTexture : register(t1);

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{

 float uGhostDispersal = gData1.z;
 int uGhosts = gData1.w;

 float2 texcoord = -pin.UV + float2(1.0f, 1.0f);
 float2 texelSize = 1.0f / gData1.xy;

 // ghost vector to image centre:
 float2 ghostVec = (float2(0.5f, 0.5f) - texcoord) * uGhostDispersal;

 // sample ghosts:  
 float4 result = float4(0.0, 0.0, 0.0, 0.0);
 //float4 dirtyLensMask = dirtyLensMaskTexture.Sample(samAnisotropic, pin.UV);

 for (int i = 0; i < uGhosts; ++i)
 {
	 float2 offset = frac(texcoord + ghostVec * float(i));

	 float weight = length(float2(0.5, 0.5) - offset) / length(float2(0.5, 0.5));
	 weight = pow(1.0 - weight, 10.0);

	 result += origTexture.Sample(samAnisotropic, offset) * weight * gData2.x;
 }

 return result;
}

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change
