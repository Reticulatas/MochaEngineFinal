#include "MochaShader.hlsl"

M_TEXTURE2D normal		: register(t0);
M_TEXTURE2D albedo		: register(t1);
//M_TEXTURE2D specular	: register(t2);
M_TEXTURE2D depth		  : register(t2);
M_TEXTURE3D EnvMap		: register(t3);
M_TEXTURE3D ShadowMap : register(t4);

cbuffer cbReflectionProbe : register(b4)
{
	float4 gProbePos;
	float3 gProbeDim;
	float  isRenderingProbe;
};

// Pixel Shader
float4 PS(VertexOutLightStage pin) : SV_Target
{
  // Unproject position to use as texCoords
  float2 texCoords = pin.PosP.xy / pin.PosP.w;
  texCoords.y *= -1;
  texCoords = texCoords * .5f + .5f;  

  // Get Position from depth
  // Clamp the view space position to the plane at Z = 1
  float3 viewRay = float3(pin.PosV.xy / pin.PosV.z, 1.0f);
  float depthVal = depth.Sample(pointSampler, texCoords).r;
  float linearDepth = gPlanes.y / (depthVal - gPlanes.x);
  float3 posVS = viewRay * linearDepth;
  float3 posWS = mul(float4(posVS, 1), gInvView).xyz;

  float4 diffuse = albedo.Sample(pointSampler, texCoords);
  //diffuse = GammaCorrectedToLinear(diffuse);
  // Move back range to [-1, 1]
  float3 bumpNormal = normal.Sample(pointSampler, texCoords).xyz * 2.f - 1.f;
  //float4 spec = specular.Sample(pointSampler, texCoords);
  float3 toEyeW = normalize(gEyePosW - posWS);

  Material gMaterial;
  gMaterial.Ambient = float4(0, 0, 0, 0);
  gMaterial.Diffuse = float4(diffuse.rgb, 1);
  //gMaterial.Specular = spec;

  //gMaterial.Diffuse = GammaCorrectedToLinear(gMaterial.Diffuse);
  gMaterial.Specular = float4(0, 0, 0, 0);
 // gMaterial.Specular = pow(gMaterial.Specular, 1/2.2f);

  float4 texColor = float4(0, 0, 0, 0);

 // roughness is now in the alpha channel of the albedo Gbuffer
  float roughness = diffuse.a;

  float metallic = normal.Sample(pointSampler, texCoords).a;
 // metallic = pow(metallic, 2.2f);

  float mipIndex = roughness * roughness * 8.0f;

  // Compute shadow factor
  float shadowFactor = GetOmniDirectionShadowFactor(posWS - gLight.Position, ShadowMap, gShadowBuffer, gIsRenderingShadow, gUseSoftShadow);

  // Compute reflection
  //float3 reflectVector = reflect(-toEyeW, bumpNormal);
  float3 reflectVector = BPCEM(gEyePosW, bumpNormal, posWS, gProbePos.xyz, gProbeDim);

  float4 envColor = EnvMap.SampleLevel(samAnisotropic, reflectVector, mipIndex);

  if (gLight.Type == DIRECTIONAL)
  {
    //shadowFactor = GetOmniDirectionShadowFactor(gLight.Direction, ShadowMap, gShadowBuffer, gIsRenderingShadow);
    texColor += ComputeDirLightPBR(gMaterial, gLight, posWS, bumpNormal, toEyeW, envColor.rgb, roughness, metallic);
  }
  
  // Point light
  else if (gLight.Type == POINT)
    texColor += ComputePointLightPBR(gMaterial, gLight, posWS, bumpNormal, toEyeW, envColor.rgb, roughness, metallic); 

  // SpotLight
  else if (gLight.Type == SPOTLIGHT)
    texColor += ComputeSpotLightPBR(gMaterial, gLight, posWS, bumpNormal, toEyeW, envColor.rgb, roughness, metallic);
  
  texColor = float4(gIntensity * shadowFactor * texColor.xyz, 1);

  //return GammaCorrection(texColor);
  //return envColor;
   return texColor;
}