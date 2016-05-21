#include "Light.hlsl"

// Mocha shader definitions

cbuffer cbPerCamera : register(b0)
{
  matrix gWorldViewProj;  
};

cbuffer cbPerObject : register(b1)
{
  matrix gWorld;
  matrix gWorldInvTranspose;
  matrix gPrevWorldViewProj;
};

cbuffer cbPerLight : register(b2)
{
  matrix gInvView;
  Light gLight;  
  float3 gEyePosW;
  float  gIntensity;
  float4 gPlanes;
};

cbuffer cPerMeshLight : register(b3)
{
	float3  gGlobalAmbient;
	float   gBumpFactor;
	float4	gEmissiveColor;
	float4  gEyePosWorld;
	float   gMetallicFactor;
	float	gRoughnessFactor;
	float2  padding;
};

// Input
struct VertexIn
{
  float3 PosL      : POSITION;
  float4 Color     : COLOR;
  float2 UV 	   : TEXCOORD_ONE;
  float3 NormalL   : NORMAL;
  float3 TangentL  : TANGENT;
  float3 BinormalL : BINORMAL;
};

// Output
struct VertexOut
{
  float4 PosH		   : SV_POSITION;
  float3 PosW		   : POSITION;
  float4 Color		 : COLOR;
  float2 UV 		   : TEXCOORD_ONE;
  float3 NormalW	 : NORMAL;
  float3 TangentW  : TANGENT;
  float3 BinormalW : BINORMAL;
  float4 CurrPosH  : CURRPOSITION;
  float4 PrevPosH  : PREVPOSITION;
};

struct VertexOutLightStage
{
  float4 PosH		   : SV_POSITION;
  float4 PosV		   : POSITION;      // View Space
  float4 PosP      : POISTION_PROJ; // Proj Space - for sampling
  float4 Color		 : COLOR;
  float2 UV 		   : TEXCOORD_ONE;
};

struct PixelOut
{
	float4 normal   : SV_Target0;
	float4 albedo   : SV_Target1;
 // float4 specular : SV_Target2;
  float4 velocity : SV_Target2;
  float4 light    : SV_Target3;
};

struct ParticleVertIn
{
	float3 Pos      : POSITION;
	float4 Color    : COLOR;
	float2 UV 	    : TEXCOORD_ONE;
};

float4 GammaCorrectedToLinear(float4 gammaSpaceColor)
{
	return pow(gammaSpaceColor, 2.2);
}

float4 GammaCorrection(float4 linearColor)
{
	return pow(linearColor, 1 / 2.2);
}