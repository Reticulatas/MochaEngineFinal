#include "FBXExporter.h"
#include "FBXUtility.h"

FBXExporter::FBXExporter()
{
  mFbxManager = 0;
  mFbxScene = 0;
  mMeshType = MESH_STATIC;
  mHasSkeleton = false;
  mBindPose = 0;
}

FBXExporter::~FBXExporter()
{
  if (mFBXImporter)
    mFBXImporter->Destroy();

  if (mFbxScene)
    mFbxScene->Destroy();

  if (mFbxManager)
    mFbxManager->Destroy();
}

bool FBXExporter::Initialize()
{
  // Create the FBX SDK manager
  mFbxManager = FbxManager::Create();

  if(!mFbxManager)
      return false;

  // Create an IOSettings object.
  FbxIOSettings * ios = FbxIOSettings::Create(mFbxManager, IOSROOT );
  mFbxManager->SetIOSettings(ios);    

  // Create a new scene so it can be populated by the imported file.
  mFbxScene = FbxScene::Create(mFbxManager, "myScene");

  mFBXImporter = FbxImporter::Create(mFbxManager, "myImporter");

  return true;
}

void FBXExporter::LoadFBX(BoostPath path)
{
  std::cout << "Loading: " << path.filename().string() << std::endl;

  bool result = mFBXImporter->Initialize(path.string().c_str(), -1, mFbxManager->GetIOSettings());

  if (!result)
  {
    printf("Call to FbxLoader::Initialize() failed.\n");
    printf("Error returned: %s\n\n", mFBXImporter->GetStatus().GetErrorString());
    return;
  }

  mFBXImporter->Import(mFbxScene);

  mConverter.Initialize(mFbxScene);
  mBindPose =  mFbxScene->GetPose(0);

  FbxArray<FbxString*> AnimNameArray;
  mFbxScene->FillAnimStackNameArray(AnimNameArray);
  int poseCount = mFbxScene->GetPoseCount();//Need a Bind Pose
  int animCount = mFbxScene->GetSrcObjectCount<FbxAnimStack>();
  if (poseCount > 0 && mBindPose && animCount >= 1)
  {
    mMeshType = MESH_SKINNED;
    mHasSkeleton = true;
  }

  // Process Skeleton
  if(mHasSkeleton == true)
  {
    ProcessSkeletonHierarchy(mFbxScene->GetRootNode());
    ProcessBindPose();
    //for(int i = 0; i < animCount; ++i)
    //  ProcessAnimations(i);
  }
  ProcessGeometry(mFbxScene->GetRootNode());
  ReIndexBuffer();

  // If no mesh, just process animations
  if (mFBXIndices.empty())
  {
    mMeshType = ONLY_ANIMATION;
    ProcessSkeletonHierarchy(mFbxScene->GetRootNode());
    for (int i = 0; i < animCount; ++i)
      ProcessAnimations(i);
  }
    
}

void FBXExporter::DestroyScene()
{
  // Reinitialize members
  mMeshType = MESH_STATIC;
  mHasSkeleton = false;
  mBindPose = 0;

  // Clear containers
  mBoneNodes.clear();
  mFBXVertices.clear();
  mFBXSkinnedVertices.clear();
  mFBXIndices.clear();
  mFBXTriangles.clear();

  mMax = mMin = mOrigin = Vector3::cZero;
  //mUniqueVertices.clear();
  //mUniqueIndices.clear();
  //mUniqueTriangles.clear();

  for (auto iter = mFBXMaterialLookUp.begin(); iter != mFBXMaterialLookUp.end(); ++iter)
    delete iter->second;
  mFBXMaterialLookUp.clear();

  mFBXSkeleton.mBones.clear();

  for (auto iter = mFBXAnimation.begin(); iter != mFBXAnimation.end(); ++iter)
  {
    Animation* anim = *iter;
    anim->mTracks.clear();
    delete anim;
  }
  mFBXAnimation.clear();
}

void FBXExporter::ProcessGeometry(FbxNode* fbxNode)
{
  if (fbxNode->GetNodeAttribute())
  {
    switch (fbxNode->GetNodeAttribute()->GetAttributeType())
    {
    case FbxNodeAttribute::eMesh:

        ProcessControlPoints(fbxNode);
        if(mHasSkeleton)
            ProcessSkinWeights(fbxNode);
        ProcessMesh(fbxNode);
        AssociateMaterialToMesh(fbxNode);
        ProcessMaterials(fbxNode);

        break;
    }
  }

  for (int i = 0; i < fbxNode->GetChildCount(); ++i)
      ProcessGeometry(fbxNode->GetChild(i));
}

void FBXExporter::ProcessControlPoints(FbxNode* fbxNode)
{
  FbxMesh* currMesh = fbxNode->GetMesh();
  unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
  for(unsigned int i = 0; i < ctrlPointCount; ++i)
  {
    ControlPoint* ctrlPoint = new ControlPoint();
    ctrlPoint->mPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
    ctrlPoint->mPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
    ctrlPoint->mPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
    mControlPoints[i] = ctrlPoint;
  }
}

