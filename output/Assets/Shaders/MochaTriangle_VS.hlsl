#include "PostProcessBuffer.hlsl"

VS_Output VS(uint id : SV_VertexID) 
{
  VS_Output Output;
  Output.UV = float2((id << 1) & 2, id & 2);
  Output.Pos = float4(Output.UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
  return Output;
}