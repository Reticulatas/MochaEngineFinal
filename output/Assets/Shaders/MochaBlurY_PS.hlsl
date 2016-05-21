#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

static const int kernelSize = 13;
static float2 PixelKernel[kernelSize] =
{
  float2(0,-6),
  float2(0,-5),
  float2(0,-4),
  float2(0,-3),
  float2(0,-2),
  float2(0,-1),
  float2(0, 0),
  float2(0, 1),
  float2(0, 2),
  float2(0, 3),
  float2(0, 4),
  float2(0, 5),
  float2(0, 6)
};

static float BlurWeights[kernelSize] =
{
  0.002216,
  0.008764,
  0.026995,
  0.064759,
  0.120985,
  0.176033,
  0.199471,
  0.176033,
  0.120985,
  0.064759,
  0.026995,
  0.008764,
  0.002216
};

// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{
  float4 color = float4(0, 0, 0, 0);
  float2 texDim = gData1.xy;

  for (int i = 0; i < 13; i++)
  {
	float2 texelOffset = PixelKernel[i] / texDim;
    color += origTexture.Sample(samAnisotropic, pin.UV + texelOffset) * BlurWeights[i];
  }
  
  return color;
}



/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