void FBXExporter::ProcessMesh(FbxNode* fbxNode)
{
  FbxAMatrix  meshTransform;
  if(mBindPose)
  {
    //For skinned meshes get the pose matrix
    meshTransform = GetPoseMatrix( fbxNode , mBindPose );
  }
  else
    meshTransform = GetGlobalDefaultPosition(fbxNode);

  //Meshes have a separate geometry transform that also needs to be applied
  FbxAMatrix geoTransform = GetGeometry(fbxNode);
  meshTransform = meshTransform  * geoTransform;

  //Convert the matrix into the destination coordinate space
  mConverter.ConvertMeshMatrix( meshTransform );

  for(auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
    itr->second->mPosition = Transform(meshTransform, itr->second->mPosition);

  meshTransform.SetRow(3, FbxVector4(0,0,0,1));

  FbxMesh* currMesh = fbxNode->GetMesh();

  int vertexCounter = 0;
  // faces could be quads not just triangles
  unsigned triangleCount = currMesh->GetPolygonCount();
  mFBXTriangles.reserve(triangleCount);

  std::vector<Vector3> normalArray;
  std::vector<Vector3> tangentArray;
  std::vector<Vector3> binormalArray;
  std::vector<Vector2> uvArray;

  for (unsigned i = 0; i < triangleCount; ++i)
  {
    unsigned polySize = currMesh->GetPolygonSize(i);

    normalArray.resize(polySize);
    uvArray.resize(polySize);
    tangentArray.resize(polySize);
    binormalArray.resize(polySize);

    // New Triangle
    FBXTriangle currTriangle;
    mFBXTriangles.push_back(currTriangle);

    for (unsigned j = 0; j < polySize; ++j)
    {
      int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
      ControlPoint* currCtrlPoint = mControlPoints[ctrlPointIndex];
      mMax = Max(mMax, currCtrlPoint->mPosition);
      mMin = Min(mMin, currCtrlPoint->mPosition);

      ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normalArray[j]);
      ReadTangent(currMesh, ctrlPointIndex, vertexCounter, tangentArray[j]);
      ReadBinormal(currMesh, ctrlPointIndex, vertexCounter, binormalArray[j]);

      // We only have diffuse texture
      for (int k = 0; k < 1; ++k)
          ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), k, uvArray[j]);

      // Store new vertex - Static/Skinned
      if (mMeshType == MESH_STATIC)
      {  
        FBXVertex v;
        v.position = currCtrlPoint->mPosition;
        v.normal = Transform(meshTransform, normalArray[j]);
        v.tangent = Transform(meshTransform, tangentArray[j]);
        v.binormal = Transform(meshTransform, binormalArray[j]);
        v.texcoords = uvArray[j];
        v.texcoords.y = 1 - v.texcoords.y;

        mFBXVertices.push_back(v);
      }
      else
      {  
        FBXVertexSkin v;
        v.position = currCtrlPoint->mPosition;
        v.normal = Transform(meshTransform, normalArray[j]);
        v.tangent = Transform(meshTransform, tangentArray[j]);
        v.binormal = Transform(meshTransform, binormalArray[j]);
        v.texcoords = uvArray[j];
        v.texcoords.y = 1 - v.texcoords.y;

        for (unsigned i = 0; i < currCtrlPoint->mBlendingInfo.size(); ++i)
        {
          v.blendIndices[i] = currCtrlPoint->mBlendingInfo[i].mBlendingIndex;
          v.blendWeights[i] = currCtrlPoint->mBlendingInfo[i].mBlendingWeight;
        }

        mFBXSkinnedVertices.push_back(v);
      }
      
      mFBXTriangles.back().mIndices.push_back(vertexCounter);
      ++vertexCounter;
    }
  }

  // no longer need mControlPoints, delete them
  for(auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
    delete itr->second;

  mOrigin = (mMax + mMin) * .5f;
  mControlPoints.clear();
}

void FBXExporter::ReadNormal( FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outNormal )
{
  if(inMesh->GetElementNormalCount() < 1)
  {
    throw std::exception("Invalid Normal Number");
  }

  FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
  switch(vertexNormal->GetMappingMode())
  {
  case FbxGeometryElement::eByControlPoint:
    switch(vertexNormal->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
      {
        outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
        outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
        outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
      }
      break;

    case FbxGeometryElement::eIndexToDirect:
      {
        int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
        outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
        outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
        outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
      }
      break;

    default:
      throw std::exception("Invalid Reference");
    }
    break;

  case FbxGeometryElement::eByPolygonVertex:
    switch(vertexNormal->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
      {
        outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
        outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
        outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
      }
      break;

    case FbxGeometryElement::eIndexToDirect:
      {
        int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
        outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
        outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
        outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
      }
      break;

    default:
      throw std::exception("Invalid Reference");
    }
    break;
  }
}

