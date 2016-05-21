#include "stdafx.h"
#include "ModelLoader.h"
#include "Renderer.h"
#include "AssetManager.h"

void ModelLoader::FirstLoad()
{

}

void ModelLoader::OnReload()
{

}

bool ModelLoader::LoadBean(Mesh** pMesh, const BoostPath& beanFile)
{
  ChunkReader reader;
  bool success = reader.Open(beanFile.string());
  if(!success)
  {
    std::cout << "Failed to open " << beanFile;
    return false;
  }

  GChunk fileChunk = reader.ReadChunkHeader();

  if (fileChunk.Type != FileMark)
    return false;

  MeshType type;
  reader.Read(type);

  // On first load, have to allocate memory
  if (!(*pMesh))
  {
    if (type == MESH_STATIC)
      *pMesh = new Mesh(beanFile.filename().string());
    else
      *pMesh = new SkinnedMesh(beanFile.filename().string());
  }  

  fileChunk = reader.PeekChunk();

  while (fileChunk.Type != 0)
  {
    switch (fileChunk.Type)
    {
      case MarkMesh:
      {      
        (*pMesh)->BeginMesh();
        ImportMesh(*pMesh, reader);
        (*pMesh)->EndMesh(Renderer::getInstance()->GetD3D11Device());
      }
      break;

      case MarkSkel:
      {
        ImportSkeleton(*pMesh, reader);
      }
      break;

      //case MarkAnimation:
      //{
      //  ImportAnimation(*pMesh, reader);
      //}
      //break;

      default:
        reader.SkipChunk(fileChunk);
      break;
    }

    fileChunk = reader.PeekChunk();
  }

  reader.Close();

  return true;
}


Animation* ModelLoader::LoadAnim(const BoostPath& beanFile)
{
  ChunkReader reader;
  bool success = reader.Open(beanFile.string());
  if (!success)
  {
    std::cout << "Failed to open " << beanFile;
    return false;
  }

  GChunk fileChunk = reader.ReadChunkHeader();
  Animation* anim = new Animation(beanFile.filename().string());

  if (fileChunk.Type != FileMark)
    return false;

  MeshType type;
  reader.Read(type);

  fileChunk = reader.PeekChunk();

  while (fileChunk.Type != 0)
  {
    switch (fileChunk.Type)
    {
    case MarkAnimation:
    {
      ImportAnimation(anim, reader);
    }
      break;

    default:
      reader.SkipChunk(fileChunk);
      break;
    }

    fileChunk = reader.PeekChunk();
  }

  reader.Close();

  return anim;
}


ConvexHullMesh* ModelLoader::LoadHull(const BoostPath& hullFile)
{
	ConvexHullMesh* pHull = new ConvexHullMesh();
	ChunkReader reader;
	bool success = reader.Open(hullFile.string());
	if (!success)
	{
		std::cout << "Failed to open " << hullFile;
		return false;
	}
	GChunk meshChunk = reader.ReadChunkHeader();
	unsigned numOfHulls;
	reader.Read(numOfHulls);

	std::vector<std::vector< float>> listverts;
	std::vector<std::vector< int>> listindices;
	//ConvexHullMesh readInMesh;
	//readInMesh.hullLists.resize(numOfHulls, ConvexHull());

	unsigned numOfFinalHull = 0;

	unsigned numOfVerts;
	unsigned numOfIndices;
	// hulls
	for (unsigned i = 0; i < numOfHulls; ++i)
	{
		// verts
		
		reader.Read(numOfVerts);
		if (numOfVerts == 0)
			continue;
		float* tempVerts = new float[numOfVerts];
		reader.ReadArray<float>(tempVerts, numOfVerts);
		std::vector<float> vertaa;
		for (unsigned i = 0; i < numOfVerts; ++i)
		{
			vertaa.push_back(tempVerts[i]);
		}
		listverts.push_back(vertaa);
		delete[] tempVerts;
		//readInMesh.hullLists[i].verts.resize(numOfVerts);
		//reader.ReadArray<float>(readInMesh.hullLists[i].verts.data(), numOfVerts);

		// indices
		
		reader.Read(numOfIndices);
		if (numOfIndices == 0)
			continue;

		std::vector< int> tempIndices(numOfIndices);
		reader.ReadArray<int>(tempIndices.data(), numOfIndices);
		listindices.push_back(tempIndices);
		//readInMesh.hullLists[i].indices.resize(numOfIndices);
		
		//reader.ReadArray<int>(listindices[i].data(), numOfIndices);
		++numOfFinalHull;
	}

	// create final result
	for (unsigned i = 0; i < numOfFinalHull; ++i)
	{
		if (listindices[i].size() == 0 || listverts[i].size() == 0)
			continue;
		ConvexHull ch;
		ch.indices = listindices[i];
		ch.verts = listverts[i];
		pHull->hullLists.push_back(ch);
	}
	//pHull->hullLists.resize(numOfFinalHull, ConvexHull());
	//
	//// remove size == 0
	//numOfFinalHull = 0;

	//for (auto mesh : readInMesh.hullLists)
	//{
	//	if (mesh.indices.size() == 0 || mesh.verts.size() == 0)
	//		continue;
	//	pHull->hullLists[numOfFinalHull].indices = mesh.indices;
	//	pHull->hullLists[numOfFinalHull].verts = mesh.verts;
	//	++numOfFinalHull;
	//	int i = 0;
	//}
	

	return pHull;
}

