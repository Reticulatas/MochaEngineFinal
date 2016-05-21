#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
	float2 texDim = gData1.xy;
	float gGlobalTime = gData1.z;

	float2 uv = pin.UV;

	float amount = 0.0;

	amount = (1.0 + sin(gGlobalTime*6.0)) * 0.5;
	amount *= 1.0 + sin(gGlobalTime*16.0) * 0.5;
	amount *= 1.0 + sin(gGlobalTime*19.0) * 0.5;
	amount *= 1.0 + sin(gGlobalTime*27.0) * 0.5;
	amount = pow(amount, 3.0);

	amount *= 0.05;

	float3 col;
	col.r = origTexture.Sample(samAnisotropic, float2(uv.x + amount, uv.y)).r;
	col.g = origTexture.Sample(samAnisotropic, uv).g;
	col.b = origTexture.Sample(samAnisotropic, float2(uv.x - amount, uv.y)).b;

	col *= (1.0 - amount * 0.5);

	return float4(col, 1.0);
}

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change
