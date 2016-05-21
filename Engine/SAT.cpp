/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */

#include "stdafx.h"
#include "SAT.h"


#define EPSLON 0.01
void SAT::FlipNormal(Vector* normal)
{
	float temp = -normal->x;
	normal->x = normal->y;
	normal->y = temp;
}
bool SAT::IsIndexInA(OBB* OBBa, Point4 pt)
{
  static Vector vec,normal;
  int next;
  float temp, res;
  for (int i = 0; i < 4; ++i)
  {
    if(i == 3)
      next = 0;
    else
      next = i+1;
    normal = OBBa->mPoints[next] - OBBa->mPoints[i];
    temp = normal.x;
    normal.x = normal.y;
    normal.y = -temp;
    normal.z = 0;
    vec = pt - OBBa->mPoints[i];
    res = normal.Dot(vec);
    if (res > EPSLON)
      return false;
  }
  return true;
}
void SAT::FindPreNextIndex(int index, int* prev, int* next)
{
  if(index == 3)
    *next = 0;
  else
    *next = index + 1;

  if(index == 0)
    *prev = 3;
  else
    *prev = index - 1;
}
float SAT::OBBFindX(Vector* v1, Vector* v2, Vector* v3, Vector* v4, Option X)
{
  Vector vertexs[] = {*v1, *v2, *v3, *v4};
  Vector ret = *v1;
  if (X == Option::MaxNum)
  {
    for (int i = 1; i < 4; ++i)
    {
      if(vertexs[i].x > ret.x)
        ret = vertexs[i];
    }
  }
  else
  {
    for (int i = 1; i < 4; ++i)
    {
      if(vertexs[i].x < ret.x)
        ret = vertexs[i];
    }
  }
  return ret.x;
}
float SAT::OBBFindY(Vector* v1, Vector* v2, Vector* v3, Vector* v4, Option Y)
{
  Vector vertexs[] = {*v1, *v2, *v3, *v4};
  Vector ret = *v1;
  if (Y == Option::MaxNum)
  {
    for (int i = 1; i < 4; ++i)
    {
      if(vertexs[i].y > ret.y)
        ret = vertexs[i];
    }
  }
  else
  {
    for (int i = 1; i < 4; ++i)
    {
      if(vertexs[i].y < ret.y)
        ret = vertexs[i];
    }
  }
  return ret.y;
}

/*
std::pair<float,float> OBBFindMaxMinVertexs(OBB* Box,Option X, Option Y)
{
  float BoxhalfExtX = Box->GetHalfExtend().x;
  float BoxhalfExtY = Box->GetHalfExtend().y;
  Vector BoxV1, BoxV2, BoxV3, BoxV4;
  Vector NewBoxV1, NewBoxV2, NewBoxV3, NewBoxV4;
  std::pair<float,float> ret;
  BoxV1 = BoxV2 = BoxV3 = BoxV4 = XMVectorSet(0,0,0,0);

  BoxV1.x += BoxhalfExtX; BoxV1.y += BoxhalfExtY;
  BoxV2.x += BoxhalfExtX; BoxV2.y -= BoxhalfExtY;
  BoxV3.x -= BoxhalfExtX; BoxV3.y -= BoxhalfExtY;
  BoxV4.x -= BoxhalfExtX; BoxV4.y += BoxhalfExtY;

  XMMATRIX rotation = XMMatrixIdentity();
  XMVECTOR BoxUaxis = Box->GetAxisU();  
  XMVECTOR BoxVaxis = Box->GetAxisV();
  rotation._11 = BoxUaxis.x;
  rotation._12 = BoxUaxis.y;
  rotation._21 = BoxVaxis.x;
  rotation._22 = BoxVaxis.y;

  NewBoxV1 = XMVector4Transform(BoxV1,rotation);
  NewBoxV2 = XMVector4Transform(BoxV2,rotation);
  NewBoxV3 = XMVector4Transform(BoxV3,rotation);
  NewBoxV4 = XMVector4Transform(BoxV4,rotation);

  float retX = OBBFindX(&NewBoxV1,&NewBoxV2,&NewBoxV3,&NewBoxV4,X);
  float retY = OBBFindX(&NewBoxV1,&NewBoxV2,&NewBoxV3,&NewBoxV4,Y);
  
    ret.first = abs(retX) ;

    ret.second = abs(retY);

  return ret;
}*/

