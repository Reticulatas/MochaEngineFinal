
// Output
struct VertexOut
{
  float4 PosH		   : SV_POSITION;
  float4 Color		 : COLOR;
};

// Pixel Shader
float4 PS(VertexOut pin) : SV_Target
{
  return pin.Color;
}

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change
