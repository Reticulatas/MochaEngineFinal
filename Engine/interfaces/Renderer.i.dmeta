%{
#include "Renderer.h"
%}

class Renderer
{
public:

  Math::Vector4 mGlobalAmbient;

  static Renderer* getInstance();	

  void DrawBoxPerma(const Math::Vector3& pos, const Math::Vector3& halflengths, const Color& col = COLOR::WHITE);
  void DrawLine(const Math::Vector3& begin, const  Math::Vector3& end, const Color& col);
  void DrawLinePerma(const Math::Vector3& begin, const  Math::Vector3& end, const Color& col);
};