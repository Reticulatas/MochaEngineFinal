%{
#include "CArtifactFX.h"
%}

class CArtifactFX : public CPostProcess
{
    CArtifactFX();
	  ~CArtifactFX();

  public:

    float mBlockThreshold;
    float mLineThreshold;
};
