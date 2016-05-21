%{
#include "FrameController.h"
%}

class FrameController
{
	FrameController();
	~FrameController();
public:
	float GetDeltaTime();
	void CapOn(float fps);
	void CapOff();
	float FPSMax() const;

	static FrameController* getInstance(); // singleton
	static float DT();
};