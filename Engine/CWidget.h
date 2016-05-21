/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include <vector>
#include "Component.h"

class GameObject;
class CCamera;
class CWidget : public Component
{
protected:
	std::vector<CWidget*> mySubWidgets;
	typedef std::vector<CWidget*>::iterator widget_iter;
public:
	CWidget(void) : Component() { }
	virtual ~CWidget(void) { }

	void AddSubWidget(CWidget* widget);
	void RemoveSubWidget(CWidget* widget);

	virtual void OnInit() = 0;
	virtual void OnStart() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnFree();
	//called when ray cast checks hit the object and the user clicked the "MouseLClick" bound button
	virtual void OnMouseClick(CCamera*) = 0;
	virtual void OnMouseEnter(CCamera*) = 0;

	metabasedef(CWidget)
		endmetabasedef;
};

