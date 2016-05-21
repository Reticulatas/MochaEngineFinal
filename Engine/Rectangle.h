/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

class Rect
{
public:
	float x,y,width,height;

	float GetX();
	float GetY();
	float GetWidth();
	float GetHeight();
	float GetTopLeftPointX();
	float GetTopLeftPointY();
	float GetBottomRightPointX();
	float GetBottomRightPointY();

	void Set(float x, float y, float width, float height);
	void Scale(float sx, float sy);

	Rect(void);
	Rect(float x,float y,float width,float height);
	//Rect(MPoint2 topLeft,MPoint2 bottomRight);
};

//Nicholas Fuller