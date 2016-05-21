#include "FBXUtility.h"

FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
  FbxAMatrix lPoseMatrix;
  FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
  SetAMatrix(lPoseMatrix,lMatrix);
  return lPoseMatrix;
}

FbxAMatrix GetPoseMatrix(FbxNode* pNode,FbxPose* pPose)
{
  int lNodeIndex = pPose->Find(pNode);
  return GetPoseMatrix( pPose , lNodeIndex );
}

void SetAMatrix(FbxAMatrix& outMatrix, const FbxMatrix& inMatrix)
{
  memcpy((double*)&outMatrix.mData, &inMatrix.mData, sizeof(inMatrix.mData));
}

FbxAMatrix GetGeometry(FbxNode* node)
{
  // Stores the local position, global position, and parent's global position
  FbxAMatrix geometryTransform;

  // Get the translation, rotation, and scaling
  FbxVector4 T, R, S;
  T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
  R = node->GetGeometricRotation(FbxNode::eSourcePivot);
  S = node->GetGeometricScaling(FbxNode::eSourcePivot);
  geometryTransform.SetTRS(T,R,S);

  return geometryTransform;
}

Math::Vector3 Transform(const FbxMatrix& pMatrix, const Math::Vector3& pt)
{
  FbxVector4 v(pt.x, pt.y, pt.z);
  v = pMatrix.MultNormalize(v);

  return Vector3(v.mData[0], v.mData[1], v.mData[2]);
}

inline bool IsEqualEpsilon( float A, float B )
{
  return fabs( A - B ) <= 1e-5f;
}

bool CheckPositive(FbxVector4 scale)
{
  if( scale[0] < 0.0f || 
    scale[1] < 0.0f ||
    scale[2] < 0.0f )
  {
    return false;
  }

  return true;
}

bool CheckScaling(FbxVector4 scale)
{
  if( !IsEqualEpsilon( scale[0] , scale[1] ) ||
    !IsEqualEpsilon( scale[1], scale[2] ) ||
    !IsEqualEpsilon( scale[0], scale[2] ) )
  {
    return false;
  }

  return true;
}

FbxAMatrix GetLocalMatrixFromTime( FbxNode* fbxNode, const FbxTime& current )
{
  FbxNode* pParent = fbxNode->GetParent();
  FbxAMatrix localTransform;

  if( IsBoneNode(pParent) )
  {
    FbxAMatrix PX = pParent->EvaluateGlobalTransform(current);
    FbxAMatrix LX = fbxNode->EvaluateGlobalTransform(current);
    PX = PX.Inverse();
    localTransform = PX * LX;
  }
  else
  {
    //We want root bones in global space
    localTransform = fbxNode->EvaluateGlobalTransform(current);
  }

  return localTransform;
}

bool IsBoneNode( FbxNode* pNode )
{
  if( pNode->GetNodeAttribute() != NULL )
  {
    FbxNodeAttribute::EType lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
    if( lAttributeType == FbxNodeAttribute::eSkeleton )
      return true;
  }
  return false;
}

Math::Vector3 FbxVectorToMyVector( FbxVector4& inVec )
{
  Vector3 v;
  v.x = inVec.mData[0];
  v.y = inVec.mData[1];
  v.z = inVec.mData[2];

  return v;
}

Math::Quaternion FbxQuaternionToMyQuaternion( FbxQuaternion& inQuat )
{
  Quaternion q;
  q.x = inQuat.mData[0];
  q.y = inQuat.mData[1];
  q.z = inQuat.mData[2];
  q.w = inQuat.mData[3];

  return q;
}

FbxAMatrix GetGlobalDefaultPosition(FbxNode* fbxNode) 
{
  // Stores the local position, global position, and parent's global position
  FbxAMatrix local_position;
  FbxAMatrix global_position;
  FbxAMatrix parent_global_position;

  local_position.SetT(fbxNode->EvaluateLocalTranslation());
  local_position.SetR(fbxNode->EvaluateLocalRotation());
  local_position.SetS(fbxNode->EvaluateLocalScaling());

  // If the node has a parent
  if (fbxNode->GetParent())
  {
    parent_global_position = GetGlobalDefaultPosition(fbxNode->GetParent());
    global_position = parent_global_position * local_position;
  }
  else
  {
    global_position = local_position;
  }

  return global_position;
}