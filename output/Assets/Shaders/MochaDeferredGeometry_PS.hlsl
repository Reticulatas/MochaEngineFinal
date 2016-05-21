#include "MochaShader.hlsl"

M_TEXTURE2D diffuseTexture : register(t0);
M_TEXTURE2D normalTexture  : register(t1);
M_TEXTURE2D specTexture    : register(t2);
M_TEXTURE2D roughnessTexture : register(t3);
M_TEXTURE2D metallicTexture : register(t4);
M_TEXTURE3D EnvMap : register(t5);

// Pixel Shader
PixelOut PS(VertexOut pin)
{
  // Interpolating normal can unnormalize it, so normalize it. [-1, 1]
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

  bumpNormal *= gBumpFactor; // The bump factor is in the alpha channel now.

  float4 diffuse = diffuseTexture.Sample(samAnisotropic, pin.UV);
 // diffuse = GammaCorrectedToLinear(diffuse);

  // Store GBuffers
  PixelOut pixelOut;

  pixelOut.albedo = diffuse * pin.Color;
  pixelOut.albedo = GammaCorrectedToLinear(pixelOut.albedo);

  pixelOut.albedo.a = roughnessTexture.Sample(samAnisotropic, pin.UV).r * gRoughnessFactor;

  // Metallic stored in alpha of normal
  pixelOut.normal.rgb = bumpNormal.xyz * .5f + .5f;
  pixelOut.normal.a = metallicTexture.Sample(samAnisotropic, pin.UV).r * gMetallicFactor;

  // Specular needs alpha for specular power
  // Roughness stored in alpha of specular
  //pixelOut.specular = specTexture.Sample(samAnisotropic, pin.UV);
  //pixelOut.specular.a = roughnessTexture.Sample(samAnisotropic, pin.UV).r * gRoughnessFactor;

  // velocity buffer
  float2 vVelocity = (pin.CurrPosH.xy / pin.CurrPosH.w) - (pin.PrevPosH.xy / pin.PrevPosH.w);
  vVelocity *= .5f;
  vVelocity.y *= -1;
  pixelOut.velocity = float4(vVelocity, 0.0f, 1.0f); // mask z to 0 -> don't censor

  // Global Ambient
  float3 emissive = gGlobalAmbient.xyz + gEmissiveColor.xyz * diffuse.xyz;

  // Compute reflection
  //float3 toEyeW = normalize(gEyePosWorld.xyz - pin.PosW);
  //float3 reflectVector = reflect(-toEyeW, bumpNormal);
  //float4 GlobalIllumination = EnvMap.SampleLevel(samAnisotropic, reflectVector, 10);
  //GlobalIllumination = pow(GlobalIllumination, 1/2.2f);
  //GlobalIllumination *= pixelOut.albedo;
  //GlobalIllumination *= (1 - (max(0.1, pixelOut.normal.a)));

  //pixelOut.light = float4(emissive, 0) + GlobalIllumination * 0.3;
  pixelOut.light = float4(emissive, 0);

  //pixelOut.light = GammaCorrection(pixelOut.light);

  return pixelOut;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
