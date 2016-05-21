/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */

#include "stdafx.h"
#include "Simplex.h"

Simplex::Simplex() : size(0)
{}

void Simplex::Add(Point3& pt)
{
	mPoints[size] = pt;
	++size;
}

unsigned Simplex::Size()
{
	return size;
}

Point3& Simplex::GetLast()
{
	return mPoints[size-1];
}

Point3& Simplex::GetA()
{
    return mPoints[2];
}

Point3& Simplex::GetB()
{
	return mPoints[1];
}
Point3& Simplex::GetC()
{
	return mPoints[0];
}

void Simplex::Remove(unsigned num)
{
	for (unsigned i = num; i < size-1; ++i)
		mPoints[num] = mPoints[num+1];
	--size;
}

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Simplex Class