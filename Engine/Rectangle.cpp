/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Rectangle.h"

float Rect::GetX() { return x; }
float Rect::GetY() { return y; }
float Rect::GetWidth() { return width; }
float Rect::GetHeight() { return height; }

float Rect::GetTopLeftPointX() { return x; }
float Rect::GetTopLeftPointY() { return y; }

float Rect::GetBottomRightPointX()
{
	return x + width;
}

float Rect::GetBottomRightPointY()
{
	return y + height;
}

void Rect::Set( float x_, float y_, float width_, float height_ )
{
	x = x_;
	y = y_;
	width = width_;
	height = height_;
}

void Rect::Scale( float sx, float sy )
{
	width *= sx;
	height *= sy;
}

Rect::Rect( float x,float y,float width,float height )
{
	Set(x,y,width,height);
}

/*
Rect::Rect( MPoint2 topLeft,MPoint2 bottomRight )
{
	Set(topLeft.x,topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
}*/

Rect::Rect( void )
{
	x = y = width = height = 0;
}
