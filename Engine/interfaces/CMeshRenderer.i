%{
#include "CMeshRenderer.h"
%}

class CMeshRenderer : public CRenderer 
{
public:

	virtual void SetMesh(AssetHandle& meshHandle);
};