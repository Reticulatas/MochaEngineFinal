#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

float rand(float2 co)
{
  return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float2 outlineCurve(float2 uv) 
{
  //Set the position of the outline diagonal 
  uv = (uv - 0.5) * 2.0;
  uv *= 1.1;

  //Warps(for lack of a better term) the image on the x and y axis	
  uv.x *= 1.0 + pow((abs(uv.y) / 4.0), 2.5);
  uv.y *= 1.0 + pow((abs(uv.x) / 3.5), 2.5);

  uv = (uv / 2.0) + 0.5;
  uv = uv * 0.92 + 0.04;

  return uv;
}

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
  float2 uv = outlineCurve(pin.UV);
  float2 texDim = gData1.xy;
  float screenRatio = texDim.x / texDim.y;
  float gGlobalTime = gData1.z;

  float3 tex = origTexture.Sample(samAnisotropic, uv).rgb;

  float barHeight = gData1.w;
  float barSpeed = gData2.x;
  float barOverflow = gData2.y;
  float blurBar = clamp(sin(uv.y * barHeight + gGlobalTime * barSpeed) + 1.25f, 0.f, 1.f);
  float bar = clamp(floor(sin(uv.y * barHeight + gGlobalTime * barSpeed) + 1.95f), 0., barOverflow);

  float noiseIntensity = gData2.z;
  float pixelDensity = gData2.w;

  float r = clamp(rand(float2(floor(uv.x * pixelDensity * screenRatio), floor(uv.y * pixelDensity)) *
            gGlobalTime / 1000.f) + 1. - noiseIntensity, 0.f, 1.f);

  float3 color = float3(r,r,r);

  color = lerp(color - noiseIntensity * float3(.25, .25, .25), color, blurBar);
  color = lerp(color - noiseIntensity * float3(.08, .08, .08), color, bar);
  color = lerp(float3(0,0,0), tex, color);

  r = 1.0 - pow(distance(uv, float2(0.5, 0.5)), 2.1) * 2.8;
  color *= float3(r,r,r);

  // Vignette
  float vignette = (2.0 * 2.0 * uv.x * uv.y * (1.0 - uv.x));
  r = pow(vignette, 0.1);
  color *= float3(r,r,r);
  
  //float scanLines = clamp(0.35 + 0.35 * cos(3.5 + uv.y * gTexDim.y * 1.5), 0.0, 1.0);
  //
  //float saturation = pow(scanLines, 1.0);
  //r = 1.0 + 1.5 * saturation;
  //color = color * float3(r,r,r);
  //
  //if (uv.x < 0.0 || uv.x > 0.999)
  //  color *= 0.0;
  //
  //if (uv.y < 0.0 || uv.y > 0.999)
  //  color *= 0.0;

  return float4(color, 1.f);
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
