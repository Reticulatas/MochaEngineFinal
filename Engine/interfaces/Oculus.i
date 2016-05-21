
%{
#include "Oculus.h"
%}

class OculusVR
{
public:
	
  bool IsConnected();
  unsigned GetRecommendedWidth();
  unsigned GetRecommendedHeight();

  static OculusVR* getInstance();
  
  Math::Vector3 GetEyePos(unsigned index);
};