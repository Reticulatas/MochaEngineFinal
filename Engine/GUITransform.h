#pragma once
#include "Component.h"

class GUITransform : public Component
{
	void reposition();

public:
	virtual void OnStart(void) override;
	virtual void OnInit(void) override;
	virtual void OnUpdate(void) override;
	virtual void OnAlwaysUpdate(void) override;
	virtual void OnFree(void) override;
	virtual void OnEditorUpdate(void) override;

	//all values normalized relative to parent (canvas, frame, other control, etc)
	float x, y;
	float width, height;
};

