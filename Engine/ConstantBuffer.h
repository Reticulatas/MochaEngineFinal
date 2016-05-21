#pragma once

#include "GraphicsInclude.h"
#include "MathIncludes.h"

struct EmitterProperties;
struct ParticleRenderConstants;
class CShadowProbe;

class ConstantBuffer
{
public:

  // Constant buffer descriptions
  struct PerMeshBuffer
  {
    Matrix gWorld;
    Matrix gWorldInvTranspose;
    Matrix gPrevWVP;
  };

  struct PerMeshLight
  {
	  Vector3 gGlobalAmbient;
	  float   gBumpFactor;
	  Vector4 gEmissiveColor;
	  Vector4 gEyePosWorld;
	  float   gMetallicFactor;
	  float	  gRoughnessFactor;
	  Vector2 padding;
  };

  struct PerCameraBuffer
  {
    Matrix gWorldViewProj;
  };

  struct PerLightBuffer
  {
    Matrix gInvView;
    LightProperties gLight;    
    Vector3 gEyePos;
    float gIntensity;
    Vector4 gPlanes;
  };

  struct ForwardLightBuffer
  {
    LightProperties gFLight[MAX_FORWARDLIGHTS];
    Vector3 gFEyePos;
    int     gNumLights;
  };

  struct PerAnimBuffer
  {
    Matrix4 gAnimTransforms[MAX_SKELBONES];
  };

  struct PostProcessBuffer
  {
	  // Generic float 4, use them for whatever
	  Vector4 gDataBank1;
	  Vector4 gDataBank2;
  };


  struct PerMatBuffer
  {
    Vector4 gTintcolor;
    Matrix4 gUVTransform;
  };

  struct ReflectionProbeBuffer
  {
	  Vector4 gProbePos;
	  Vector3 gProbeDim; // X , Y, Z Dimentions
	  float	isRenderingProbe;
  };

  struct ShadowProbeBuffer
  {
    Vector4 gShadowBuffer;
    int gIsRenderingShadow;
    int gUseSoftShadow;
    Vector2 gPad;
  };


  template <typename T>
  bool CreateBuffer(ID3D11Device* device);

  ID3D11Buffer* mConstBuffer;
};

template <typename T>
bool ConstantBuffer::CreateBuffer(ID3D11Device* device)
{
  HRESULT result;
  D3D11_BUFFER_DESC BufferDesc;

  // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
  BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  BufferDesc.ByteWidth = sizeof(T);
  BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  BufferDesc.MiscFlags = 0;
  BufferDesc.StructureByteStride = 0;

  // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
  result = device->CreateBuffer(&BufferDesc, NULL, &mConstBuffer);
  
  if (FAILED(result))
    return false;

  return true;
}

class ConstantBufferManager
{
public:

  enum ConstBufferType
  {
    PER_CAMERA,
    PER_MESH,
    PER_LIGHT,
    PER_ANIM,
    POST_PROCESS,
    PER_MATERIAL,
    FORWARDLIGHT,
    REFLECTION_PROBE,
    SHADOW_PROBE,
    PER_MESH_LIGHT,
    PARTICLE_BUF,
    TOTAL
  };

  static void Init(ID3D11Device* device);
  static void Destroy();

  // Buffer Updates
  static void BindCameraBuffer(const Matrix4& wvpMatrix);
  static void BindMeshBuffer(Matrix4& worldMatrix, Matrix4& worldInvTraspMatrix = Matrix4(), Matrix4& prevWVPMatrix = Matrix4());
  static void BindLightBuffer(const CLight* light, const CCamera* cam);
  static void BindAnimBuffer(std::vector<VQS>& animTransforms);
  static void BindFXBuffer(const Vector4& data1, const Vector4& data2 = Vector4());
  static void BindMaterialBuffer(const Vector4& tintcolor, const Matrix4& uvTransform);
  static void BindForwardLightBuffer(const vector<LightProperties>& lightBuffer, const int& numLights, const CCamera* cam);
  static void BindReflectionProbeBuffer(const Vector3& probePosition, const Vector3& probeDim, float isRenderingProbe = 0.0f);
  static void BindShaderProbeBuffer(const CShadowProbe* shadowProbe = NULL, bool isRenderingProbe = false);
  static void BindMeshLightBuffer(const Vector4& emissiveColor, float bumpFactor, float metallicFactor, float roughnessFactor, const Vector3& camPos);
  static void BindParticleUpdateConstantBuffer(const CCamera* cam, const EmitterProperties& eProp, ID3D11Buffer * buf);
  static void BindParticleRenderConstantBuffer(const CCamera* cam, ID3D11Buffer * buf);

  static ConstantBuffer mConstBufferContainer[TOTAL];
  static unsigned mBuffersInUse;
};