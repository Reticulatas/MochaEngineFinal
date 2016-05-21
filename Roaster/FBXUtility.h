#ifndef FBXUTILITY_H_
#define FBXUTILITY_H_

#include "fbxsdk.h"
#include "Math/Math.h"

using namespace Math;

FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
FbxAMatrix GetPoseMatrix(FbxNode* pNode,FbxPose* pPose);
void SetAMatrix(FbxAMatrix& outMatrix, const FbxMatrix& inMatrix);

FbxAMatrix GetGeometry(FbxNode* node);
FbxAMatrix GetGlobalDefaultPosition(FbxNode* fbxNode);

Vector3 Transform(const FbxMatrix& pMatrix, const Vector3& point);

bool CheckScaling(FbxVector4 scale);
bool CheckPositive(FbxVector4 scale);

bool IsBoneNode(FbxNode* pNode);
FbxAMatrix GetLocalMatrixFromTime(FbxNode* fbxNode, const FbxTime& current);

Vector3 FbxVectorToMyVector(FbxVector4& vec);
Quaternion FbxQuaternionToMyQuaternion(FbxQuaternion& quat);

#endif