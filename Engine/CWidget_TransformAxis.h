/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "CWidget.h"
#include "CDiffuseRenderer.h"

class CWidget_Transform;
class Widget_TransformAxis : public CWidget
{
public:
	enum WidgetMode
	{
		TRANSLATE,
		SCALE,
		ROTATE
	};
	enum WidgetAxis
	{
		X,
		Y,
		Z,
        BASE
	};
	enum WidgetSpace {
		LOCAL,
		GLOBAL
	};

private:
	WidgetAxis mAxis;
	WidgetSpace mSpace;
	WidgetMode mMode;
	bool isDragging;
	CCamera* cameraDragging;
	CDiffuseRenderer* myRenderer;
	CWidget_Transform* myParent;
	Vector3      parentLookAt;
	Vector3      parentUp;
	Vector3      parentRight;
	Vector3      oldPos;
	bool lastFrameMouseOver;
	bool isGlobal;
	bool hasSwapped;

	Color originalColor;			//the color while this widget is not dragged and not moused over
	Color draggingColor;			//the color while this widget is being dragged
	Vector3 offsetPoint;

    Vector2 initialDragPoint;
public:
	Widget_TransformAxis(WidgetAxis, WidgetMode, CWidget_Transform* parent);
	virtual void OnInit();
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFree();
	virtual void OnMouseClick(CCamera* cam);
	virtual void OnMouseEnter(CCamera* cam);

	void ToggleWidgetSpace(void);



	void Rescale(GameObject*);
	bool IsDragging() const { return isDragging; }
	void SetSpace(WidgetSpace space);

	virtual ::Component* Clone(GameObject*) const { throw std::exception("Object can not be cloned"); }

	metadef(Widget_TransformAxis)
		endmetadef;

private:
	void ResetXRotationWidget(void);
	void ResetYRotationWidget(void);
	void ResetZRotationWidget(void);
};
