#ifndef CSURFACEINFO_H
#define CSURFACEINFO_H

#include "Component.h"

class CSurfaceInfo : public Component
{
public:
    CSurfaceInfo();
public:
    unsigned int areaType_;
    unsigned int material_;

    ___SERIALIZE_BEGIN___
        ___DEFBASE(Component)
        ___DEFSER(areaType_,1)
        ___DEFSER(material_,1)
        ___SERIALIZE_END___
public:
    metadef(CSurfaceInfo)
        endmetadef;
};

___SERIALIZE_CLASS(CSurfaceInfo,1);
#endif