int SAT::FindSmall(float* proj, float* xfactor, bool*IsInBox)
{
  //float smallest = proj[0];
  float smallest = FLT_MAX;
  //int index = 0;
  int index = -1;
  for (int i = 0; i < 4; ++i)
  {
    //if(IsInBox[i] == false)
      //continue;
    if(proj[i] < smallest)
    {
      smallest = proj[i];
      index = i;
    }
    else if(abs(proj[i] - smallest) < EPSLON && abs(proj[i] - smallest) >= 0)
    {
      if(xfactor[i] < xfactor[index])
      {
        smallest = proj[i];
        index = i;
      }
    }
  }
  return index;
}
int SAT::FindSmallIndex(Point3 * aPoint1,Point3 * aPoint2,Point3 * aPoint3,Point3 * aPoint4,
                    float halfX, float halfY, Axis axis, Vector* normal,OBB *b)
{
  Point3 pts[4];
  Axis opsAxis;
  float lenth;
  float projection[4], xfactor[4];
  pts[0] = *aPoint1;
  pts[1] = *aPoint2;
  pts[2] = *aPoint3;
  pts[3] = *aPoint4;
  if(axis == X)
  {
    opsAxis = Y;
    lenth = b->GetHalfExtendX().x;
  }
  else
  {
    opsAxis = X;
    lenth = b->GetHalfExtendX().y;
  }

  for (int i = 0; i < 4; ++i)
  {
    projection[i] = pts[i].Dot(*normal);
    if(axis == X)
      xfactor[i] = abs(pts[i].y);
    else
      xfactor[i] = abs(pts[i].x);
  }



  float smallest = projection[0];
  int index = 0;
  for (int i = 1; i < 4; ++i)
  {
    if(projection[i] < smallest)
    {
      smallest = projection[i];
      index = i;
    }
    else if(abs(projection[i] - smallest) < EPSLON && abs(projection[i] - smallest) >= 0)
    {
      if(xfactor[i] < xfactor[index])
      {
        smallest = projection[i];
        index = i;
      }
    }
  }
  return index;
}
bool SAT::ProjectAToBLeastPenetration(Point3* aPoint1,Point3* aPoint2,Point3* aPoint3,Point3* aPoint4,
                                 Vector* normalAxis, OBB* b,  Axis* axis, 
                                 Vector* normal,Point3* contactPoint,float* penetration, 
                                 int* indexA, int* indexB, int refB, Direction* dir, Direction* result, ContactInfo* info,
                                 int listCounter)
{
  /*   V1      V4  */
  /*   --------    */
  /*   |   .  |    */
  /*   --------    */
  /*   V2      V3  */
  static Point3 aPoints[4];
  int leastindex;
  float project[4], lenth, smallestPoint, potentialPen;
  float xfactor[4];
  bool IsInBox[4];
  float width, height;
  width = b->GetHalfExtendX().x;;
  height = b->GetHalfExtendY().y;
  for (int i = 0; i < 4; ++i)
  {
    IsInBox[i] = true;
  }
  if(*axis == X)
    lenth = b->GetHalfExtendX().x;
  else
    lenth = b->GetHalfExtendY().y;

  aPoints[0] = *aPoint1;
  aPoints[1] = *aPoint2;
  aPoints[2] = *aPoint3;
  aPoints[3] = *aPoint4;

  for (int i = 0; i < 4; ++i)
  {
    
    project[i] = aPoints[i].Dot(*normalAxis);
    if(abs(aPoints[i].x) > width ||abs(aPoints[i].y) > height)
      IsInBox[i] = false;
    if(*axis == X)
    {
      xfactor[i] = abs(aPoints[i].y);
    }
    else
    {
      xfactor[i] = abs(aPoints[i].x);
    }

  }

  leastindex = FindSmall(project, xfactor,IsInBox);
  if(leastindex == -1)
    return false;
  smallestPoint = project[leastindex];

  if(smallestPoint > lenth)
  {
    //printf("No collide\n\n\n");
    return false;
  }
  else
  {
    potentialPen = lenth - smallestPoint;
    //if(potentialPen < *penetration)
    //{
      *penetration = potentialPen;
      info[listCounter].normal = b->mRotation * (*normalAxis);
      info[listCounter].contactPoint = b->mWorld * aPoints[leastindex];
      info[listCounter].penetration = potentialPen;
      info[listCounter].indexA = leastindex;
      info[listCounter].indexB = refB;
      info[listCounter].direction = *dir;
      info[listCounter].IsInBox = IsInBox[leastindex];
      /*
      *normal = b->mRotation * (*normalAxis);
      *contactPoint = b->mWorld * aPoints[leastindex];
      *penetration = potentialPen;
      *indexA = leastindex;
      *result = dir;
      *indexB = refB;
      */
    //}
    return true;
  }
}
Point3 SAT::FindIntersectPointReturnGarbage(Point3*p1,Point3*p2,Point3*p3,Point3*p4 )
{
  Point3 ret;
  Vector slope1 = *p1 - *p2;
  Vector slope2 = *p3 - *p4;
  float a1,b1,c1,a2,b2,c2;
  a1 = p2->y - p1->y;
  b1 = p1->x - p2->x;
  c1 = a1 * p1->x + b1 * p1->y;

  a2 = p4->y - p3->y;
  b2 = p3->x - p4->x;
  c2 = a2 * p3->x + b2 * p3->y;

  float delta = a1 * b2  - a2 * b1;

  if(delta == 0)
  {
    ret.Set(-1000.0f,-1000.0f,-1000.0f);
    return ret;
  }

  float x = (b2*c1 - c2*b1) / delta;
  float y = (a1*c2 - a2*c1)/delta;

  ret.x = x;
  ret.y = y;
  ret.z = 0.0f; 
  ret.w = 1.0f;
  return ret;
}
Point3 SAT::FindIntersectPoint(Point3*p1,Point3*p2,Point3*p3,Point3*p4 )
{
  static Point3 ret;
  static Vector slope1;
  slope1 = *p1 - *p2;
  static Vector slope2;
  slope2 = *p3 - *p4;
  float a1,b1,c1,a2,b2,c2;
  a1 = p2->y - p1->y;
  b1 = p1->x - p2->x;
  c1 = a1 * p1->x + b1 * p1->y;

  a2 = p4->y - p3->y;
  b2 = p3->x - p4->x;
  c2 = a2 * p3->x + b2 * p3->y;

  float delta = a1 * b2  - a2 * b1;

  if(delta == 0)
    int i = 0;

  float x = (b2*c1 - c2*b1) / delta;
  float y = (a1*c2 - a2*c1)/delta;
  
  ret.x = x;
  ret.y = y;
  ret.z = 0.0f; 
  ret.w = 1.0f;
  return ret;
}
float SAT::PosOfPointsCompVec(Point3* mpoint,Point3* mpointonline, Vector* mvector)
{
  static Point3 pointOnLine;
  pointOnLine = *mpointonline;
  static Point3 point;
  point = *mpoint;
  static Vector vector;
  vector = *mvector;
  float res = vector.Dot(point -pointOnLine);
  return res;

}
int SAT::NumberOfPointsIntersect(ContactsManager *cm, OBB* OBBa, OBB* OBBb)
{
  static Contacts c;
  c = cm->ContactList[cm->NumContacts];
  static Vector normal;
  normal = c.normal;
  int counter = 0;
  int indexB = c.indexB;
  static Point4 ptB;
  ptB = OBBb->mPoints[indexB];
  
  float res;
  for (int i = 0; i < 4; ++i)
  {

	 res = (OBBa->mPoints[i] - ptB).Dot(normal);
    //res = PosOfPointsCompVec(&OBBa->mPoints[i],&ptB,&normal);
     if( res<= EPSLON)
        counter++;
  }
  return counter;

}

