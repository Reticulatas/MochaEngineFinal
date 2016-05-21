#include "ShaderIncludes.hlsl"

cbuffer cbShadowProbe : register(b5)
{
  float4 gShadowBuffer;
  int gIsRenderingShadow;
  // Strength; // range 0 - 1.f
  // Bias;     // range 0 - 2.f
  // Near;
  // Far;
  int gUseSoftShadow;
  float2 gPad;
};

// Unprojects vector for depth comparison
float VectorToDepth(float3 Vec, float n, float f)
{
  float3 AbsVec = abs(Vec);
  float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));
  float NormZComp = (f + n) / (f - n) - (2 * f*n) / (f - n) / LocalZcomp;
  return (NormZComp + 1.0) * 0.5;
}

float GetOmniDirectionShadowFactor(float3 vLight, M_TEXTURE3D shadowMap, float4 ShadowBuffer, int isRenderingShadow, int useSoftShadow)
{
  // Not rendering shadow, just return 1;
  if (isRenderingShadow == 0)
    return 1.f;

  float Strength = saturate(ShadowBuffer.x);
  float Bias = clamp(ShadowBuffer.y, 0.f, 2.f);
  float Near = ShadowBuffer.z;
  float Far = ShadowBuffer.w;

  float shadowDepth, shadowFactor = 0;
  float depthCmp = VectorToDepth(vLight, Near, Far) - Bias;

  // Use soft shadows
  if (gUseSoftShadow)
  {
    // Percentage close filtering 16x16 kernel
    float kernelSize = 8;
    float kernel = 1.f / (kernelSize * kernelSize);
    float kernelExtent = (kernelSize * .5f - .5f) * .01f;
    for (float y = -kernelExtent; y <= kernelExtent; y += .01f)
    {
      for (float x = -kernelExtent; x <= kernelExtent; x += .01f)
      {
        float2 texoffset = float2(x, y);
        float3 lightvec = vLight;
        lightvec.xy += texoffset;

        shadowDepth = shadowMap.SampleLevel(samAnisotropic, lightvec, 0).r;
        if (depthCmp - shadowDepth < 0.f)
          shadowFactor += kernel;
      }
    }
  }

  // Use hard shadows
  else
  {
    shadowDepth = shadowMap.SampleLevel(samAnisotropic, vLight, 0).r;
    shadowFactor = 1.f - Strength;
    if (depthCmp - shadowDepth < 0.f)
      shadowFactor = 1;
  }

  return shadowFactor;
}
