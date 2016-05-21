// DEFINITIONS
typedef float4 M_COLOR;
typedef Texture2D M_TEXTURE2D;
typedef TextureCube M_TEXTURE3D;
typedef float4x4 M_UV_TRANSFORM;

SamplerState pointSampler   : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerState clampAnisotropic : register(s2);