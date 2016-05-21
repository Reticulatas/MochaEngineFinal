

struct BufType
{
    int i;
    float f;
};

RWStructuredBuffer<BufType> BufferOut : register(u0);

[numthreads(1, 1, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
	int val = BufferOut[DTid.x].i * 4;
	float val2 = sin(BufferOut[DTid.x].f) * tan(BufferOut[DTid.x].f);
	
	BufferOut[DTid.x].i = val;
	BufferOut[DTid.x].f = val2;

	//Buffer0[DTid.x].i = val;
	//Buffer0[DTid.x].f = val2;
}
