Texture2D origTexture : register(t0);

struct VS_Output
{
  float4 Pos : SV_POSITION;
  float2 UV : TEXCOORD0;
};

cbuffer FXBuffer : register(b3)
{
  //float2 gTexDim;
  //float gGlobalTime;
  //float gPad;
	float4 gData1;
	float4 gData2;
};

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