void SAT::TwoUnderNormalFindPoints(int indexA,int indexB,  OBB* OBBa,OBB* OBBb,Vector*normal,
                                Point3*ptA1, Point3*ptA2,Point3*ptA3,Point3*ptA4)
{
  Point3 point;
  int i = 0, next, prev,now, pair1, pair2;
  FindPreNextIndex(indexA, &prev, &next); 
  // check contact points are inside or not
  if(PosOfPointsCompVec(&OBBa->mPoints[prev], &OBBb->mPoints[indexB],normal) > 0.001f)
  {
    point = OBBa->mPoints[next];
    prev = -1;

  }
  else
  {
    point = OBBa->mPoints[prev];
    next = -1;
  }
  if(next == -1)
  {
    now = prev + 1;
    if(now == 4)
      now = 0;
  }
  else if(prev == -1)
  {
    now =  next - 1;
    if(now == -1)
      now = 3;
  }

  if(next == -1)
  {
    pair1 = prev - 1;
    pair2 = now + 1;
    if(pair1 == -1)
      pair1 = 3;
    if(pair2 == 4)
      pair2 = 0;

   // if(pair1 == prev || pair2 == now)
      //_asm int 3;

    *ptA1 = OBBa->mPoints[now];
    *ptA2 = OBBa->mPoints[pair2];
    *ptA3 = OBBa->mPoints[prev];
    *ptA4 = OBBa->mPoints[pair1];
  }
  else if(prev == -1)
  {
    pair1 = next + 1;
    pair2 = now - 1;
    if(pair1 == 4)
      pair1 = 0;
    if(pair2 == -1)
      pair2 = 3;

    //if(pair1 == next || pair2 == now)
     // _asm int 3;

    *ptA1 = OBBa->mPoints[now];
    *ptA2 = OBBa->mPoints[pair2];
    *ptA3 = OBBa->mPoints[next];
    *ptA4 = OBBa->mPoints[pair1];
  }
}

void SAT::FlipXY(Vector*mnormal)
{
  mnormal->z = mnormal->x;
  mnormal->x = mnormal->y;
  mnormal->y = -mnormal->z;
  mnormal->z = 0;
}

PointsUnderNormal SAT::TwoUnderNormalCheck(int indexA,int indexB,  OBB* OBBa,OBB* OBBb,Vector*mnormal)
{
	float Epsolon = 0.001f;
  int counter = 0;
  static Vector normal = *mnormal;
  static Point3 normalPtA , normalPtB, OBBaPtA, OBBaPtB, firstTest,secTest;
  int preA, nextA, preB, nextB, nextnextB, dummy;// dont need preB
  FindPreNextIndex(indexA, &preA, &nextA);
  FindPreNextIndex(indexB, &preB, &nextB);
  FindPreNextIndex(nextB, &dummy, &nextnextB);
  normalPtA = OBBb->mPoints[indexB];
  normalPtB = OBBb->mPoints[nextB];

  OBBaPtA = OBBa->mPoints[indexA];
  if(PosOfPointsCompVec(&OBBa->mPoints[nextA], &normalPtA, &normal) < EPSLON)
    OBBaPtB = OBBa->mPoints[nextA];
  else if(PosOfPointsCompVec(&OBBa->mPoints[preA], &normalPtA, &normal) < EPSLON)
    OBBaPtB = OBBa->mPoints[preA];
  //else
	  //_asm int 3;
  static Vector testNormal1;
  testNormal1 = OBBb->mPoints[indexB] - OBBb->mPoints[preB];
  static Vector testNormal2;
  testNormal2 = OBBb->mPoints[nextnextB] - OBBb->mPoints[nextB];
  FlipXY(&testNormal1);
  FlipXY(&testNormal2);

  float test1N1 = PosOfPointsCompVec(&OBBaPtA, &OBBb->mPoints[indexB], &testNormal1);
  float test1N2 = PosOfPointsCompVec(&OBBaPtA, &OBBb->mPoints[nextB], &testNormal2);
  float test2N1 = PosOfPointsCompVec(&OBBaPtB, &OBBb->mPoints[indexB], &testNormal1);
  float test2N2 = PosOfPointsCompVec(&OBBaPtB, &OBBb->mPoints[nextB], &testNormal2);
  if(test1N1 > EPSLON || test1N2 > EPSLON)// pts on line is inside
	  counter++;
  else if(test2N1 > EPSLON || test2N2 > EPSLON)
	  counter++;
  /*
  // take two contact pts dot with normal to find the farer one
  Vector dotwithnormal = OBBaPtA - OBBaPtB;
  if(dotwithnormal.Dot(testNormal) < 0)
  {
    firstTest = OBBaPtB;
    secTest = OBBaPtA;
  }
  else
  {
    firstTest = OBBaPtA;
    secTest = OBBaPtB;

  }

  Vector contactToPt = firstTest - normalPtA;
  if(contactToPt.Dot(testNormal) < 0)
    ++counter;


  contactToPt = secTest - normalPtB;
  testNormal.x= -testNormal.x;testNormal.y= -testNormal.y;
  if(contactToPt.Dot(testNormal) < 0)
    ++counter;
	*/
  if(counter == 0)
  {
   // printf("BOTH INSIDE\n\n");
    return BothInside;
  }
  else if(counter == 1)
  {
    //printf("ONE INSIDE\n\n");
    return OneInOneOut;
  }
  else
  {
    //printf("OUTTTTTT\n\n");
    return BothOutside;
  }
}
void SAT::TwoUnderNormalFindPointsFlip(int indexA,int indexB,  OBB* OBBa,OBB* OBBb,Vector*normal,
                              Point3*ptA1, Point3*ptA2,Point3*ptA3,Point3*ptA4,
                              Point3* normalA, Point3* normalB)
{
  Point3 point;
  int i = 0, next, prev;
  int preB, nextB;
  FindPreNextIndex(indexA, &prev, &next); 
  FindPreNextIndex(indexB, &preB, &nextB);
  // check contact points are inside or not
  if(PosOfPointsCompVec(&OBBa->mPoints[prev], &OBBb->mPoints[indexB],normal) < EPSLON)
    *normalB = OBBa->mPoints[prev];
  else
    *normalB = OBBa->mPoints[next];

  *normalA = OBBa->mPoints[indexA];
  
  int pt1, pt2, pt3, pt4;

  pt1 = indexB;
  if(indexB == 0)
    pt2 = 3;
  else
    pt2 = indexB - 1;

  if(indexB == 3)
    pt3 = 0;
  else
    pt3 = indexB + 1;

  if(pt3 == 3)
    pt4 = 0;
  else
    pt4 = pt3 + 1;
  *ptA1 = OBBb->mPoints[indexB];
  *ptA2 = OBBb->mPoints[pt2];
  *ptA3 = OBBb->mPoints[pt3];
  *ptA4 = OBBb->mPoints[pt4];
}
Point3 SAT::FindPointAlongNormalByPenetration(Point3 oriCP,Vector normal, float penetration)  // cp1mB
{
  static Point3 cp1mB;
  cp1mB = oriCP + normal * penetration;
  
  return cp1mB;
}
Point3 SAT::FindSecondPair(Point3 oriCP, Point3 cp1,Point3 cp2) // the same point but different in modle space
{
  Vector normal;
  normal = cp1 - cp2;
  Vector v1;
  v1 = cp1 - oriCP;
  Vector v2;
  v2 = cp2 - oriCP;
  if(abs(v1.Dot(normal)) > abs(v2.Dot(normal)))
    return cp1;
  else
    return cp2;
}

