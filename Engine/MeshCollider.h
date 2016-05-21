#pragma once
#include "ColliderCom.h"
class CMeshColliderComponent
	: public ColliderComponent
{
public:
	void OnFree(void);
	void OnInit(void);
	void OnStart(void);
	void OnUpdate(void);

	const static std::string ID;

	CMeshColliderComponent(void);
	//virtual MeshColliderComponent3D * Clone(void) const;

	CMeshColliderComponent &SetMesh(const std::string &file);
	virtual const Vector3& UpdateSize(const Vector3 &size);
	bool mHasHull;
	unsigned mIndexOfHull;
	AssetHandle mHulls;
	std::string nameOfHull;
protected:

	virtual Collider *CreateCollider(RigidBody &body);

private:

	std::string m_file;
	bool mHasMesh;
	
public:
	___BEGIN_REGISTERED___
	___END_REGISTERED___
	___SERIALIZE_BEGIN___
		___DEFBASE(ColliderComponent)
		___DEFSER(m_file, 1)
		___DEFSER(mHasMesh, 2)
		___DEFSER(mHasHull, 2)
	___SERIALIZE_END___
	virtual void CopyFrom(const ::CMeshColliderComponent* c)
	{
		m_file = c->m_file;
		mHasHull = c->mHasHull;
		mHasMesh = c->mHasMesh;
	}
	metadef(CMeshColliderComponent)
		m_addbase(ColliderComponent)
		endmetadef;
	COMPONENTCLONE(CMeshColliderComponent);
};
___SERIALIZE_CLASS(CMeshColliderComponent, 2);