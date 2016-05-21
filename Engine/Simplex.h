/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */

class Simplex
{
public:
	 Simplex();
	 void Add(Point3& pt);
	 unsigned Size();
	 Point3& GetLast();
	 void Remove(unsigned num);
	 Point3& GetA();
	 Point3& GetB();
	 Point3& GetC();
	

	 unsigned size;
	 Point3 mPoints[4];
};

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Simplex Class