#include "stdafx.h"
#include "CSurfaceInfo.h"
#include "AreaAttributes.h"

CSurfaceInfo::CSurfaceInfo()
    :   areaType_(AT_SAFE),
        material_(SM_CONCRETE)
{
}

meta_define(CSurfaceInfo);