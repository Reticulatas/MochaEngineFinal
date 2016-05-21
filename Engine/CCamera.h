/*-----------------------------------------------------------------
----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Nicholas Fuller
-* See Footer for Revisions                                      */


#pragma once

#include "MathIncludes.h"
#include "Component.h" 
#include "IRegisterable.h"
#include "Frustum.h"
#include "AssetHandle.h"

class GameObject;
 
class CCamera : public Component
{
public:

  enum CamType
  {
    MAIN,
    VR,
    GENERIC
  };

  enum CamProj
  {
    PERSP,
    ORTHO,
    UNASSIGNED
  };

  // Constructor
  CCamera(void);

  void OnStart(void);
  void OnUpdate(void);
  void OnInit(void);
  void OnFree(void);
  void OnDraw(void);
  virtual void OnEditorStart(void) override;
  virtual void OnEditorFree(void) override;

  // Get/Set world camera position
  Vector3 GetPosition() const;
  void SetPosition(const float x, const float y, const float z);

  // Get frustum properties
  float GetNearZ() const;
  float GetFarZ() const;
  float GetAspect() const;
  float GetFovY() const;
  float GetFovX() const;

  // Get near and far plane dimensions in view space
  float GetNearWindowWidth()  const;
  float GetNearWindowHeight() const;
  float GetFarWindowWidth() const;
  float GetFarWindowHeight() const;

  // Get View/Proj matrices.
  Matrix4 View() const;
  Matrix4 Proj() const;
  Matrix4 ProjView() const;
  Matrix4 Ortho() const;

  // Ray direction not position
  Vector3 ScreenToWorldPosition(int x, int y);

  // Strafe/Walk the camera a distance d.
  void Strafe(float d);
  void Walk(float d);

  // Rotate the camera.
  void Pitch(float angle);
  void RotateY(float angle);

  // After modifying camera position/orientation, call to rebuild the view matrix.
  void UpdateViewMatrix();
  void UpdatePostProcessMatrix();
  Matrix4 PostProcessMatrix() const;

  void SetProjection(float aspect);
  void SetPerspectiveProj(float aspect, float znear, float zfar, float fovY = 0.25f * cPi);
  void SetOrthographicProj(float aspect, float znear, float zfar, float fovY = cPi);

  CamType GetCamType() const;
  struct sortCam
  {
    bool operator()( CCamera *lhs, CCamera *rhs ) const;
  };

  float FoVmodifier() const { return mFoVmodifier; }
  void FoVmodifier(float val) { if (val > 0.0f) { mFoVmodifier = val; } }

  bool IsOculusCam() const;
  void ComputeFrustum();
  void ComputeOculusFrustum(const Matrix4& view, const Matrix4& proj, const Matrix4& ortho);
  bool CullSphere(const Vector3& pt, float range);
  bool CullAABB(const std::vector<Vector3>& box);
  unsigned mVisibleObjects;

  virtual void Resize();
  virtual RenderTarget* GetRenderTarget(void) const;
  AssetHandle GetRenderTargetAssetHandle() const;
  void SetRenderTarget(AssetHandle assetH);
  void SetRenderTarget(std::string name);
  virtual void SetupCameraDirection(unsigned cubemapDirectionIndex = 0);
  virtual void OnRefreshed(void);

  CamType mCamType;
  CamProj mActiveProj;
  float mFoVmodifier;				/* vertical field of view in radians */
  bool DispatchClickMessages;
  AssetHandle mCameraRT;
  float mNearZ;				/* distance to the near plane */
  float mFarZ;				/* distance to the far plane */

  virtual void CopyFrom(const ::CCamera* c)
  {
    mFoVmodifier = c->mFoVmodifier;
    mActiveProj = c->mActiveProj;
    mPreviousProj = c->mPreviousProj;
    mCameraRT = c->mCameraRT;
    mCamType = c->mCamType;
    mFovY = c->mFovY;
    mNearZ = c->mNearZ;
    mFarZ = c->mFarZ;
    mAspect = c->mAspect;
    mNearWindowHeight = c->mNearWindowHeight;
    mFarWindowHeight = c->mFarWindowHeight;
    mView = c->mView;
    mProj = c->mProj;
    mPostProcess = c->mPostProcess;
    DispatchClickMessages = c->DispatchClickMessages;
    Component::CopyFrom(c);
  }
  COMPONENTCLONE(CCamera);

private:

  Frustum mFrustum;

  GameObject* myWidget;

  // Frustum properties
  float mFovY;
  float mAspect;				/* aspect ratio = width / height */
  float mNearWindowHeight;
  float mFarWindowHeight;
  CamProj mPreviousProj;

  // View/Proj matrices
  Matrix4 mView;
  Matrix4 mProj;
  Matrix4 mPostProcess;
  Matrix4 mOrthoProj;

  //RenderTarget* mCameraRT;

  ___SERIALIZE_BEGIN___
    ___DEFBASE(Component)
    ___DEFSER(mFoVmodifier, 1)
    ___DEFSER(mActiveProj, 1)
    ___DEFSER(mCamType, 1)
    ___DEFSER(mFovY, 1)
    ___DEFSER(mNearZ, 1)
    ___DEFSER(mFarZ, 1)
    ___DEFSER(mAspect, 1)
    ___DEFSER(mNearWindowHeight, 1)
    ___DEFSER(mFarWindowHeight, 1)
    ___DEFSER(mView, 1)
    ___DEFSER(mProj, 1)
    ___DEFSER(DispatchClickMessages, 3)
    ___DEFSER(mCameraRT, 4)
    ___SERIALIZE_END___

	public:
		metadef(CCamera)
		m_add(float, mFovY)
		m_tag(MINTBOARDTAG)
		m_add(float, mNearZ)
		m_tag(MINTBOARDTAG)
		m_add(float, mFarZ)
		m_tag(MINTBOARDTAG)
		m_add(float, mAspect)
		m_tag(MINTBOARDTAG)
		m_add(float, mNearWindowHeight)
		m_tag(MINTBOARDTAG)
		m_add(float, mFarWindowHeight)
		m_tag(MINTBOARDTAG)
		endmetadef;



};
 
___SERIALIZE_CLASS(CCamera, 4);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change