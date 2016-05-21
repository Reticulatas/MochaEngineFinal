#ifndef CAUDIOGEOMETRY
#define CAUDIOGEOMETRY

#include "Component.h"

class CAudioGeometry : public Component
{
public:
    CAudioGeometry();
    ~CAudioGeometry();
    void OnInit(void);
    void OnUpdate(void);
    void OnFree(void);
private:
    COMPONENTCLONE(CAudioGeometry);

    ___SERIALIZE_BEGIN___
        ___DEFBASE(Component)
        ___SERIALIZE_END___

public:
    metadef(CAudioGeometry)
        endmetadef;
};

___SERIALIZE_CLASS(CAudioGeometry,1);
#endif