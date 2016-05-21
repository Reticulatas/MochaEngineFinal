#include "stdafx.h"
#include "CRenderer.h"
#include "GameObject.h"
#include "CTransform.h"
#include "ConstantBuffer.h"
#include "TextureNode.h"

CRenderer::CRenderer() : Component()
{
}

CRenderer::~CRenderer()
{

}

void CRenderer::OnFree(void)
{
}

void CRenderer::OnInit(void)
{
  mBumpFactor = 1.f;
  mMetallicFactor = 1.0f;
  mRoughnessFactor = 1.0f;
  mBlendState = NO_BLEND_DEPTH_LESS;
  mPerspective = true;
  mReceiveShadows = true;
}

void CRenderer::OnStart(void)
{
  if (!mMesh.Valid())
    mMesh = AssetManager::getInstance()->GetAsset("Crate.bean").GetAssetPtr<Mesh>();

  if (!mMaterial.GetAttribute<TextureNode>()->GetTexture().Valid())
    mMaterial.SetTexture("default.png");

  Component::OnStart();
}

void CRenderer::OnUpdate(void)
{

}

Mesh* CRenderer::GetMesh() const
{
  if (mMesh.Valid())
    return mMesh.GetAssetPtr<Mesh>();

  return NULL;
}

Material* CRenderer::GetMaterial()
{
  return &mMaterial;
}

Material* CRenderer::GetCopyMaterial()
{
  return &mMaterial;
}

void CRenderer::BindBuffers(const CCamera* cam, Shader* overrideShader /*= 0*/)
{
  InputLayouts::SetInputLayout(InputLayouts::InputLayout_Vert);

  Matrix4 World = gameObject->transform->GetWorldMatrix();
  Matrix4 WorldViewProj = mPerspective ? cam->ProjView() * World : cam->Ortho() * World;

  Matrix4 WorldInvTranspose;
  float determinant = World.Determinant();
  if (!Math::IsZero(determinant))
    WorldInvTranspose = World.InverseTranspose();

  ConstantBufferManager::BindCameraBuffer(WorldViewProj);
  ConstantBufferManager::BindMeshBuffer(World, WorldInvTranspose, mPrevWorldViewProj[cam->gameObject->GetID()]);
  ConstantBufferManager::BindMeshLightBuffer(mEmissiveColor.ToVector4(), mBumpFactor, mMetallicFactor, mRoughnessFactor,cam->GetPosition());

  Renderer::getInstance()->SetBlendState(mBlendState);

  mPrevWorldViewProj[cam->gameObject->GetID()] = WorldViewProj;
}

void CRenderer::BindEarlyZBuffers(const CCamera* cam, Shader* overrideShader /*= 0*/)
{
  InputLayouts::SetInputLayout(InputLayouts::InputLayout_Vert);

  Matrix4 World = gameObject->transform->GetWorldMatrix();
  Matrix4 WorldViewProj = mPerspective ? cam->ProjView() * World : cam->Ortho() * World;

  ConstantBufferManager::BindCameraBuffer(WorldViewProj);
}

