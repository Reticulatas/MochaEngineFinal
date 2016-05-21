#include "MochaShader.hlsl"

struct PS_INPUT
{
	float4	pos		: SV_POSITION;
    float4  posP    : POSP;
    float3  posW    : POSW;
    float2  tex0	: TEXCOORD0;
    float4  colour  : COLOUR;
    float2  depth   : DEPTH;
};

Texture2D Texture : register(t0);
Texture2D Normal : register(t1);
Texture2D Depth : register(t2);

static const int maxLights = 50;

cbuffer cbForwardLight : register(b2)
{
    Light  gFLights[maxLights];
    float3 gFEyePosW;
    int    gNumLights;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 output = Texture.Sample(samAnisotropic, input.tex0);
	
    // Unproject position to use as texCoords
    float2 texCoords = input.posP.xy / input.posP.w;
    texCoords.y *= -1;
    texCoords = texCoords * .5f + .5f;


    float sceneDepth = Depth.Sample(pointSampler, texCoords).r;
    float linearDepth = input.depth.y / (sceneDepth - input.depth.x);

    float zFade = saturate(0.75f*(linearDepth - input.posP.z));

    // Per light per pixel per particle calculation

    float4 lightColour = float4(0, 0, 0, 0);
    float3 toEyeW = normalize(gFEyePosW - input.posW);
    for (int i = 0; i < gNumLights; ++i)
    {
        float4 oldColour = lightColour;
        // Directional light
        if (gFLights[i].Type == DIRECTIONAL)
        {

        }

        // Point light
        else if (gFLights[i].Type == POINT)
        {
            
            lightColour += ComputePointLightPhong(gFLights[i], input.posW, toEyeW, toEyeW);
        }

        // SpotLight
        else if (gFLights[i].Type == SPOTLIGHT)
        {

        }

        
        //output.w *= zFade;
    }

    float total = lightColour.r + lightColour.g + lightColour.b;
    lightColour.r /= total;
    lightColour.g /= total;
    lightColour.b /= total;
    output.xyz *= (input.colour.xyz + lightColour.xyz);
    //output.w *= clamp(min(input.colour.w, lightColour.w),0.0f, 1.0f);
    output.w *= min(input.colour.w, lightColour.w);
    output.w *= zFade;
    return output;

}