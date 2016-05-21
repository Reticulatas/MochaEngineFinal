#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

M_TEXTURE2D noiseTexture : register(t1);

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
  float gGlobalTime = gData1.x;
  float gBlockThreshold = gData1.y;
  float gLineThreshold = gData1.z;
  float2 pixelUV = float2(gData2.x, gData2.y) * pin.UV;


  float2 block = floor(pixelUV / float2(16, 16));
  float2 uv_noise = block / float2(64,64);
  uv_noise += floor(float2(gGlobalTime, gGlobalTime) * float2(1234.0, 3543.0)) / float2(64, 64);

  float block_thresh = pow(frac(gGlobalTime * 1236.0453), 2.0) * gBlockThreshold;
  float line_thresh = pow(frac(gGlobalTime * 2236.0453), 3.0) * gLineThreshold;

  float2 uv_r = pin.UV, uv_g = pin.UV, uv_b = pin.UV;

  float3 blockTexture = noiseTexture.Sample(samAnisotropic, uv_noise).rgb;
  float3 lineTexture = noiseTexture.Sample(samAnisotropic, float2(uv_noise.y, 0)).rgb;

  // glitch some blocks and lines
  if (blockTexture.r < block_thresh || lineTexture.g < line_thresh)
  {
    float2 dist = (frac(uv_noise) - 0.5) * 0.3;
    uv_r += dist * 0.1;
    uv_g += dist * 0.2;
    uv_b += dist * 0.125;
  }

  float4 color;
  color.r = origTexture.Sample(samAnisotropic, uv_r).r;
  color.g = origTexture.Sample(samAnisotropic, uv_g).g;
  color.b = origTexture.Sample(samAnisotropic, uv_b).b;
  color.a = 1;

  // loose luma for some blocks
  if (blockTexture.g < block_thresh)
    color.rgb = color.ggg;

  // discolor block lines
  if (lineTexture.b * 3.5 < line_thresh)
    color.rgb = float3(0.0, dot(color.rgb, float3(1,1,1)), 0.0);

  // interleave lines in some blocks
  if (blockTexture.g * 1.5 < block_thresh || lineTexture.g * 2.5 < line_thresh)
  {
    float linef = frac(pixelUV.y / 3.0);
    float3 mask = float3(3.0, 0.0, 0.0);
      if (linef > 0.333)
        mask = float3(0.0, 3.0, 0.0);
    if (linef > 0.666)
      mask = float3(0.0, 0.0, 3.0);

    color.xyz *= mask;
  }

  return color;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