void CRenderer::GetFrustumCullData(Vector3& origin, Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
{
  Vector3 scale, globalPos, scaledHalfVec;
  Quaternion rot; Matrix3 rotMat;

  Matrix4 world = gameObject->transform->GetWorldMatrix();
  world.Decompose(&scale, &rotMat, &globalPos);

  rot = ToQuaternion(rotMat);
  scaledHalfVec = GetMesh()->GetAABB().maxPoint - origin;
  scaledHalfVec *= scale;
  origin *= scale;

  xAxis = rot.RotatedVector(Vector3::cXAxis * scaledHalfVec.x);
  yAxis = rot.RotatedVector(Vector3::cYAxis * scaledHalfVec.y);
  zAxis = rot.RotatedVector(Vector3::cZAxis * scaledHalfVec.z);
  rot.RotateVector(&origin);
  origin += globalPos;
}

void CRenderer::OnDraw(void)
{
  if (gameObject->mIsSelected)
  {
	  if (!mMesh.Valid())
		  return;
    std::vector<Vector3> verts = mMesh.GetAssetPtr<Mesh>()->GetUniqueVertices();
    std::vector<int> indices = mMesh.GetAssetPtr<Mesh>()->GetUnqiueIndices();

    Matrix4 world = gameObject->transform->GetWorldMatrix();

    for (unsigned i = 0; i < indices.size(); i += 3)
    {
      unsigned index = indices[i];
      unsigned index1 = indices[i + 1];
      unsigned index2 = indices[i + 2];

      Vector3 pos0 = TransformPoint(world, verts[index]);
      Vector3 pos1 = TransformPoint(world, verts[index1]);
      Vector3 pos2 = TransformPoint(world, verts[index2]);

      Renderer::getInstance()->DrawLinePerma(pos0, pos1, COLOR::LIME_GREEN);
      Renderer::getInstance()->DrawLinePerma(pos0, pos2, COLOR::LIME_GREEN);
      Renderer::getInstance()->DrawLinePerma(pos1, pos2, COLOR::LIME_GREEN);
    }
  }
}

void CRenderer::SetMesh(AssetHandle& meshHandle)
{
  mMesh = meshHandle.GetAssetPtr<Mesh>();
}
void CRenderer::SetMaterial(Material& matHandle)
{
    mMaterial = matHandle;
}

Vector3 CRenderer::GetMeshDimension() const
{
  Vector3 dim;
  if (mMesh.Valid())
  {
    AABB box = mMesh.GetAssetPtr<Mesh>()->GetAABB();
    dim = box.maxPoint - box.minPoint;
  }
  return dim;
}

AABB CRenderer::GetBoundingBox() const
{
  AABB box;
  box.maxPoint = Vector3(-FLT_MAX);
  box.minPoint = Vector3(FLT_MAX);
  if (mMesh.Valid())
  {
    Vector3 v; Matrix4 world = gameObject->transform->GetWorldMatrix();
    std::vector<Vector3> vertices = GetMesh()->GetUniqueVertices();
    for (auto& iter : vertices)
    {
      v = TransformPoint(world, iter);
      box.maxPoint = Max(v, box.maxPoint);
      box.minPoint = Min(v, box.minPoint);
    }
  }

  return box;
}

bool CRenderer::IsReceiveShadows() const
{
  return mReceiveShadows;
}

bool CRenderer::sortBackToFront::operator()(CRenderer *lhs, CRenderer *rhs) const
{
  Vector3 lhsPos = lhs->gameObject->transform->GetGlobalPosition();
  Vector3 rhsPos = rhs->gameObject->transform->GetGlobalPosition();

  if (lhs->mPerspective && !rhs->mPerspective)
    return true;

  else if (!lhs->mPerspective && rhs->mPerspective)
    return false;

  else if (!lhs->mPerspective && !rhs->mPerspective)
  {
    if (Equal(lhsPos.z, rhsPos.z))
      return lhs->gameObject->GetID() > rhs->gameObject->GetID();
    else
      return lhsPos.z > rhsPos.z;
  }

  Vector3 camPos = cam->gameObject->transform->GetGlobalPosition();
  Vector3 camForward = cam->gameObject->transform->GetForwardVector();

  float lhsZ = (lhsPos - camPos).Dot(camForward);
  float rhsZ = (rhsPos - camPos).Dot(camForward);

  if (Equal(lhsZ, rhsZ))
    return lhs->gameObject->GetID() > rhs->gameObject->GetID();
  else
    return lhsZ > rhsZ;
}

bool CRenderer::sortFrontToBack::operator()(CRenderer *lhs, CRenderer *rhs) const
{
  Vector3 camPos = cam->gameObject->transform->GetGlobalPosition();
  Vector3 camForward = cam->gameObject->transform->GetForwardVector();

  float lhsZ = (lhs->gameObject->transform->GetGlobalPosition() - camPos).Dot(camForward);
  float rhsZ = (rhs->gameObject->transform->GetGlobalPosition() - camPos).Dot(camForward);

  if (lhsZ == rhsZ)
    return lhs->gameObject->GetID() < rhs->gameObject->GetID();
  else
    return lhsZ < rhsZ;
}

meta_define(CRenderer);