void FBXExporter::ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outTangent)
{
  // No tangents
  if (inMesh->GetElementTangentCount() < 1)
    return;
  
  FbxGeometryElementTangent* vertexTangent = inMesh->GetElementTangent(0);
  switch (vertexTangent->GetMappingMode())
  {
  case FbxGeometryElement::eByControlPoint:
      switch (vertexTangent->GetReferenceMode())
      {
      case FbxGeometryElement::eDirect:
      {
        outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
        outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
        outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
      }
      break;
  
      case FbxGeometryElement::eIndexToDirect:
      {
        int index = vertexTangent->GetIndexArray().GetAt(inCtrlPointIndex);
        outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
        outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
        outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
      }
      break;
  
      default:
          throw std::exception("Invalid Reference");
      }
      break;
  
  case FbxGeometryElement::eByPolygonVertex:
      switch (vertexTangent->GetReferenceMode())
      {
      case FbxGeometryElement::eDirect:
      {
        outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[0]);
        outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[1]);
        outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[2]);
      }
          break;
  
      case FbxGeometryElement::eIndexToDirect:
      {
        int index = vertexTangent->GetIndexArray().GetAt(inVertexCounter);
        outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
        outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
        outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
      }
      break;
  
      default:
          throw std::exception("Invalid Reference");
      }
      break;
  }
}

void FBXExporter::ReadBinormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outBinormal)
{
  if (inMesh->GetElementBinormalCount() < 1)
    return;

  FbxGeometryElementBinormal* vertexBinormal = inMesh->GetElementBinormal(0);
  switch (vertexBinormal->GetMappingMode())
  {
  case FbxGeometryElement::eByControlPoint:
    switch (vertexBinormal->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
    {
      outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
      outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
      outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
    }
      break;

    case FbxGeometryElement::eIndexToDirect:
    {
      int index = vertexBinormal->GetIndexArray().GetAt(inCtrlPointIndex);
      outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
      outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
      outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
    }
      break;

    default:
      throw std::exception("Invalid Reference");
    }
    break;

  case FbxGeometryElement::eByPolygonVertex:
    switch (vertexBinormal->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
    {
      outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
      outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
      outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
    }
      break;

    case FbxGeometryElement::eIndexToDirect:
    {
      int index = vertexBinormal->GetIndexArray().GetAt(inVertexCounter);
      outBinormal.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
      outBinormal.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
      outBinormal.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
    }
      break;

    default:
      throw std::exception("Invalid Reference");
    }
    break;
  }
}

void FBXExporter::ReadUV( FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, Vector2& outUV )
{
  if(inUVLayer >= 2 || inMesh->GetElementUVCount() <= inUVLayer)
  {
    throw std::exception("Invalid UV Layer Number");
  }
  FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayer);

  switch(vertexUV->GetMappingMode())
  {
  case FbxGeometryElement::eByControlPoint:
    switch(vertexUV->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
      {
        outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
        outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
      }
      break;

    case FbxGeometryElement::eIndexToDirect:
      {
        int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
        outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
        outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
      }
      break;

    default:
      throw std::exception("Invalid Reference");
    }
    break;

  case FbxGeometryElement::eByPolygonVertex:
    switch(vertexUV->GetReferenceMode())
    {
    case FbxGeometryElement::eDirect:
    case FbxGeometryElement::eIndexToDirect:
      {
        outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[0]);
        outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
      }
      break;

    default:
      throw std::exception("Invalid Reference");
    }
    break;
  }
}

void FBXExporter::AssociateMaterialToMesh(FbxNode* fbxNode)
{
  FbxLayerElementArrayTemplate<int>* materialIndices;
  FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;
  FbxMesh* currMesh = fbxNode->GetMesh();

  if(currMesh->GetElementMaterial())
  {
    materialIndices = &(currMesh->GetElementMaterial()->GetIndexArray());
    materialMappingMode = currMesh->GetElementMaterial()->GetMappingMode();

    if(materialIndices)
    {
      switch(materialMappingMode)
      {
      case FbxGeometryElement::eByPolygon:
        {
          if (materialIndices->GetCount() == mFBXTriangles.size())
          {
            for (unsigned int i = 0; i < mFBXTriangles.size(); ++i)
            {
              unsigned int materialIndex = materialIndices->GetAt(i);
              mFBXTriangles[i].mMaterialIndex = materialIndex;
            }
          }
        }
        break;

      case FbxGeometryElement::eAllSame:
        {
          unsigned int materialIndex = materialIndices->GetAt(0);
          for (unsigned int i = 0; i < mFBXTriangles.size(); ++i)
          {
            mFBXTriangles[i].mMaterialIndex = materialIndex;
          }
        }
        break;

      default:
        throw std::exception("Invalid mapping mode for material\n");
      }
    }
  }
}

void FBXExporter::ProcessMaterials(FbxNode* fbxNode)
{
  unsigned int materialCount = fbxNode->GetMaterialCount();

  for(unsigned int i = 0; i < materialCount; ++i)
  {
    FbxSurfaceMaterial* surfaceMaterial = fbxNode->GetMaterial(i);
    ProcessMaterialAttribute(surfaceMaterial, i);
    ProcessMaterialTexture(surfaceMaterial, mFBXMaterialLookUp[i]);
  }
}

