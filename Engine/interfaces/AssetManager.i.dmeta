%{
#include "AssetManager.h"
#include "Asset.h"
#include "Mesh.h"
#include "Sound.h"
#include "SkinnedMesh.h"
%}

%rename(GetAssetType) Asset::GetType() const;
class Asset : public IMeta
{
	Asset();
	~Asset();
public:
	std::string GetName();
	std::string GetType() const;
	unsigned int GetUseCounter();
	unsigned GetId() const;
};

class AssetHandle
{
public:
	AssetHandle(Asset* rawPtr);
	AssetHandle(const AssetHandle& rhs);
	AssetHandle();

	bool Valid();
	void ResetHandle();

	unsigned GetId()const;
	std::string GetName()const;
	Asset* GetRawAssetPtr() const;
	std::string GetUnderlyingAssetType() const;
};

class AssetManager : public IMeta
{
	AssetManager();
	~AssetManager();
public:
	void CheckForChangedAssets();
	void PrintLoadedAssets();
	unsigned NumOfAssets() const;

	AssetHandle GetAsset(unsigned assetId);
	AssetHandle GetAsset(std::string assetId);

	static AssetManager* getInstance();
};

//////ASSET TYPES//////

%template(VertexVector)std::vector<Vertex>;
class Mesh : public Asset
{
	Mesh();
	~Mesh();
public:
	std::string GetMeshName();
	std::vector<Vertex>& GetVertices();
	std::vector<int>& GetIndices();
	MeshType GetMeshType();
	unsigned GetVertexCount();
	AABB GetAABB(void);
};