void ModelLoader::ImportMesh(Mesh* mesh, ChunkReader& reader)
{
  GChunk meshChunk = reader.ReadChunkHeader();

  MeshType meshType;
  reader.Read(meshType);

  reader.Read(mesh->mMeshAABB.maxPoint);
  reader.Read(mesh->mMeshAABB.minPoint);
  reader.Read(mesh->mOrigin);

  // Indices
  reader.Read(mesh->mIndexCount);
  mesh->mIndices.resize(mesh->mIndexCount);
  reader.ReadArray(mesh->mIndices.data(), mesh->mIndexCount);

  // Unique Indices for physics
  unsigned uniqueIndicesCount;
  reader.Read(uniqueIndicesCount);
  mesh->mUniqueIndices.resize(uniqueIndicesCount);
  reader.ReadArray(mesh->mUniqueIndices.data(), uniqueIndicesCount);

  // Unique Vertices for physics
  unsigned unqiueVertCount;
  reader.Read(unqiueVertCount);
  mesh->mUnqiueVertices.resize(unqiueVertCount);
  reader.ReadArray(mesh->mUnqiueVertices.data(), unqiueVertCount);

  // Verts
  reader.Read(mesh->mVertexCount);
  if (meshType == MESH_STATIC)
  {
    // Static Verts
    mesh->mVertices.resize(mesh->mVertexCount);
    reader.ReadArray(mesh->mVertices.data(), mesh->mVertexCount);
  }
  else
  {
    // Skinned Verts
    SkinnedMesh* sMesh = static_cast<SkinnedMesh*>(mesh);
    sMesh->mSkinnedVertices.resize(mesh->mVertexCount);
    reader.ReadArray(sMesh->mSkinnedVertices.data(), mesh->mVertexCount);
  }
}

void ModelLoader::ImportSkeleton(Mesh* mesh, ChunkReader& reader)
{
  SkinnedMesh* sMesh = static_cast<SkinnedMesh*>(mesh);
  if (!sMesh->mSkeleton)
    sMesh->mSkeleton = new Skeleton();

  GChunk skeletonChunk = reader.ReadChunkHeader();

  unsigned numBones = 0;
  reader.Read( numBones );

  sMesh->mSkeleton->mBones.resize(numBones);
  for(unsigned i = 0; i < numBones; ++i)
  {
    Bone& bone = sMesh->mSkeleton->mBones[i];
    reader.Read( bone.mName );
    reader.Read( bone.mParentIndex );
    reader.Read( bone.mBindTransform );
    reader.Read( bone.mBoneSpaceTransform );
  }
}

void ModelLoader::ImportAnimation(Animation* anim, ChunkReader& reader)
{
  GChunk animation = reader.ReadChunkHeader();

  // Read the number of animations
  unsigned numAnimations = 0, numTracks = 0, numKeyframes = 0;
  reader.Read(numAnimations);

  // Should be only 1 animation
  if (numAnimations != 1)
    std::cout << "Error on animation loading: Should have only 1 anim per file\n";

  for(unsigned i = 0; i < numAnimations; ++i)
  {
    std::string inName;
    reader.Read(inName);

    //reader.Read(anim->mName);
    anim->mTrackName = inName;
    reader.Read(anim->mDuration);

    // Read all the tracks
    reader.Read(numTracks);
    anim->mTracks.resize(numTracks);
    for(unsigned j = 0; j < numTracks; ++j)
    {
      reader.Read(numKeyframes);
      
      anim->mTracks[j].mKeyFrames.resize(numKeyframes);
      reader.ReadArray(anim->mTracks[j].mKeyFrames.data(), numKeyframes);
    }
  }
}

ModelLoader* ModelLoader::getInstance()
{
  static ModelLoader* instance = 0;

  if (!instance)
    instance = new ModelLoader();

  return instance;
}