void SAT::UpdateContacPoints(ContactsManager *cm, OBB* OBBa, OBB* OBBb)
{
  int indexA = cm->ContactList[cm->NumContacts].indexA;
  int indexB = cm->ContactList[cm->NumContacts].indexB;
  static Point3 prePtA, nextPtA, indexPtA,nextnextPtA,prePtB, nextPtB, indexPtB,nextnextPtB;
  int nextA, nextnextA, preA, nextB, nextnextB, preB, dummy;
  FindPreNextIndex(indexA, &preA,&nextA);
  indexPtA = OBBa->mPoints[indexA];
  prePtA = OBBa->mPoints[preA];
  nextPtA = OBBa->mPoints[nextA];
  FindPreNextIndex(nextA,&dummy, &nextnextA);
  nextnextPtA = OBBa->mPoints[nextnextA];

  FindPreNextIndex(indexB, &preB, &nextB);
  indexPtB = OBBb->mPoints[indexB];
  prePtB = OBBb->mPoints[preB];
  nextPtB = OBBb->mPoints[nextB];
  FindPreNextIndex(nextB,&dummy, &nextnextB);
  nextnextPtB = OBBb->mPoints[nextnextB];
  // test
//  if(cm->ContactList[cm->NumContacts].contacts[0]->mIsDynamic == false && 
	  //cm->ContactList[cm->NumContacts].contacts[1]->mIsDynamic == false)
	 // int i = 0;
  //test
  int numOfPoints = NumberOfPointsIntersect(cm,OBBa,OBBb);
  Contacts c = cm->ContactList[cm->NumContacts];

  static Vector contactPt;
  contactPt = cm->ContactList[cm->NumContacts].contactPoint;
  static Vector normal;
  normal =  cm->ContactList[cm->NumContacts].normal;
  float penetration = cm->ContactList[cm->NumContacts].penetration;

  if(numOfPoints == 1)
  {
    static Vector contactPtOld;
	  contactPtOld = cm->ContactList[cm->NumContacts].contactPoint;
    cm->ContactList[cm->NumContacts].numberOfcontacts = 1;
    bool IsIndexBinA, IsNextBinA;
    IsIndexBinA = IsIndexInA(OBBa, indexPtB);
    IsNextBinA  = IsIndexInA(OBBa, nextPtB); 
    if(IsIndexBinA)
    {
      //printf("1 point, indexB in A\n\n\n");
      cm->ContactList[cm->NumContacts].contactPoint  =  FindIntersectPoint(&indexPtA, &nextPtA, &indexPtB,&prePtB );
      cm->ContactList[cm->NumContacts].contactPoint2 =  FindIntersectPoint(&indexPtA, &prePtA,  &indexPtB,&nextPtB);
      cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld * indexPtA;//
      cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2; //
      cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;
      cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld * indexPtB;
    }
    else if(IsNextBinA)
    {
      //printf("1 point, nextB in A\n\n\n");
      cm->ContactList[cm->NumContacts].contactPoint  =  FindIntersectPoint(&indexPtA, &nextPtA, &indexPtB,&nextPtB);
      cm->ContactList[cm->NumContacts].contactPoint2 =  FindIntersectPoint(&indexPtA, &prePtA,  &nextPtB ,&nextnextPtB);
      cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld * indexPtA;//
      cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint; //
      cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;
      cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld * nextPtB;
    }
    else
    {
      //printf("1 point, Normal\n\n\n");
      cm->ContactList[cm->NumContacts].contactPoint =  FindIntersectPoint(&indexPtA, &nextPtA, &indexPtB,&nextPtB );
      cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&indexPtA, &prePtA,  &indexPtB,&nextPtB );
      // generate new cps for resolving penetration //
      static Point3 cp1wB;  
      cp1wB= FindPointAlongNormalByPenetration(contactPtOld, normal, penetration);
      cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld * indexPtA; //
      cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld * cp1wB; // 
      static Point3 cp2w;
      cp2w = FindSecondPair(contactPtOld,cm->ContactList[cm->NumContacts].contactPoint, cm->ContactList[cm->NumContacts].contactPoint2);
      cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld * cp2w; //
      cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld * cp2w; //
    }

    /*
    if(!IsIndexInA(OBBa, indexPtB))
    {
      printf("1 point, indexB not in A\n\n\n");
      cm->ContactList[cm->NumContacts].contactPoint = FindIntersectPoint(&contactPt, &nextPtA, &indexPtB,&nextPtB );
      if( PosOfPointsCompVec(&prePtA, &indexPtB,&normal) > 0)  // cull with reference face
      {
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&contactPt, &prePtA, &indexPtB,&nextPtB );
        // generate new cps for resolving penetration //
        Point3 cp1wB;  
        cp1wB= FindPointAlongNormalByPenetration(contactPtOld, normal, penetration);
        cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld * contactPtOld; //
        cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld * cp1wB; // 
        Point3 cp2w;
        cp2w = FindSecondPair(contactPtOld,cm->ContactList[cm->NumContacts].contactPoint, cm->ContactList[cm->NumContacts].contactPoint2);
        cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld * cp2w; //
        cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld * cp2w; //
      }
      else // cull with side face
      {
        Point4 newPt;
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&contactPt, &prePtA, &nextnextPtB,&nextPtB );
        // generate new cps for resolving penetration //
        cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld * contactPtOld; //
        cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint; //
        newPt = FindIntersectPoint(&indexPtA, &prePtA, &nextnextPtB,&nextPtB );
        cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld *newPt;
        cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld * nextPtB;
      }
    }
    else
    {
      printf("1 point, indexB in A\n\n\n");
      cm->ContactList[cm->NumContacts].contactPoint = FindIntersectPoint(&contactPt, &prePtA, &indexPtB,&nextPtB );
      float DotProduct1,DotProduct2;
      
      if( DotProduct1 = PosOfPointsCompVec(&nextPtA, &indexPtB,&normal); > 0)  // cull with reference face
      {
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&contactPt, &nextPtA, &indexPtB,&nextPtB );
        // generate new cps for resolving penetration //
        Point3 cp1wB;  
        cp1wB= FindPointAlongNormalByPenetration(contactPtOld, normal, penetration);
        cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld * contactPtOld; //
        cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld * cp1wB; // 
        Point3 cp2w;
        cp2w = FindSecondPair(contactPtOld,cm->ContactList[cm->NumContacts].contactPoint, cm->ContactList[cm->NumContacts].contactPoint2);
        cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld * cp2w; //
        cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld * cp2w; //
      }
      else // cull with side face
      {
        Point4 newPt;
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&contactPt, &nextPtA, &indexPtB,&prePtB );
        // generate new cps for resolving penetration //
        cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld * contactPtOld; //
        cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint; //
        newPt = FindIntersectPoint(&indexPtA, &nextPtA, &indexPtB,&prePtB );
        cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld *newPt;
        cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld * indexPtB;
      }
    }
    */
    //static XMFLOAT3 cont1 , cont2;
    //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].cp1mA );
    //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].cp1mB );
    //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);

    //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].cp2mA );
    //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].cp2mB );
    //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);
    /***** generate new cps for resolving penetration ****/
    /*=========================DRAW CONTACT ============================*/

    //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].contactPoint );
    //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].contactPoint2 );
    //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::RED);
    /*=========================DRAW CONTACT ============================*/
  }
  else if(numOfPoints == 2)
  {
    static Point3 contactPtOld;
	contactPtOld = cm->ContactList[cm->NumContacts].contactPoint;
    static Point3 pair1A, pair1B, pair2A, pair2B;

    PointsUnderNormal side = TwoUnderNormalCheck(indexA, indexB,OBBa,OBBb,&normal);
    cm->ContactList[cm->NumContacts].side = side;
    if(side == BothInside)
    {
      //printf("Both Inside\n\n\n");
      float DotProduct1,DotProduct2;
      pair1A = indexPtA;
      DotProduct1 = PosOfPointsCompVec(&nextPtA, &indexPtB, &normal);
      DotProduct2 = PosOfPointsCompVec(&prePtA, &indexPtB, &normal);
      if(DotProduct1 < EPSLON)
      {
        pair1B = prePtA;
        pair2A = nextPtA;
        pair2B = nextnextPtA;
      }
      else if(DotProduct2 < EPSLON)
      {
        pair1B = nextPtA;
        pair2A = prePtA;
        pair2B = nextnextPtA;
      }
      //else
        //_asm int 3;
      cm->ContactList[cm->NumContacts].contactPoint =  FindIntersectPoint(&pair1A, &pair1B, &indexPtB,&nextPtB );
      cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&pair2A, &pair2B, &indexPtB,&nextPtB );
      /*
      TwoUnderNormalFindPoints(indexA, indexB,OBBa,OBBb,&normal,&ptA1,&ptA2,&ptA3,&ptA4);
      cm->ContactList[cm->NumContacts].contactPoint = FindIntersectPoint(&ptA1, &ptA2, &indexPtB,&nextPtB );
      cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&ptA3, &ptA4, &indexPtB,&nextPtB );
      */
      /***** generate new cps for resolving penetration ****/
      /*
      cm->ContactList[cm->NumContacts].cp1mA =OBBa->mInvWorld * ptA1;
      cm->ContactList[cm->NumContacts].cp1mB =OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;
      cm->ContactList[cm->NumContacts].cp2mA =OBBa->mInvWorld * ptA3;
      cm->ContactList[cm->NumContacts].cp2mB =OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;
      */

      cm->ContactList[cm->NumContacts].cp1mA =  OBBa->mInvWorld * pair1A;// 
      cm->ContactList[cm->NumContacts].cp1mB =  OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;//
      cm->ContactList[cm->NumContacts].cp2mA =  OBBa->mInvWorld * pair2A;//
      cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;
      
      //
      //static XMFLOAT3 cont1 , cont2;
      //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].cp1mA );
      //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].cp1mB );
      //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);

      //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].cp2mA );
      //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].cp2mB );
      //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);

      //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].contactPoint);
      //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].contactPoint2 );
      //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);
      /***** generate new cps for resolving penetration ****/
    }
    else if(side == BothOutside)
    {
      //printf("Both Outside\n\n\n");
      cm->ContactList[cm->NumContacts].side = BothOutside;
      static Point4 refPt;
      static Vector normalA;
      float DotProduct1,DotProduct2;
      pair1A = indexPtB;
      DotProduct1 = PosOfPointsCompVec(&nextPtA, &indexPtB, &normal);
      DotProduct2 = PosOfPointsCompVec(&prePtA, &indexPtB, &normal);
      if(DotProduct1 < EPSLON)
      {
        refPt = nextPtA;
        normalA = refPt - indexPtA;
      }
      else if(DotProduct2 < EPSLON)
      {
        refPt = prePtA;
        normalA = indexPtA - refPt;
      }
     // else
       // _asm int 3;

      FlipNormal(&normalA);
      DotProduct1 = PosOfPointsCompVec(&nextPtB, &indexPtA, &normalA);
      DotProduct2 = PosOfPointsCompVec(&prePtB, &indexPtA, &normalA);
      if(DotProduct1 < EPSLON)
      {
        pair1B = prePtB;
        pair2A = nextPtB;
        pair2B = nextnextPtB;
      }
      else if(DotProduct2 < EPSLON)
      {
        pair1B = nextPtB;
        pair2A = prePtB;
        pair2B = nextnextPtB;
      }
	 // else
		  //_asm int 3;
      cm->ContactList[cm->NumContacts].contactPoint =  FindIntersectPoint(&pair1A, &pair1B,&indexPtA,&refPt);
      cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&pair2A, &pair2B,&indexPtA,&refPt);
      /*
      TwoUnderNormalFindPointsFlip(indexA, indexB,OBBa,OBBb,&normal,&ptA1,&ptA2,&ptA3,&ptA4,&normalA,&normalB);
      cm->ContactList[cm->NumContacts].contactPoint = FindIntersectPoint(&ptA1, &ptA2,&normalA,&normalB);
      cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&ptA3, &ptA4,&normalA,&normalB);
      */
      /***** generate new cps for resolving penetration ****/
      cm->ContactList[cm->NumContacts].cp1mB =  OBBb->mInvWorld * pair1A;//
      cm->ContactList[cm->NumContacts].cp1mA =  OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;//
      cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * pair2A;//
      cm->ContactList[cm->NumContacts].cp2mA =  OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;

      //
      //static XMFLOAT3 cont1 , cont2;
      //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].cp1mA );
      //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].cp1mB );
      //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);

      //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].cp2mA );
      //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].cp2mB );
     // Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);
      //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].contactPoint);
      //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].contactPoint2 );
      //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);
      /***** generate new cps for resolving penetration ****/
    }
    else if(side == OneInOneOut)
    {
      bool IsIndexBInA, IsNextBInA; 
      float IsNextAUnder, IsPreAUnder;
      IsIndexBInA = IsIndexInA(OBBa, indexPtB);
      IsNextBInA  = IsIndexInA(OBBa, nextPtB);
      IsNextAUnder = PosOfPointsCompVec(&nextPtA, &indexPtB, &normal);
      IsPreAUnder =  PosOfPointsCompVec(&prePtA, &indexPtB, &normal);
	  if(IsIndexBInA && IsNextBInA && IsPreAUnder < EPSLON)
	  {
		 cm->ContactList[cm->NumContacts].contactPoint  = FindIntersectPoint(&indexPtA, &prePtA,&indexPtB,&prePtB);
		 cm->ContactList[cm->NumContacts].contactPoint2 = indexPtA;;
		 cm->ContactList[cm->NumContacts].cp1mA =  OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;
		 cm->ContactList[cm->NumContacts].cp1mB =  OBBb->mInvWorld * nextPtB;
		 cm->ContactList[cm->NumContacts].cp2mA =  OBBa->mInvWorld * indexPtA;
		 cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * indexPtB;
	  }
      else if(IsIndexBInA || IsNextAUnder < EPSLON)
      {
	    //printf("One In One Out Index B In A\n\n\n");
        cm->ContactList[cm->NumContacts].contactPoint  = FindIntersectPoint(&indexPtA, &nextPtA,&indexPtB,&prePtB);
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&indexPtA, &prePtA ,&indexPtB,&nextPtB);
        cm->ContactList[cm->NumContacts].cp1mA =  OBBa->mInvWorld * indexPtA;
        cm->ContactList[cm->NumContacts].cp1mB =  OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;
        cm->ContactList[cm->NumContacts].cp2mA =  OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;
        cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * indexPtB;
      }
      else if(IsNextBInA || IsPreAUnder < EPSLON)
      {
		  //printf("One In One Out Next B In A\n\n\n");
        cm->ContactList[cm->NumContacts].contactPoint  = FindIntersectPoint(&indexPtA, &nextPtA,&indexPtB,&nextPtB);
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&indexPtA, &prePtA ,&nextPtB ,&nextnextPtB);
        cm->ContactList[cm->NumContacts].cp1mA =  OBBa->mInvWorld * indexPtA;
        cm->ContactList[cm->NumContacts].cp1mB =  OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;
        cm->ContactList[cm->NumContacts].cp2mA =  OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;
        cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * nextPtB;

      }
	  cm->ContactList[cm->NumContacts].numberOfcontacts = 2;
	  /*
      else if(IsNextAUnder <= 0.02f)
      {
        printf("One In One Out, Next A under\n\n\n");
        cm->ContactList[cm->NumContacts].contactPoint  = FindIntersectPoint(&indexPtA, &nextPtA,&indexPtB,&prePtB);
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&indexPtA, &prePtA ,&indexPtB,&nextPtB);
        cm->ContactList[cm->NumContacts].cp1mA =  OBBa->mInvWorld * indexPtA;
        cm->ContactList[cm->NumContacts].cp1mB =  OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;
        cm->ContactList[cm->NumContacts].cp2mA =  OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;
        Point4 newPt;
        newPt = FindIntersectPoint(&nextPtA, &nextnextPtA,&indexPtB,&nextPtB);
        cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * newPt;
      }
      else if(IsPreAUnder <= 0.02f)
      {
		  printf("One In One Out,  pre A under\n\n\n");
        cm->ContactList[cm->NumContacts].contactPoint  = FindIntersectPoint(&indexPtA, &nextPtA,&indexPtB,&nextPtB);
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&indexPtA, &prePtA ,&nextPtB ,&nextnextPtB);
        cm->ContactList[cm->NumContacts].cp1mA =  OBBa->mInvWorld * indexPtA;
        cm->ContactList[cm->NumContacts].cp1mB =  OBBb->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint;
        cm->ContactList[cm->NumContacts].cp2mA =  OBBa->mInvWorld * cm->ContactList[cm->NumContacts].contactPoint2;
        cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * nextPtB;
        Point4 newPt;
        newPt = FindIntersectPoint(&prePtA, &nextnextPtA,&indexPtB,&nextPtB);
        cm->ContactList[cm->NumContacts].cp2mB =  OBBb->mInvWorld * newPt;
      }
	  */
     // else
       // _asm int 3;
      /*
      bool IsInA = IsIndexInA(OBBa, indexPtB);
      if(!IsInA) // not in A
      {
        printf("2 point, One Inside one out\n");
        printf("Index in A\n\n\n");
        cm->ContactList[cm->NumContacts].type = 1;
        Point3 nextnextBPt;
        int twocase;
        bool isInABigInB;
        Point3 potentilaPt1, potentilaPt2;
        potentilaPt1 = FindIntersectPointReturnGarbage(&indexPtA, &nextPtA,&indexPtB,&prePtB);
        potentilaPt2 = FindIntersectPointReturnGarbage(&indexPtA, &nextPtA,&indexPtB,&nextPtB);

        if(IsIndexInA(OBBa, potentilaPt1) &&IsIndexInA(OBBb, potentilaPt1))
        {
          twocase = 1;
          cm->ContactList[cm->NumContacts].contactPoint = potentilaPt1;
        }
        else
        {
          twocase = 2;
          cm->ContactList[cm->NumContacts].contactPoint = potentilaPt2;
        }
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&indexPtA, &prePtA,&nextPtB,&nextnextPtB);

        // generate new cps for resolving penetration //
        cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld *indexPtA;//
        if(twocase == 1)
        {
          Point4 newPt;
          cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint2;
          cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint;
          newPt = FindIntersectPoint(&nextPtA, &nextnextPtA,&prePtB,&indexPtB);
          cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld *newPt;
        }
        else
        {
          cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint;
          cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint2;
          cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld *nextPtB;
        }

        XMFLOAT3 cont1 , cont2;
        PtStoreXMFloat3(&cont1,OBBa->mWorld * cm->ContactList[cm->NumContacts].cp1mA );
        PtStoreXMFloat3(&cont2,OBBb->mWorld * cm->ContactList[cm->NumContacts].cp1mB );
        //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::RED);

        PtStoreXMFloat3(&cont1,OBBa->mWorld * cm->ContactList[cm->NumContacts].cp2mA );
        PtStoreXMFloat3(&cont2,OBBb->mWorld * cm->ContactList[cm->NumContacts].cp2mB );
        //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);

        PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].contactPoint);
        PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].contactPoint2 );
        //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);
        // generate new cps for resolving penetration //

      }
      else // in a
      {
        printf("2 point, One Inside one out\n");
        printf("Index not in A\n\n\n");
        Point3 nextnextBPt;
        int twocase;
        cm->ContactList[cm->NumContacts].type = 2;
        Point3 potentilaPt1, potentilaPt2;
        potentilaPt1 = FindIntersectPointReturnGarbage(&indexPtA, &prePtA,&nextnextPtB,&nextPtB);
        potentilaPt2 = FindIntersectPointReturnGarbage(&indexPtA, &prePtA,&indexPtB,&nextPtB);
        bool check1, check2;
        check1 = IsIndexInA(OBBa, potentilaPt1) ;
        check2 = IsIndexInA(OBBb, potentilaPt1);
        if(check1&&check2)
        {
          //IndA && C2
          twocase = 1;
          cm->ContactList[cm->NumContacts].contactPoint = potentilaPt1;
        }
        else
        {
          //IndA && C1
          twocase = 2;
          cm->ContactList[cm->NumContacts].contactPoint = potentilaPt2;
        }
        cm->ContactList[cm->NumContacts].contactPoint2 = FindIntersectPoint(&indexPtA, &nextPtA,&indexPtB,&prePtB);
        // generate new cps for resolving penetration //

        cm->ContactList[cm->NumContacts].cp1mA = OBBa->mInvWorld *indexPtA;//
        if(twocase == 1)
        {
          Point4 newPt;
          cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint2;
          cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint;
          newPt = FindIntersectPoint(&prePtA, &nextnextPtA,&nextPtB,&nextnextPtB);
          cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld *newPt;
        }
        else
        {
          cm->ContactList[cm->NumContacts].cp1mB = OBBb->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint;
          cm->ContactList[cm->NumContacts].cp2mA = OBBa->mInvWorld *cm->ContactList[cm->NumContacts].contactPoint2;
          cm->ContactList[cm->NumContacts].cp2mB = OBBb->mInvWorld *indexPtB;
        }

        XMFLOAT3 cont1 , cont2;
        PtStoreXMFloat3(&cont1,OBBa->mWorld * cm->ContactList[cm->NumContacts].cp1mA );
        PtStoreXMFloat3(&cont2,OBBb->mWorld * cm->ContactList[cm->NumContacts].cp1mB );
        //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::RED);

        PtStoreXMFloat3(&cont1,OBBa->mWorld * cm->ContactList[cm->NumContacts].cp2mA );
        PtStoreXMFloat3(&cont2,OBBb->mWorld * cm->ContactList[cm->NumContacts].cp2mB );
       //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);

        PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].contactPoint);
        PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].contactPoint2 );
        //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);
        /*
        /***** generate new cps for resolving penetration ****/
      
      
    }
    /*
    if(cm->ContactList[cm->NumContacts].contactPoint.x != cm->ContactList[cm->NumContacts].contactPoint.x
    ||cm->ContactList[cm->NumContacts].contactPoint.y != cm->ContactList[cm->NumContacts].contactPoint.y
    ||cm->ContactList[cm->NumContacts].contactPoint2.x != cm->ContactList[cm->NumContacts].contactPoint2.x
    ||cm->ContactList[cm->NumContacts].contactPoint2.y != cm->ContactList[cm->NumContacts].contactPoint2.y)
      _asm int 3;
      */

  }
  /*=========================DRAW CONTACT ============================*/

  //static XMFLOAT3 cont1 , cont2;
  //PtStoreXMFloat3(&cont1,OBBa->mWorld *cm->ContactList[cm->NumContacts].cp1mA );
  //PtStoreXMFloat3(&cont2,OBBb->mWorld *cm->ContactList[cm->NumContacts].cp1mB );
  //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::RED);

  //PtStoreXMFloat3(&cont1,OBBa->mWorld *cm->ContactList[cm->NumContacts].cp2mA );
  //PtStoreXMFloat3(&cont2,OBBb->mWorld *cm->ContactList[cm->NumContacts].cp2mB );
  //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::BLUE);

  //PtStoreXMFloat3(&cont1,cm->ContactList[cm->NumContacts].contactPoint);
  //PtStoreXMFloat3(&cont2,cm->ContactList[cm->NumContacts].contactPoint2 );
  //Renderer::getInstance()->DrawLine(cont1, cont2,COLOR::RED);
  /*=========================DRAW CONTACT ============================*/

}
ContactInfo SAT::FindFinalContactInfo(ContactInfo* info)
{
  float smallPene = FLT_MAX;
  int index = 0;
  for (int i = 0; i < 8;++i)
  {
    if (info[i].IsInBox)
    {
      if(info[i].penetration < smallPene)
      {
        smallPene = info[i].penetration ;
        index = i;
      }
    }
  }
  return info[index];
}

