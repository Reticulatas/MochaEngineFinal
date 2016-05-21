#include "MochaShader.hlsl"

M_TEXTURE2D diffuseTexture : register(t0);
M_TEXTURE2D normalTexture  : register(t1);
M_TEXTURE2D specTexture    : register(t2);
M_TEXTURE2D roughnessTexture : register(t3);
M_TEXTURE2D metallicTexture : register(t4);
M_TEXTURE3D EnvMap         : register(t5);

static const int maxLights = 50;

cbuffer cbForwardLight : register(b2)
{
  Light  gFLights[maxLights];
  float3 gFEyePosW;
  int    gNumLights;
};

cbuffer cbReflectionProbe : register(b4)
{
	float4 gProbePos;
	float3 gProbeDim;
	float  isRenderingProbe;
};

// Pixel Shader
float4 PS(VertexOut pin) : SV_Target
{
  // Interpolating normal can unnormalize it, so normalize it.
  pin.NormalW = (pin.NormalW - 0.5f) * 2.0f;
  pin.TangentW = (pin.TangentW - 0.5f) * 2.0f;
  pin.BinormalW = (pin.BinormalW - 0.5f) * 2.0f;

  pin.NormalW = normalize(pin.NormalW);
  pin.TangentW = normalize(pin.TangentW);
  pin.BinormalW = normalize(pin.BinormalW);

  // Bump Mapping here
  float3 normal = normalTexture.Sample(samAnisotropic, pin.UV).xyz;
  float3 bumpNormal = normal * 2.f - 1.f;
  bumpNormal = (bumpNormal.x * pin.TangentW) + (bumpNormal.y * pin.BinormalW) + (bumpNormal.z * pin.NormalW);
  bumpNormal = normalize(bumpNormal);

  int normalCheck = length(normal);
  if (normalCheck == 0)
	  bumpNormal = pin.NormalW;
  bumpNormal *= gBumpFactor;

  float3 toEyeW = normalize(gFEyePosW - pin.PosW);

  Material gMaterial;
  gMaterial.Ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
  gMaterial.Diffuse = diffuseTexture.Sample(samAnisotropic, pin.UV);
  gMaterial.Diffuse = GammaCorrectedToLinear(gMaterial.Diffuse);
  gMaterial.Specular = float4(0,0,0,0);

  float4 texColor = float4(0, 0, 0, 0);

  float roughness = roughnessTexture.Sample(samAnisotropic, pin.UV).r * gRoughnessFactor;
  float metallic = metallicTexture.Sample(samAnisotropic, pin.UV).r * gMetallicFactor;

  float mipIndex = roughness * roughness * 8.0f;

  // Compute reflection
 // float3 reflectVector = reflect(-toEyeW, bumpNormal);
  float3 reflectVector = BPCEM(gFEyePosW, bumpNormal, pin.PosW, gProbePos.xyz, gProbeDim);
  float4 envColor = EnvMap.SampleLevel(samAnisotropic, reflectVector, mipIndex);
  //envColor = float4(0, 0, 0, 0);

  //[unroll]
  for (int i = 0; i < gNumLights; ++i)
  {
    // Directional light
    if (gFLights[i].Type == DIRECTIONAL)
      texColor += ComputeDirLightPBR(gMaterial, gFLights[i], pin.PosW, bumpNormal, toEyeW, envColor.rgb, roughness, metallic);
   
    // Point light
    else if (gFLights[i].Type == POINT)
      texColor += ComputePointLightPBR(gMaterial, gFLights[i], pin.PosW, bumpNormal, toEyeW, envColor.rgb, roughness, metallic);

    // SpotLight
    else if (gFLights[i].Type == SPOTLIGHT)
      texColor += ComputeSpotLight(gMaterial, gFLights[i], pin.PosW, bumpNormal, toEyeW);
  }  

  texColor.a = gMaterial.Diffuse.a;

  float3 emissive = gGlobalAmbient.xyz + gEmissiveColor.xyz * gMaterial.Diffuse.xyz;
  return texColor * pin.Color + float4(emissive, 0.f);
  //return gMaterial.Diffuse + pin.Color;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
