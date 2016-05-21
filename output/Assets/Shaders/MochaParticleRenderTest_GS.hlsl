cbuffer ParticleRenderConstants : register(b0) 
{
    matrix gView;
    matrix gProjection;
    //matrix gWorldProj;
    float3 camPos;
    float  quadLength;
}


struct GS_INPUT{
	float4	pos		: SV_POSITION;
    float2  vel	    : VEL;
    float4  colour  : COLOUR;
    float   size    : SIZE;
    float   life    : LIFE;
};

struct PS_INPUT{
	float4	pos		: SV_POSITION;
    float4  posP    : POSP;
    float3  posW    : POSW;
	float2  tex0	: TEXCOORD0;
    float4  colour  : COLOUR;
    float2  depth   : DEPTH;
};
static const float PI = 3.14159265359f;
static const float TWOPI = 6.28318530718f;
[maxvertexcount(4)]
void GS(point GS_INPUT p[1], inout TriangleStream<PS_INPUT> triStream){
	PS_INPUT p1 = (PS_INPUT)0;
	

    float3 normal = p[0].pos - camPos;
	normal		= mul(normal, gView);

    normal = normalize(normal);

    float3 rightAxis = cross(float3(0.0f, 1.0f, 0.0f), normal);

        //vrot = vcosA + (k x v)sinA + k(k.v)(1-cosA)
    //float radianAngle = p[0].life * TWOPI;
    //float cosA = cos(radianAngle);
    //float sinA = sin(radianAngle);

    //float4 currVel = float4(p[0].vel, 0.0f, 1.0f);
    //currVel = mul(currVel, gView);
    //rightAxis = rightAxis * cosA + cross(normal, rightAxis) * sinA + normal * (dot(normal, rightAxis) * (1 - cosA));
    //rightAxis.x = currVel.x;
    //rightAxis.y = currVel.y;
    rightAxis = normalize(rightAxis);
    float3 upAxis			= cross(normal, rightAxis);

    float nearPlane = gProjection[2][2];
    float farPlane = gProjection[3][2];

    float4 rightVector = float4(rightAxis.xyz, 1.0f);
    float4 upVector = float4(upAxis.xyz, 1.0f);
    p1.posW = p[0].pos.xyz; // world position for lighting
	p[0].pos				= mul(p[0].pos, gView);


    p1.depth.x = nearPlane;
    p1.depth.y = farPlane;
    p1.colour = p[0].colour;

    float cacheSize = quadLength * p[0].size;

    p1.pos = p[0].pos + rightVector * (cacheSize) + upVector * (cacheSize);
    p1.tex0 = float2(1.0f, 0.0f);
    p1.pos = mul(p1.pos, gProjection);
    p1.posP = p1.pos;
    triStream.Append(p1);

    p1.pos = p[0].pos + rightVector * (cacheSize) + upVector * (-cacheSize);
    p1.tex0 = float2(1.0f, 1.0f);
    p1.pos = mul(p1.pos, gProjection);
    p1.posP = p1.pos;
    triStream.Append(p1);

    p1.pos = p[0].pos + rightVector * (-cacheSize) + upVector * (cacheSize);
    p1.tex0 = float2(0.0f, 0.0f);
    p1.pos = mul(p1.pos, gProjection);
    p1.posP = p1.pos;
    triStream.Append(p1);

    p1.pos = p[0].pos + rightVector * (-cacheSize) + upVector * (-cacheSize);
    p1.tex0 = float2(0.0f, 1.0f);
    p1.pos = mul(p1.pos, gProjection);
    p1.posP = p1.pos;
    triStream.Append(p1);
	


}