bool SAT::CollisionDetection(CollisionShape* a, Point3 aPosition, CollisionShape* b, Point3 bPosition, ContactsManager *c)
{
  CPhysics* cp1 = a->cphysics;
  CPhysics* cp2 = b->cphysics;
  static ContactInfo info[8];
  int counter = 0;
  static Direction dir;
  static Direction result = None;
  static Axis axis;
  static Vector normal ;
  float penetration = FLT_MAX ;
  int indexA, indexB;
  bool InBox[4];
  OBB* BoxA;
  OBB* BoxB; 
  BoxA = (OBB*) a;
  BoxB = (OBB*) b;
  static Point3 AinBPoints[4], BinAPoints[4],contactPoint;
  static Vector egdeNormal[4], normalTest[4];
  Matrix InvWorld = b->mInvWorld;
  normalTest[0].Set(-1.0f,0,0);
  normalTest[1].Set(0,-1.0f,0);
  normalTest[2].Set(1.0f,0,0);
  normalTest[3].Set(0,1.0f,0);
  for (int i = 0; i < 4; ++i)
  {
    InBox[i] = false;
  }
  for (int i = 0; i < 4; ++i)
  {
    AinBPoints[i] = InvWorld * BoxA->mPoints[i];
  }

  dir = AtoB;
  for (int i = 0; i < 4; ++i)
  {
    
    if(i == 0 || i == 2)
      axis = X;
    else
      axis = Y;
    if(ProjectAToBLeastPenetration(&(AinBPoints[0]),&(AinBPoints[1]),&(AinBPoints[2]),&(AinBPoints[3]),&(normalTest[i]),BoxB,&axis,&normal,
                                  &contactPoint, &penetration, &indexA,&indexB, i ,&dir, &result, info,counter) == false)
      return false;
    ++counter;
  }
  InvWorld = a->mInvWorld;
  for (int i = 0; i < 4; ++i)
  {
    BinAPoints[i] = InvWorld * BoxB->mPoints[i];
  }

  dir = BtoA;
  for (int i = 0; i < 4; ++i)
  {
    
    if(i == 0 || i == 2)
      axis = X;
    else
      axis = Y;
    if(ProjectAToBLeastPenetration(&(BinAPoints[0]),&(BinAPoints[1]),&(BinAPoints[2]),&(BinAPoints[3]),&(normalTest[i]),
             BoxA, &axis, &normal , &contactPoint, &penetration, &indexA,&indexB, i,&dir,&result,info,counter) == false)
      return false;
    ++counter;
  }
  
  


  ContactInfo finalInfo = FindFinalContactInfo(info);
  if(!finalInfo.IsInBox)
    return false;

  if(finalInfo.direction == AtoB)
  {
	  //static CollisionData cd;
	  //cd.indexA		     = finalInfo.indexA;
	  //cd.indexB			 = finalInfo.indexB;
	  //cd.normal          = finalInfo.normal;
	  //
	  //cd.collidedObj	 = b->cphysics->gameObject;
	  //cd.collidedObjCPhy = b->cphysics;
	  //cd.selfCPhy        = a->cphysics;
	  //cd.isA             = true;
	  ////a->cphysics->gameObject->OnCollisionEnter(cd);
	  //
	  //cd.collidedObj     = a->cphysics->gameObject;
	  //cd.collidedObjCPhy = a->cphysics;
	  //cd.selfCPhy        = b->cphysics;
	  //cd.isA		     = false;
	  //b->cphysics->gameObject->OnCollisionEnter(cd);
  }
  else
  {
	  //static CollisionData cd;
	  //cd.indexA			 = finalInfo.indexA;
	  //cd.indexB			 = finalInfo.indexB;
	  //cd.normal			 = finalInfo.normal;
	  //
	  //cd.collidedObj	 = b->cphysics->gameObject;
	  //cd.collidedObjCPhy = b->cphysics;
	  //cd.selfCPhy        = a->cphysics;
	  //cd.isA = false;
	  ////a->cphysics->gameObject->OnCollisionEnter(cd);
	  //
	  //cd.collidedObj	 = a->cphysics->gameObject;
	  //cd.collidedObjCPhy = a->cphysics;
	  //cd.selfCPhy        = b->cphysics;
	  //cd.isA			 = true;
	  //b->cphysics->gameObject->OnCollisionEnter(cd);
  }

  if(finalInfo.penetration > 1.0f)
	  return false;
  if(a->cphysics->mIsTriggered ||b->cphysics->mIsTriggered)
	  return true;
  int next;
  if(finalInfo.indexB == 3)
    next = 0;
  else 
    next = finalInfo.indexB + 1;
  c->ContactList[c->NumContacts].penetration = finalInfo.penetration;
  c->ContactList[c->NumContacts].normal = finalInfo.normal;
  c->ContactList[c->NumContacts].changePosA.Set(0,0,0);
  c->ContactList[c->NumContacts].changePosB.Set(0,0,0);
  c->ContactList[c->NumContacts].indexA = finalInfo.indexA;
  c->ContactList[c->NumContacts].indexB = finalInfo.indexB;
  c->ContactList[c->NumContacts].type = 1;
  //**********************  DRAW FIRST CONTACT POINT  ************************//
  //static XMFLOAT3 contact, normall;
  //static Point3 point = contactPoint;
  //PtStoreXMFloat3(&contact,point);
  //VecStoreXMFloat3(&normall,BoxB->GetAxisU() -BoxB->GetAxisV() );
  //Renderer::getInstance()->DrawLinePointVector(contact, normall, COLOR::BLUE);
  //**********************  DRAW FIRST CONTACT POINT  ************************//
  if(finalInfo.direction == AtoB)
  {
    c->ContactList[c->NumContacts].contactPoint = BoxA->mPoints[finalInfo.indexA];
    c->ContactList[c->NumContacts].contacts[0] = a->cphysics;
    c->ContactList[c->NumContacts].contacts[1] = b->cphysics;
	c->ContactList[c->NumContacts].PosA = aPosition;
	c->ContactList[c->NumContacts].PosB = bPosition;
    UpdateContacPoints(c,BoxA , BoxB );
    //**********************  DRAW FIRST CONTACT POINT  ************************//

    //static XMFLOAT3 contact, normall;
    //static Point3 point;
    //point.x = (BoxB->mPoints[finalInfo.indexB].x + BoxB->mPoints[next].x) * 0.5f;
    //point.y = (BoxB->mPoints[finalInfo.indexB].y + BoxB->mPoints[next].y) * 0.5f;
    //PtStoreXMFloat3(&contact,point);
    //VecStoreXMFloat3(&normall,finalInfo.normal);
    //Renderer::getInstance()->DrawLinePointVector(contact, normall, COLOR::YELLOW);
  }
  else
  {
    c->ContactList[c->NumContacts].contactPoint = BoxB->mPoints[finalInfo.indexA];
    c->ContactList[c->NumContacts].contacts[0] = b->cphysics;
    c->ContactList[c->NumContacts].contacts[1] = a->cphysics;
	c->ContactList[c->NumContacts].PosA = bPosition;
	c->ContactList[c->NumContacts].PosB = aPosition;
    UpdateContacPoints(c,BoxB ,   BoxA);
    //**********************  DRAW FIRST CONTACT POINT  ************************//
    //static XMFLOAT3 contact, normall;
    //static Point3 point;
    //point.x = (BoxA->mPoints[finalInfo.indexB].x + BoxA->mPoints[next].x) * 0.5f;
    //point.y = (BoxA->mPoints[finalInfo.indexB].y + BoxA->mPoints[next].y) * 0.5f;
    //PtStoreXMFloat3(&contact,point);
    //VecStoreXMFloat3(&normall,finalInfo.normal);
    //Renderer::getInstance()->DrawLinePointVector(contact, normall, COLOR::YELLOW);
  }
  c->ContactList[c->NumContacts].L1 = 0;
  c->ContactList[c->NumContacts].L2 = 0;
  c->ContactList[c->NumContacts].collsionPair = BoxToBox;
  ++(c->NumContacts);

  // update contact list here
  //printf("Collide\n\n\n\n");
  //printf("Hahahaha");
  return true;
}


/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   SAT algorithm