struct VS_INPUT{
	uint vertexID : SV_VertexID;
};

struct GS_INPUT{
	float4	pos		: SV_POSITION;
	float2  vel	    : VEL;
    float4  colour  : COLOUR;
    float   size : SIZE;
    float   life : LIFE;
};

struct Particle
{
	float3 mPos;
	float3 mVel;
	float mTotalLifeTime;
	float mLifetime;
	float mParticleSize;
	float4 mColor;
    //float3 mSpecialData;

};
StructuredBuffer<Particle> particleBuffer : register(u0);

GS_INPUT VS( VS_INPUT input){
	GS_INPUT output = (GS_INPUT)0;

	output.pos = float4(particleBuffer[input.vertexID].mPos, 1.0f);

    output.vel = float2(particleBuffer[input.vertexID].mVel.x, particleBuffer[input.vertexID].mVel.y);
    output.colour = particleBuffer[input.vertexID].mColor;
    output.size = particleBuffer[input.vertexID].mParticleSize;
    output.life = particleBuffer[input.vertexID].mLifetime / particleBuffer[input.vertexID].mTotalLifeTime;
	return output;
}