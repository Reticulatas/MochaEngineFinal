#include "MochaShader.hlsl"
#include "PostProcessBuffer.hlsl"

M_TEXTURE2D velocityTexture : register(t1);

float4 MotionBlur(float2 vTexCoord, float2 vPixelVelocity, int iNumSamples)
{
  // Clamp to a max velocity.  The max we can go without artifacts os
  // is 1.4f * iNumSamples...but we can fudge things a little.
	float2 texDim = gData1.xy;

  float2 maxVelocity = (2.0f * iNumSamples) / texDim;
  vPixelVelocity = clamp(vPixelVelocity, -maxVelocity, maxVelocity);

  float2 vFinalSamplePos = vTexCoord + vPixelVelocity;

  // For each sample, sum up each sample's color in "vSum" and then divide
  // to average the color after all the samples are added.
  float4 vSum = 0;
  for (int i = 0; i < iNumSamples; i++)
  {
    // Sample texture in a new spot based on vPixelVelocity vector 
    // and average it with the other samples    
    float2 vSampleCoord = vTexCoord + (vPixelVelocity * (i / (float)iNumSamples));

    // Lookup the color at this new spot
    float4 vSample = origTexture.Sample(pointSampler, vSampleCoord);

    // Add it with the other samples
    vSum += vSample;
  }

  // Return the average color of all the samples
  return vSum / (float)iNumSamples;
}

//// Pixel Shader
float4 PS(VS_Output pin) : SV_Target
{  
	float2 vCurFramePixelVelocity = velocityTexture.Sample(pointSampler, pin.UV).rg;
	float motionBlurFactor = gData1.z;
	// Modify this for color bleeding
	vCurFramePixelVelocity = pow(vCurFramePixelVelocity, 1.0 / motionBlurFactor);

	
	return MotionBlur(pin.UV, vCurFramePixelVelocity, 8);
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