void FBXExporter::ProcessMaterialAttribute( FbxSurfaceMaterial* inMaterial, unsigned int inMaterialIndex )
{
  FbxDouble3 double3;
  FbxDouble double1;
  if (inMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
  {
    PhongMaterial* currMaterial = new PhongMaterial();

    // Ambient Color
    double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Ambient;
    currMaterial->mAmbient.x = static_cast<float>(double3.mData[0]);
    currMaterial->mAmbient.y = static_cast<float>(double3.mData[1]);
    currMaterial->mAmbient.z = static_cast<float>(double3.mData[2]);

    // Diffuse Color
    double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Diffuse;
    currMaterial->mDiffuse.x = static_cast<float>(double3.mData[0]);
    currMaterial->mDiffuse.y = static_cast<float>(double3.mData[1]);
    currMaterial->mDiffuse.z = static_cast<float>(double3.mData[2]);

    // Specular Color
    double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Specular;
    currMaterial->mSpecular.x = static_cast<float>(double3.mData[0]);
    currMaterial->mSpecular.y = static_cast<float>(double3.mData[1]);
    currMaterial->mSpecular.z = static_cast<float>(double3.mData[2]);

    // Emissive Color
    double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Emissive;
    currMaterial->mEmissive.x = static_cast<float>(double3.mData[0]);
    currMaterial->mEmissive.y = static_cast<float>(double3.mData[1]);
    currMaterial->mEmissive.z = static_cast<float>(double3.mData[2]);

    // Reflection
    double3 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Reflection;
    currMaterial->mReflection.x = static_cast<float>(double3.mData[0]);
    currMaterial->mReflection.y = static_cast<float>(double3.mData[1]);
    currMaterial->mReflection.z = static_cast<float>(double3.mData[2]);

    // Transparency Factor
    double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->TransparencyFactor;
    currMaterial->mTransparencyFactor = double1;

    // Shininess
    double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->Shininess;
    currMaterial->mShininess = double1;

    // Specular Factor
    double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->SpecularFactor;
    currMaterial->mSpecularPower = double1;


    // Reflection Factor
    double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->ReflectionFactor;
    currMaterial->mReflectionFactor = double1;

    mFBXMaterialLookUp[inMaterialIndex] = currMaterial;
  }
  else if (inMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
  {
    LambertMaterial* currMaterial = new LambertMaterial();

    // Ambient Color
    double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Ambient;
    currMaterial->mAmbient.x = static_cast<float>(double3.mData[0]);
    currMaterial->mAmbient.y = static_cast<float>(double3.mData[1]);
    currMaterial->mAmbient.z = static_cast<float>(double3.mData[2]);

    // Diffuse Color
    double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Diffuse;
    currMaterial->mDiffuse.x = static_cast<float>(double3.mData[0]);
    currMaterial->mDiffuse.y = static_cast<float>(double3.mData[1]);
    currMaterial->mDiffuse.z = static_cast<float>(double3.mData[2]);

    // Emissive Color
    double3 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->Emissive;
    currMaterial->mEmissive.x = static_cast<float>(double3.mData[0]);
    currMaterial->mEmissive.y = static_cast<float>(double3.mData[1]);
    currMaterial->mEmissive.z = static_cast<float>(double3.mData[2]);

    // Transparency Factor
    double1 = reinterpret_cast<FbxSurfaceLambert *>(inMaterial)->TransparencyFactor;
    currMaterial->mTransparencyFactor = double1;

    mFBXMaterialLookUp[inMaterialIndex] = currMaterial;
  }
}

void FBXExporter::ProcessMaterialTexture( FbxSurfaceMaterial* inMaterial, FBXMaterial* ioMaterial )
{
  unsigned int textureIndex = 0;
  FbxProperty property;

  FBXSDK_FOR_EACH_TEXTURE(textureIndex)
  {
    property = inMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
    if(property.IsValid())
    {
      unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
      for(unsigned int i = 0; i < textureCount; ++i)
      {
        //FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);
        //if(layeredTexture)
        //{
        //  throw std::exception("Layered Texture is currently unsupported\n");
        //}
        //else
        {
          FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
          if(texture)
          {
            std::string textureType = property.GetNameAsCStr();
            FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

            if(fileTexture)
            {
              if (textureType == "DiffuseColor")
              {
                ioMaterial->mDiffuseMapName = fileTexture->GetFileName();
              }
              else if (textureType == "SpecularColor")
              {
                ioMaterial->mSpecularMapName = fileTexture->GetFileName();
              }
              else if (textureType == "Bump")
              {
                ioMaterial->mNormalMapName = fileTexture->GetFileName();
              }
            }
          }
        }
      }
    }
  }
}

void FBXExporter::ReIndexBuffer()
{
  mUniqueTriangles = mFBXTriangles;

  //First get a list of unique vertices  
  if(mMeshType == MESH_STATIC)
  {
    std::vector<FBXVertex> uniqueVertices;
    for(unsigned i = 0; i < mFBXVertices.size(); ++i)
    {
      if(FindVertex( mFBXVertices[i], uniqueVertices) == -1)
        uniqueVertices.push_back(mFBXVertices[i]);

      if (FindVertex(mFBXVertices[i].position, mUniqueVertices) == -1)
        mUniqueVertices.push_back(mFBXVertices[i].position);
    }
    
    // Now we update the index buffer
    unsigned counter = 0;
    for(unsigned int i = 0; i <  mFBXTriangles.size(); ++i)
    {
      unsigned polySize = mFBXTriangles[i].mIndices.size();
    
      for (unsigned int j = 0; j < polySize; ++j, ++counter)
      {
        mFBXTriangles[i].mIndices[j] = FindVertex(mFBXVertices[counter], uniqueVertices);
        mUniqueTriangles[i].mIndices[j] = FindVertex(mFBXVertices[counter].position, mUniqueVertices);
      }        
    }
    
    mFBXVertices.clear();
    mFBXVertices = uniqueVertices;
    uniqueVertices.clear();
  }

  else
  {
    std::vector<FBXVertexSkin> uniqueVertices;
    for(unsigned i = 0; i < mFBXSkinnedVertices.size(); ++i)
    {
      if(FindVertex( mFBXSkinnedVertices[i], uniqueVertices) == -1)
        uniqueVertices.push_back( mFBXSkinnedVertices[i]);

      if (FindVertex(mFBXSkinnedVertices[i].position, mUniqueVertices) == -1)
        mUniqueVertices.push_back(mFBXSkinnedVertices[i].position);
    }

    // Now we update the index buffer
    unsigned counter = 0;
    for(unsigned int i = 0; i <  mFBXTriangles.size(); ++i)
    {
      unsigned polySize = mFBXTriangles[i].mIndices.size();

      for (unsigned int j = 0; j < polySize; ++j, ++counter)
      {
        mFBXTriangles[i].mIndices[j] = FindVertex(mFBXSkinnedVertices[counter], uniqueVertices);
        mUniqueTriangles[i].mIndices[j] = FindVertex(mFBXSkinnedVertices[counter].position, mUniqueVertices);
      }        
    }

    mFBXSkinnedVertices.clear();
    mFBXSkinnedVertices = uniqueVertices;
    uniqueVertices.clear();
  }

  std::sort(mFBXTriangles.begin(), mFBXTriangles.end());
  std::sort(mUniqueTriangles.begin(), mUniqueTriangles.end());

  for (unsigned i = 0; i < mFBXTriangles.size(); ++i)
  {
    unsigned size = mFBXTriangles[i].mIndices.size();
    //Simple Convex Polygon Triangulation: always n-2 tris
    unsigned numTris = size - 2;

    for (unsigned j = 0; j < numTris; ++j)
    {
      mFBXIndices.push_back(mFBXTriangles[i].mIndices[0]);
      mFBXIndices.push_back(mFBXTriangles[i].mIndices[2 + j]);
      mFBXIndices.push_back(mFBXTriangles[i].mIndices[1 + j]);
      
      mUniqueIndices.push_back(mUniqueTriangles[i].mIndices[0]);
      mUniqueIndices.push_back(mUniqueTriangles[i].mIndices[2 + j]);
      mUniqueIndices.push_back(mUniqueTriangles[i].mIndices[1 + j]);
    }
  }
}

template<typename T>
int FBXExporter::FindVertex( const T& inTargetVertex, const std::vector<T>& uniqueVertices )
{
  for(unsigned int i = 0; i < uniqueVertices.size(); ++i)
  {
    if(inTargetVertex == uniqueVertices[i])
      return i;
  }

  return -1;
}

void FBXExporter::ProcessSkinWeights(FbxNode* fbxNode)
{
  FbxMesh* currMesh = fbxNode->GetMesh();
  unsigned numOfDeformers = currMesh->GetDeformerCount(FbxDeformer::eSkin);

  // A deformer is a FBX thing, which contains some clusters
  // A cluster contains a link, which is basically a joint
  for (unsigned deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
  {
    // We are using only skins, so we see if this is a skin
    FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
    if (!currSkin)
      continue;

    unsigned numOfClusters = currSkin->GetClusterCount();
    for (unsigned clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
    {
      FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);

      if (!currCluster->GetLink())
        continue;

      std::string currBoneName = currCluster->GetLink()->GetName();
      unsigned currBoneIndex = FindBoneIndexUsingName(currBoneName);

      int nodeIndex = mBindPose->Find(currCluster->GetLink());
      if( nodeIndex == -1 )
      {
        FbxAMatrix linkBindMatrix;
        currCluster->GetTransformLinkMatrix(linkBindMatrix);	
        mBindPose->Add(currCluster->GetLink(),FbxMatrix(linkBindMatrix));
      }

      // Associate each joint with the control points it affects
      unsigned numOfIndices = currCluster->GetControlPointIndicesCount();
      for (unsigned i = 0; i < numOfIndices; ++i)
      {
        int index = currCluster->GetControlPointIndices()[i];
        BlendingIndexWeightPair currBlendingIndexWeightPair;
        currBlendingIndexWeightPair.mBlendingIndex = currBoneIndex;
        currBlendingIndexWeightPair.mBlendingWeight = (float)currCluster->GetControlPointWeights()[i];
        mControlPoints[index]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
      }
    }
  }

  BlendingIndexWeightPair currBlendingIndexWeightPair;

  for(auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
  {
    std::sort(itr->second->mBlendingInfo.begin(), itr->second->mBlendingInfo.end());

    if(itr->second->mBlendingInfo.size() > MAXSKINWEIGHTS)
      RemoveSkinWeights(itr->second->mBlendingInfo);

    for(unsigned i = itr->second->mBlendingInfo.size(); i < MAXSKINWEIGHTS; ++i)
      itr->second->mBlendingInfo.push_back(currBlendingIndexWeightPair);

    //Normalize the weights
    float sum = 0;
    for(unsigned w = 0; w < MAXSKINWEIGHTS; ++w)
      sum += itr->second->mBlendingInfo[w].mBlendingWeight;
    for(unsigned w = 0; w < MAXSKINWEIGHTS; ++w)
      itr->second->mBlendingInfo[w].mBlendingWeight /= sum;
  }
}

void FBXExporter::RemoveSkinWeights( std::vector<BlendingIndexWeightPair>& skinWeights )
{
  int start, end, count;

  start = 0;
  end = skinWeights.size() - 1;
  count = skinWeights.size() - MAXSKINWEIGHTS;

  while(count > 0)
  {
    skinWeights[start].mBlendingWeight += skinWeights[end].mBlendingWeight;

    ++start;
    --end;
    --count;
  }

  while(skinWeights.size() > MAXSKINWEIGHTS)
    skinWeights.pop_back();
}

unsigned FBXExporter::FindBoneIndexUsingName( std::string inJointName )
{
  for(unsigned int i = 0; i < mFBXSkeleton.mBones.size(); ++i)
  {
    if (mFBXSkeleton.mBones[i].mName == inJointName)
      return i;
  }

  throw std::exception("Skeleton information in FBX file is corrupted.");
}

void FBXExporter::ProcessSkeletonHierarchy(FbxNode* fbxNode)
{
  for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
  {
    FbxNode* currNode = fbxNode->GetChild(childIndex);
    ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1);
  }
}

void FBXExporter::ProcessSkeletonHierarchyRecursively(FbxNode* fbxNode, int inDepth, int myIndex, int inParentIndex)
{
  if(fbxNode->GetNodeAttribute())
  {
    switch (fbxNode->GetNodeAttribute()->GetAttributeType())
    {
    case FbxNodeAttribute::eSkeleton:

      Bone currBone;
      currBone.mName = fbxNode->GetName();
      currBone.mParentIndex = inParentIndex;
      mBoneNodes.push_back(fbxNode);
      mFBXSkeleton.mBones.push_back(currBone);

      break;
    }
  }

  for (int i = 0; i < fbxNode->GetChildCount(); i++)
    ProcessSkeletonHierarchyRecursively(fbxNode->GetChild(i), inDepth + 1, mFBXSkeleton.mBones.size(), myIndex);
}

void FBXExporter::ProcessBindPose()
{
  for(unsigned i = 0; i < mFBXSkeleton.mBones.size(); ++i)
  {
    FbxNode* currNode = mBoneNodes[i];
    int nodeIndex = mBindPose->Find(currNode);

    FbxAMatrix localMatrix;
    if( nodeIndex == -1 )
    {
      //Bone does not have a bind pose export identity
      localMatrix.SetIdentity();
    }
    else
      localMatrix = GetPoseMatrix(mBindPose,nodeIndex);

    mConverter.ConvertMatrix( localMatrix );

    FbxVector4 scale = localMatrix.GetS();
    if( !CheckScaling(scale) || !CheckPositive(scale) )
      localMatrix.SetS( FbxVector4(1,1,1) );

    FbxAMatrix Inverse = localMatrix.Inverse();

    FbxVector4 vec = localMatrix.GetT();
    mFBXSkeleton.mBones[i].mBindTransform.v = FbxVectorToMyVector(vec);
    FbxQuaternion quat = localMatrix.GetQ();
    mFBXSkeleton.mBones[i].mBindTransform.q = FbxQuaternionToMyQuaternion(quat);

    vec = Inverse.GetT();
    mFBXSkeleton.mBones[i].mBoneSpaceTransform.v = FbxVectorToMyVector(vec);
    quat = Inverse.GetQ();
    mFBXSkeleton.mBones[i].mBoneSpaceTransform.q = FbxQuaternionToMyQuaternion(quat);
  }
}

void FBXExporter::ProcessAnimations(unsigned animStackIndex)
{
  Animation* newAnim = new Animation();

  FbxAnimStack* animStack = mFbxScene->GetSrcObject<FbxAnimStack>(animStackIndex);
  newAnim->mName = animStack->GetName();
  FbxTakeInfo* takeInfo = mFbxScene->GetTakeInfo(newAnim->mName.c_str());
  FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
  FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
  newAnim->mDuration = (float)(end - start).GetSecondDouble();  

  newAnim->mTracks.resize(mBoneNodes.size());
  for(unsigned i = 0; i < mBoneNodes.size(); ++i)
    ProcessAnimationTracks(newAnim, mBoneNodes[i], animStack, start, end, i);
  
  mFBXAnimation.push_back(newAnim);
}

void FBXExporter::ProcessAnimationTracks( Animation* anim, FbxNode* animNode, FbxAnimStack* animStack, FbxTime start, FbxTime stop, unsigned trackIndex)
{
  int numAnimLayers = animStack->GetMemberCount<FbxAnimLayer>();

  for(int i = 0; i < numAnimLayers; ++i)
  {
    FbxAnimLayer* currAnimLayer = animStack->GetMember<FbxAnimLayer>(i);

    std::set<FbxTime> keyTimes;
    keyTimes.insert(start);

    ExtractKeyTimes(keyTimes, currAnimLayer, animNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_X);
    ExtractKeyTimes(keyTimes, currAnimLayer, animNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_Y);
    ExtractKeyTimes(keyTimes, currAnimLayer, animNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_Z);

    ExtractKeyTimes(keyTimes, currAnimLayer, animNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X);
    ExtractKeyTimes(keyTimes, currAnimLayer, animNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y);
    ExtractKeyTimes(keyTimes, currAnimLayer, animNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z);

    anim->mTracks[trackIndex].mKeyFrames.resize(keyTimes.size());

    //Iterate through the keyframe set storing the key frame data for each time
    int keyIndex = 0;
    std::set<FbxTime>::iterator it;
    for(it = keyTimes.begin(); it != keyTimes.end(); ++it, ++keyIndex)
    {      
      FbxAMatrix localTransform = GetLocalMatrixFromTime(animNode, *it);

      mConverter.ConvertMatrix(localTransform);

      FbxVector4 scale = localTransform.GetS();
      if(!CheckScaling(scale))
        localTransform.SetS( FbxVector4(1,1,1) );

      KeyFrame newKeyFrame;
      newKeyFrame.Time = (float)it->GetSecondDouble();
      newKeyFrame.LocalTransform.v = FbxVectorToMyVector(localTransform.GetT());
      newKeyFrame.LocalTransform.q = FbxQuaternionToMyQuaternion(localTransform.GetQ());

      anim->mTracks[trackIndex].mKeyFrames[keyIndex] = newKeyFrame;
    }
  } 
}

void FBXExporter::ExtractKeyTimes( std::set<FbxTime>& keyTimes, FbxAnimLayer* currAnimLayer, FbxProperty& attribute, const char* curveName )
{
  FbxAnimCurve* curve = attribute.GetCurve(currAnimLayer, curveName);
  if(curve)
  {
    int keyCount = curve->KeyGetCount();
    for(int i = 0; i < keyCount; ++i)
    {
      FbxAnimCurveKey curveKey = curve->KeyGet(i);
      keyTimes.insert(curveKey.GetTime());
    }
  }
}

FBXExporter* FBXExporter::getInstance()
{
  static FBXExporter* instance = 0;

  if(! instance)
      instance = new FBXExporter();

  return instance;
}

bool FBXExporter::ExportScene(BoostPath& outPath)
{
  if (mMeshType == ONLY_ANIMATION)
    outPath = outPath.replace_extension(AnimExt);
  else
    outPath = outPath.replace_extension(BeanExt);

  ChunkFileWriter writer;    

  bool success = writer.Open(outPath.string());
  if(!success)
  {
    std::cout << "Failed to open " << outPath.string();
    return false;
  }

  unsigned fileChunkStart = writer.StartChunk(FileMark);

  writer.Write(mMeshType);

  if (mMeshType != ONLY_ANIMATION)
  {
    // Export Mesh
    ExportMesh(writer);

    // Export Skeleton
    if (mHasSkeleton)
    {
      ExportSkeleton(writer);
      //ExportAnimations(writer);
    }
  }

  // Export animation only
  else
  {
    ExportAnimations(writer, outPath.filename().string());
  }
  

  writer.EndChunk(fileChunkStart);
  writer.Close();

  DestroyScene();

  return true;
}

void FBXExporter::ExportMesh(ChunkFileWriter& writer)
{
  unsigned meshStartPos = writer.StartChunk(MarkMesh);

  //Write the vertex type to the file
  writer.Write(unsigned(mMeshType));

  writer.Write(mMax);
  writer.Write(mMin);
  writer.Write(mOrigin);

  //Write the Indices
  writer.Write(unsigned(mFBXIndices.size()));
  for (unsigned i = 0; i < mFBXIndices.size(); ++i)
    writer.Write(unsigned(mFBXIndices[i]));

  //Write the Unique Indices
  writer.Write(unsigned(mUniqueIndices.size()));
  for (unsigned i = 0; i < mUniqueIndices.size(); ++i)
    writer.Write(unsigned(mUniqueIndices[i]));

  //Write the Unique Vertices
  writer.Write(unsigned(mUniqueVertices.size()));
  for (unsigned i = 0; i < mUniqueVertices.size(); ++i)
    writer.Write(mUniqueVertices[i]);

  //Write the Vertices
  if(mMeshType == MESH_STATIC)
  {
    writer.Write(unsigned(mFBXVertices.size()));
    for (unsigned i = 0; i < mFBXVertices.size(); ++i)
    {
      FBXVertex v = mFBXVertices[i];

      // position - Vector3
      writer.Write(float(v.position[0]));
      writer.Write(float(v.position[1]));
      writer.Write(float(v.position[2]));

      // color - Vector4
      writer.Write(v.color[0]);
      writer.Write(v.color[1]);
      writer.Write(v.color[2]);
      writer.Write(v.color[3]);

      // uv = Vector2
      writer.Write(float(v.texcoords[0]));
      writer.Write(float(v.texcoords[1]));

      // normal - Vector3
      writer.Write(float(v.normal[0]));
      writer.Write(float(v.normal[1]));
      writer.Write(float(v.normal[2]));

      // tangents - Vector3
      writer.Write(float(v.tangent[0]));
      writer.Write(float(v.tangent[1]));
      writer.Write(float(v.tangent[2]));

      // binormal - Vector3
      writer.Write(float(v.binormal[0]));
      writer.Write(float(v.binormal[1]));
      writer.Write(float(v.binormal[2]));
    }
  }
  
  // Skinned Vertices
  else
  {
    writer.Write(unsigned(mFBXSkinnedVertices.size()));
    for (unsigned i = 0; i < mFBXSkinnedVertices.size(); ++i)
    {
      FBXVertexSkin v = mFBXSkinnedVertices[i];

      // position - Vector3
      writer.Write(v.position[0]);
      writer.Write(v.position[1]);
      writer.Write(v.position[2]);

      // color - Vector4
      writer.Write(v.color[0]);
      writer.Write(v.color[1]);
      writer.Write(v.color[2]);
      writer.Write(v.color[3]);

      // uv = Vector2
      writer.Write(v.texcoords[0]);
      writer.Write(v.texcoords[1]);

      // normal - Vector3
      writer.Write(v.normal[0]);
      writer.Write(v.normal[1]);
      writer.Write(v.normal[2]);

      // tangents - Vector3
      writer.Write(v.tangent[0]);
      writer.Write(v.tangent[1]);
      writer.Write(v.tangent[2]);

      // tangents - Vector3
      writer.Write(v.binormal[0]);
      writer.Write(v.binormal[1]);
      writer.Write(v.binormal[2]);

      writer.Write(v.blendWeights[0]);
      writer.Write(v.blendWeights[1]);
      writer.Write(v.blendWeights[2]);
      writer.Write(v.blendWeights[3]);

      writer.Write(v.blendIndices[0]);
      writer.Write(v.blendIndices[1]);
      writer.Write(v.blendIndices[2]);
      writer.Write(v.blendIndices[3]);
    }
  }

  writer.EndChunk(meshStartPos);
}

void FBXExporter::ExportSkeleton(ChunkFileWriter& writer)
{
  unsigned skelStartChunk = writer.StartChunk(MarkSkel);
  writer.Write(mFBXSkeleton.mBones.size());
  for (unsigned i = 0; i < mFBXSkeleton.mBones.size(); ++i)
  {
    Bone& curBone = mFBXSkeleton.mBones[i];

    writer.Write(curBone.mName);
    writer.Write(curBone.mParentIndex);

    writer.Write(float(curBone.mBindTransform.v[0]));
    writer.Write(float(curBone.mBindTransform.v[1]));
    writer.Write(float(curBone.mBindTransform.v[2]));

    writer.Write(float(curBone.mBindTransform.q[0]));
    writer.Write(float(curBone.mBindTransform.q[1]));
    writer.Write(float(curBone.mBindTransform.q[2]));
    writer.Write(float(curBone.mBindTransform.q[3]));

    writer.Write(float(curBone.mBindTransform.s));

    writer.Write(float(curBone.mBoneSpaceTransform.v[0]));
    writer.Write(float(curBone.mBoneSpaceTransform.v[1]));
    writer.Write(float(curBone.mBoneSpaceTransform.v[2]));

    writer.Write(float(curBone.mBoneSpaceTransform.q[0]));
    writer.Write(float(curBone.mBoneSpaceTransform.q[1]));
    writer.Write(float(curBone.mBoneSpaceTransform.q[2]));
    writer.Write(float(curBone.mBoneSpaceTransform.q[3]));

    writer.Write(float(curBone.mBoneSpaceTransform.s));
  }
  writer.EndChunk(skelStartChunk);
}

void FBXExporter::ExportAnimations(ChunkFileWriter& writer, std::string animName)
{
  unsigned animStartChunk = writer.StartChunk(MarkAnimation);

  // Have to handle multiple animations some how
  writer.Write(unsigned(mFBXAnimation.size()));

  for (unsigned i = 0; i < mFBXAnimation.size(); ++i)
  {
    Animation* currAnim = mFBXAnimation[i];

    writer.Write(animName);
    //writer.Write(currAnim->mName);
    writer.Write(float(currAnim->mDuration));
    writer.Write(unsigned(currAnim->mTracks.size()));

    // Write all the tracks
    for (unsigned j = 0; j < currAnim->mTracks.size(); ++j)
    {
      Track& track = currAnim->mTracks[j];
      writer.Write(unsigned(track.mKeyFrames.size()));

      // Write all the keyframes
      for (unsigned k = 0; k < track.mKeyFrames.size(); ++k)
      {
        KeyFrame& keyFrame = track.mKeyFrames[k];

        writer.Write(float(keyFrame.Time));

        writer.Write(float(keyFrame.LocalTransform.v[0]));
        writer.Write(float(keyFrame.LocalTransform.v[1]));
        writer.Write(float(keyFrame.LocalTransform.v[2]));
        
        writer.Write(float(keyFrame.LocalTransform.q[0]));
        writer.Write(float(keyFrame.LocalTransform.q[1]));
        writer.Write(float(keyFrame.LocalTransform.q[2]));
        writer.Write(float(keyFrame.LocalTransform.q[3]));

        writer.Write(float(keyFrame.LocalTransform.s));
      }
    }
  }
  
  writer.EndChunk(animStartChunk);
}
