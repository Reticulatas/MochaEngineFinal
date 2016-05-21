#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

float c_rg_to_rb_ratio = 0.522;

texture2D DistortMapTextureSampler : register(t1);

float4 PS(VS_Output pin) : SV_Target
{
  float2 vOriginal = pin.UV;

  float4 vRead = DistortMapTextureSampler.Sample(clampAnisotropic, vOriginal);

	float2 vGreen;
	vGreen.r = ( vRead.x + vRead.z ) / 2;
	vGreen.g = ( vRead.y + vRead.w ) / 2;

	float4 vFinal;
  vFinal.r = origTexture.Sample(clampAnisotropic, vRead.xy).r;
  vFinal.ga = origTexture.Sample(clampAnisotropic, vGreen).ga;
  vFinal.b = origTexture.Sample(clampAnisotropic, vRead.zw).b;
	
	float fBoundsCheck = saturate( dot( (vGreen.xy < float2(0.05,0.05)), float2(1,1)) + dot( (vGreen.xy > float2(0.95,0.95)), float2(1,1)) );
	vFinal.xyz = lerp( vFinal.xyz, float3(0,0,0), fBoundsCheck );

  return vFinal;
}

