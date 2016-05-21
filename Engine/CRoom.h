#ifndef CROOM_H
#define CROOM_H

#include "Component.h"
#include <fmod.hpp>

// used for reverb calculation and occlusion
class CRoom : public Component
{
public:
    CRoom();
    void OnInit();
private:
    FMOD::Geometry 
};

#endif