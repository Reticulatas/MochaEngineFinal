/*-----------------------------------------------------------------
  ----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kai Chuan Hsiao
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Resolution.h"
#include "CPhysics.h"
#define EPSLON 0.002
#define ANGEPSLON 0.001
#define LINEPSLON 0.9
ContactsManager::ContactsManager() : NumContacts(0)
{
}
Contacts* Resolution::FindMostPenetration(ContactsManager* CM)
{
  float mostPenetra = 0.0f;
  Contacts* resolveContact = NULL;
  for (unsigned i = 0; i < CM->NumContacts; ++i)
  {
    if(CM->ContactList[i].penetration > mostPenetra)
    {
      mostPenetra = CM->ContactList[i].penetration;
      resolveContact = &(CM->ContactList[i]);
    }
  }
  return resolveContact;
}
float PosOfPointsCompVecRes(Point3* mpoint,Point3* mpointonline, Vector* mvector)
{
  Point3 pointOnLine = *mpointonline;
  Point3 point = *mpoint;
  Vector vector = *mvector;

  return vector.Dot(point -pointOnLine);

}
void Resolution::UpdateWorldMatrix(Contacts* C)
{
  
  CPhysics* GoA = C->contacts[0];
  CPhysics* GoB = C->contacts[1];
  float angleA = GoA->GetRotation();
  float angleB = GoB->GetRotation();
  static Matrix4 tranA, tranB, rotA, rotB;
  tranA.m03 = GoA->GetPosition().x; tranA.m13 = GoA->GetPosition().y; tranA.m23 = GoA->GetPosition().z;
  tranB.m03 = GoB->GetPosition().x; tranB.m13 = GoB->GetPosition().y; tranB.m23 = GoB->GetPosition().z;
  rotA.m00 = cosf(angleA); rotA.m01 = -sinf(angleA); rotA.m10 = sinf(angleA); rotA.m11 = cosf(angleA);
  rotB.m00 = cosf(angleB); rotB.m01 = -sinf(angleB); rotB.m10 = sinf(angleB); rotB.m11 = cosf(angleB);
  C->contacts[0]->GetCollisionShape()->mWorld = tranA * rotA;
  C->contacts[1]->GetCollisionShape()->mWorld = tranB * rotB;
}
float Resolution::PositionConstrain(Contacts* C, int numPene)
{
  static Point3 cp1wA, cp1wB, cp2wA, cp2wB;
  static Matrix4 aWorld;
  static Matrix4 bWorld;
  aWorld = C->contacts[0]->GetCollisionShape()->mWorld;
  bWorld = C->contacts[1]->GetCollisionShape()->mWorld;
  cp1wA = aWorld * C->cp1mA;
  cp2wA = aWorld * C->cp2mA;
  cp1wB = bWorld * C->cp1mB;
  cp2wB = bWorld * C->cp2mB;

  static Vector L1,L2, normal, rA, rB;
  float A1,A2, MInv1,MInv2, IneInv1, IneInv2, rACrossN, rBCrossN;

  float L, JMJt, JV, bias, penetration;
  
  normal = C->normal;
  L1 = C->contacts[0]->GetVelocity();
  L2 = C->contacts[1]->GetVelocity();
  A1 = C->contacts[0]->GetAngVelocity();
  A2 = C->contacts[1]->GetAngVelocity();
  MInv1 = C->contacts[0]->GetInverseMass();
  MInv2 = C->contacts[1]->GetInverseMass();
  IneInv1 = C->contacts[0]->GetInvInertia();
  IneInv2 = C->contacts[1]->GetInvInertia();
  if(numPene == 1)
  {
      rA = cp1wA - C->contacts[0]->GetPosition();

      rB = cp1wB - C->contacts[1]->GetPosition();

      penetration = (cp1wB - cp1wA).Dot(normal);
  }
  else
  {
      rA = cp2wA - C->contacts[0]->GetPosition();
      rB = cp2wB - C->contacts[1]->GetPosition();
      penetration = (cp2wB - cp2wA).Dot(normal);
  }
  bias = penetration - 0.005F;
  rACrossN = rA.x * normal.y - rA.y * normal.x;
  rBCrossN = rB.x * normal.y - rB.y * normal.x;
  JMJt = normal.Dot(normal) * MInv1 + rACrossN * rACrossN *  IneInv1
       + normal.Dot(normal) * MInv2 + rBCrossN * rBCrossN * IneInv2;
  JV = 0;
  //JV = normal.Dot(L1) + rACrossN * A1 - normal.Dot(L2) - rBCrossN * A2;
  L = -1 / JMJt *(JV + bias);

  return L;
  

}
void Resolution::ResolvePenetrationImpulse(Contacts* C, int numPene,ContactsManager* CM)
{
  CPhysics* GoA = C->contacts[0];
  CPhysics* GoB = C->contacts[1];
  float InveMassA = GoA->GetInverseMass();
  float InveMassB = GoB->GetInverseMass();
  if(InveMassA + InveMassB == 0)
    return;
  float movePerInveMass = C->penetration / (InveMassA + InveMassB);
  float moveA =  movePerInveMass * InveMassA * 0.3f;
  float moveB = movePerInveMass * InveMassB * 0.3f;
  C->penetration *= 0.7f;
  C->changePosA = C->normal * moveA;
  C->changePosB = C->normal * moveB ;



  C->contacts[0]->SetPosition(GoA->GetPosition() + C->changePosA );
  C->contacts[1]->SetPosition(GoB->GetPosition() - C->changePosB );
  //if(C->changePosA.Length() > 0.3f || C->changePosB.Length() > 0.3f)
    //_asm int 3;
  UpdatePenetration(C, CM);
}
void Resolution::ResolvePenetrationConstraint(Contacts* C, int numPene)
{
  static Point3 cpwA, cpwB;
  static Matrix4 aWorld;
  static Matrix4 bWorld;
  float penetration;
  static Vector normal;
  normal = C->normal;
  aWorld = C->contacts[0]->GetCollisionShape()->mWorld;
  bWorld = C->contacts[1]->GetCollisionShape()->mWorld;
  if(numPene == 1)
  {
      cpwA = aWorld * C->cp1mA;
      cpwB = bWorld * C->cp1mB;

  }
  else
  {
    cpwA = aWorld * C->cp2mA;
    cpwB = bWorld * C->cp2mB;

  }
  penetration = (cpwB - cpwA).Dot(normal) ;
  if(penetration < 0)
    return;



  CPhysics* GoA = C->contacts[0];
  CPhysics* GoB = C->contacts[1];
  static Vector rA,rB;
  float Lam, rACrossN,rBCrossN;
  if(numPene == 1)
  {
    Lam = PositionConstrain(C,1);
  }
  else
  {
    Lam = PositionConstrain(C,2);
  }
  
  rA = cpwA - C->PosA;
  rB = cpwB - C->PosB;
  rACrossN = rA.x * normal.y - rA.y * normal.x;
  rBCrossN = rB.x * normal.y - rB.y * normal.x;
  float InvM1, InvM2, InvI1, InvI2;
  static Vector V1Lam, V2Lam ; 
  static Vector L1,L2;
  float A1Lam, A2Lam;

  InvM1 = C->contacts[0]->GetInverseMass();InvM2 = C->contacts[1]->GetInverseMass();
  InvI1 = C->contacts[0]->GetInvInertia(); InvI2 = C->contacts[1]->GetInvInertia();


  V1Lam = normal * InvM1 * Lam;
  V2Lam = -normal * InvM2 * Lam;
  A1Lam = rACrossN * InvI1 * Lam;
  A2Lam = -rBCrossN * InvI2 * Lam;

  C->changePosA = V1Lam;
  C->changePosB = V2Lam;

  if(C->contacts[0]->mIsJoint)
	  C->changePosA.ZeroOut();
  if(C->contacts[1]->mIsJoint)
	  C->changePosB.ZeroOut();
  //if(V1Lam.Dot(normal) > 2.0f || V1Lam.Dot(normal) < -2.0f )
   //return;

  //if(C->changePosA.Length() > 3.0f || C->changePosB.Length() > 3.0f)
   // _asm int 3;

  C->changePosA.z = 0;
  C->changePosB.z = 0;

  C->contacts[0]->SetPosition(GoA->GetPosition() - C->changePosA );
  C->contacts[0]->SetRotation(GoA->GetRotation() - A1Lam);


  C->contacts[1]->SetPosition(GoB->GetPosition() - C->changePosB );
  C->contacts[1]->SetRotation(GoB->GetRotation() - A2Lam);


  UpdateWorldMatrix(C);
  
 }
Vector Resolution::GetRelativeVelocity(Contacts* C, int numContact)
{
   Point3 contactPt;
   if(numContact == 1)
    contactPt = C->contactPoint;
   else
     contactPt = C->contactPoint2;
   Point3 contactPt2 = C->contactPoint2;
   Vector Vp1, Vp2,Vrel,V1,V2, r1, r2, rCorssw1, rCrossw2;
   float w1 = C->contacts[0]->GetAngVelocity();
   float w2 = C->contacts[1]->GetAngVelocity();

   r1 = contactPt - C->contacts[0]->GetPosition();
   r2 = contactPt - C->contacts[1]->GetPosition();
   rCorssw1 = r1 * w1; rCrossw2 = r2 * w2; 
   V1 = C->contacts[0]->GetVelocity();
   V2 = C->contacts[1]->GetVelocity();
   rCorssw1.z = rCorssw1.x; rCorssw1.x = rCorssw1.y * -1; rCorssw1.y = rCorssw1.z;
   rCrossw2.z = rCrossw2.x; rCrossw2.x = rCrossw2.y * -1; rCrossw2.y = rCrossw2.z;
   rCorssw1.z = 0; rCrossw2.z = 0;
   Vp1 = V1 + rCorssw1;
   Vp2 = V2 + rCrossw2;
   Vrel =  Vp1 - Vp2;
   return Vrel;
}
float Resolution::GenerateImpulse(Contacts* C, Vector*normal, Vector*r1, Vector*r2, int numContact)
{
  float J;
  Vector relativeVel = GetRelativeVelocity(C,numContact);
  
  /**** debug draw ******/
  /*
  Point3 contactPt;
  if(numContact == 1)
    contactPt = C->contactPoint;
  else
    contactPt = C->contactPoint2;
  Vector tiveVel = GetRelativeVelocity(C,numContact);
  XMFLOAT3 contact, normall;
  PtStoreXMFloat3(&contact,contactPt);
  VecStoreXMFloat3(&normall,tiveVel);
  Renderer::getInstance()->DrawLinePointVector(contact, normall, COLOR::RED);
  */
  /**** debug draw ******/
  
  float contactVel = relativeVel.Dot(*normal);
  if(contactVel >= 0.0)
    return 0;

  float InveMassSum = C->contacts[0]->GetInverseMass() + C->contacts[1]->GetInverseMass();
  float restitution = (C->contacts[0]->GetRestitution() + C->contacts[0]->GetRestitution()) * 0.5f;


  float r1crossN = r1->x * normal->y - r1->y * normal->x;
  float r2crossN = r2->x * normal->y - r2->y * normal->x;

  InveMassSum += r1crossN * r1crossN * C->contacts[0]->GetInvInertia();
  InveMassSum += r2crossN * r2crossN * C->contacts[1]->GetInvInertia();

  J = -(1 + restitution) * contactVel;

  J /= InveMassSum;

  return J;
}
void Resolution::ResolveFriction(Contacts* C, int numContact)
{
  static Vector r1 ; 
  static Vector r2 ;
  static Vector normalFric;
  static Vector normal;
  normal = C->normal;
  if(numContact == 1)
  {
    r1 = C->contactPoint - C->PosA; 
    r2 = C->contactPoint - C->PosB;
    normalFric.x = -normal.y;
    normalFric.y = normal.x;
    normalFric.z = 0.0f;
    normalFric.w = 0.0f;
  }
  else
  {
    r1 = C->contactPoint2 - C->PosA;
    r2 = C->contactPoint2 - C->PosB;
    normalFric.x = normal.y;
    normalFric.y = -normal.x;
    normalFric.z = 0.0f;
    normalFric.w = 0.0f;
  }
  
  float Jf1;

  Jf1 = GenerateImpulse(C, &normalFric, &r1, &r2,numContact);

  static Vector impulseF1;
  impulseF1 = normalFric * Jf1 * C->contacts[0]->GetFriction();
  static Vector impulseF2;
  impulseF2 = normalFric * Jf1 * C->contacts[1]->GetFriction();
  C->contacts[0]->ApplyImpulse(impulseF1, r1);
  C->contacts[1]->ApplyImpulse(-impulseF2, r2);
}
float Resolution::VelocityConstraint(Contacts* C, int numPene)
{
  static Point3 cp1wA, cp1wB, cp2wA, cp2wB;
  static Matrix4 aWorld;
  static Matrix4 bWorld;
  aWorld = C->contacts[0]->GetCollisionShape()->mWorld;
  bWorld = C->contacts[1]->GetCollisionShape()->mWorld;
  cp1wA = aWorld * C->cp1mA;
  cp2wA = aWorld * C->cp2mA;
  cp1wB = bWorld * C->cp1mB;
  cp2wB = bWorld * C->cp2mB;

  static Vector L1,L2, normal, rA, rB;
  float A1,A2, MInv1,MInv2, IneInv1, IneInv2, rACrossN, rBCrossN;

  float L, JMJt, JV, bias;

  normal = C->normal;
  L1 = C->contacts[0]->GetVelocity();
  L2 = C->contacts[1]->GetVelocity();
  A1 = C->contacts[0]->GetAngVelocity();
  A2 = C->contacts[1]->GetAngVelocity();
  MInv1 = C->contacts[0]->GetInverseMass();
  MInv2 = C->contacts[1]->GetInverseMass();
  IneInv1 = C->contacts[0]->GetInvInertia();
  IneInv2 = C->contacts[1]->GetInvInertia();
  if(numPene == 1)
  {
    rA = cp1wA - C->contacts[0]->GetPosition();
    rB = cp1wB - C->contacts[1]->GetPosition();
  }
  else
  {
    rA = cp2wA - C->contacts[0]->GetPosition();
    rB = cp2wB - C->contacts[1]->GetPosition();
  }
  // baungaurd
  float penetration = C->penetration;
  bias = (1 / 0.01666666f) * 0.04f * penetration;
  //slope
  bias = bias * (-penetration + 0.01f < 0 ? -penetration + 0.01f:0);
  rACrossN = rA.x * normal.y - rA.y * normal.x;
  rBCrossN = rB.x * normal.y - rB.y * normal.x;
  JMJt = normal.Dot(normal) * MInv1 + rACrossN * rACrossN *  IneInv1
    + normal.Dot(normal) * MInv2 + rBCrossN * rBCrossN * IneInv2;
  
  JV = normal.Dot(L1) + rACrossN * A1 - normal.Dot(L2) - rBCrossN * A2;
  L = -1 / JMJt *(JV + bias);
  //printf("L = %f\n\n\n", L);
  return L;
}
float Resolution::ClampLamda(float lamda)
{
  if(lamda < 0)
    return 0;
  else
    return lamda;
}
void Resolution::ResolveVelocityImpulse(Contacts* C, int numPene)
{
    Vector r1; 
    Vector r2; 
    if(numPene == 1)
    {
      r1 = C->contactPoint - C->contacts[0]->GetPosition(); 
      r2 = C->contactPoint - C->contacts[1]->GetPosition();
    }
    else
    {
       r1 = C->contactPoint2 - C->contacts[0]->GetPosition(); 
       r2 = C->contactPoint2 - C->contacts[1]->GetPosition();
    }
    Vector normal = C->normal;

    float J1;
    J1 = GenerateImpulse(C, &normal, &r1, &r2,numPene);

    Vector impulse1 = normal * J1;

    C->contacts[0]->ApplyImpulse( impulse1, r1);
    C->contacts[1]->ApplyImpulse(-impulse1, r2);
}
void Resolution::ResolveVelocity(Contacts* C, int numPene)
{
  
  static Point3 cpwA, cpwB;
  static Matrix4 aWorld;
  static Matrix4 bWorld;
  static Vector normal;
  normal = C->normal;
  aWorld = C->contacts[0]->GetCollisionShape()->mWorld;
  bWorld = C->contacts[1]->GetCollisionShape()->mWorld;
  float Lamold;
  CPhysics* GoA = C->contacts[0];
  CPhysics* GoB = C->contacts[1];
  static Vector rA,rB;
  float Lam, rACrossN,rBCrossN;
  if(numPene == 1)
  {
    cpwA = aWorld * C->cp1mA;
    cpwB = bWorld * C->cp1mB;
    Lamold = C->L1;
    Lam = VelocityConstraint(C,1);
    C->L1 = ClampLamda(Lamold + Lam);
    Lam = C->L1 - Lamold;
  }
  else
  {
    cpwA = aWorld * C->cp2mA;
    cpwB = bWorld * C->cp2mB;
    Lamold = C->L2;
    Lam = VelocityConstraint(C,2);
    C->L2 = ClampLamda(Lamold + Lam);
    Lam = C->L2 - Lamold;
  }
  
  
  rA = cpwA - C->PosA;
  rB = cpwB - C->PosB;
  rACrossN = rA.x * normal.y - rA.y * normal.x;
  rBCrossN = rB.x * normal.y - rB.y * normal.x;
  float InvM1, InvM2, InvI1, InvI2;
  static Vector V1Lam, V2Lam ; 
  static Vector L1,L2;
  float A1,A2,A1Lam, A2Lam;

  InvM1 = C->contacts[0]->GetInverseMass();InvM2 = C->contacts[1]->GetInverseMass();
  InvI1 = C->contacts[0]->GetInvInertia(); InvI2 = C->contacts[1]->GetInvInertia();
  L1 = C->contacts[0]->GetVelocity();
  L2 = C->contacts[1]->GetVelocity();
  A1 = C->contacts[0]->GetAngVelocity();
  A2 = C->contacts[1]->GetAngVelocity();

  V1Lam = normal * InvM1 * Lam + L1;
  V2Lam = -normal * InvM2 * Lam + L2;
  A1Lam = rACrossN * InvI1 * Lam + A1;
  A2Lam = -rBCrossN * InvI2 * Lam + A2;

  if(!GoA->mIsJoint)
	  GoA->SetVelocity(V1Lam);
  GoA->SetAnguVelocity(A1Lam);

  if(!GoB->mIsJoint)
    GoB->SetVelocity(V2Lam);
  GoB->SetAnguVelocity(A2Lam);
}

void Resolution::UpdatePenetration(Contacts* C, ContactsManager* CM)
{
  CPhysics* GoA = C->contacts[0]; // + normal
  CPhysics* GoB = C->contacts[1]; // - normal
  Vector ChangePosA = C->changePosA;
  Vector ChangePosB = C->changePosB;
  Vector oriNormal = C->normal;
  for (unsigned i = 0; i < CM->NumContacts; ++i)
  {
    if (&(CM->ContactList[i]) != C)
    {
      if(CM->ContactList[i].contacts[0] == GoA || CM->ContactList[i].contacts[1] == GoA)
      {
        float changeNormalScale = CM->ContactList[i].normal.Dot(ChangePosA);
        Vector changeNormal = CM->ContactList[i].normal * changeNormalScale;
        if(CM->ContactList[i].contacts[0] == GoA)
          CM->ContactList[i].penetration -= changeNormalScale;
        else
          CM->ContactList[i].penetration += changeNormalScale;

      }
      else if (CM->ContactList[i].contacts[0] == GoB || CM->ContactList[i].contacts[1] == GoB)
      {
        float changeNormalScale = CM->ContactList[i].normal.Dot(ChangePosB);
        Vector changeNormal = CM->ContactList[i].normal * changeNormalScale;
        if(CM->ContactList[i].contacts[0] == GoB)
          CM->ContactList[i].penetration -= changeNormalScale;
        else
          CM->ContactList[i].penetration += changeNormalScale;
      }
    }
  }
}
void Resolution::ResetLamda(ContactsManager* CM)
{
  unsigned int num = CM->NumContacts;
  for (unsigned j = 0; j < num ; ++j)
  {
      CM->ContactList[j].L1 = 0;
      CM->ContactList[j].L2 = 0;
  }
}
void Resolution::ResolveContactList(ContactsManager* CM)
{
  
  unsigned int num = CM->NumContacts;
  bool IsBOxToBox;
  ResetLamda(CM);
    for (unsigned i = 0; i < 3 ; ++i)
  {
    for (unsigned j = 0; j < num ; ++j)
    {
	  if(CM->ContactList[j].collsionPair == BoxToBox)
		  IsBOxToBox = true;
	  else
		  IsBOxToBox = false;
	  if(IsBOxToBox)
	  {
		ResolveFriction(&CM->ContactList[j],1);
		ResolveFriction(&CM->ContactList[j],2);
	  }
	  else
	  {
		  if(i == 0)
            ResolveFriction(&CM->ContactList[j],1);
	  }
    }
  }
  for (unsigned i = 0; i < 5 ; ++i)
  {
    for (unsigned j = 0; j < num ; ++j)
    {
		if(CM->ContactList[j].collsionPair == BoxToBox)
			IsBOxToBox = true;
		else
			IsBOxToBox = false;
		if(IsBOxToBox)
		{
			ResolveVelocity(&CM->ContactList[j],1);
			ResolveVelocity(&CM->ContactList[j],2);
		}
		else
		{
			if(i == 0)
				ResolveVelocity(&CM->ContactList[j],1);
		}
    }
  }

  ResetLamda(CM);
  
  for (unsigned i = 0; i < 5 ; ++i)
 {
    for (unsigned j = 0; j < num ; ++j)
    {
		if(CM->ContactList[j].collsionPair == BoxToBox)
			IsBOxToBox = true;
		else
			IsBOxToBox = false;
		if(IsBOxToBox)
		{
			ResolvePenetrationConstraint(&CM->ContactList[j],1);  //first contact pair
			ResolvePenetrationConstraint(&CM->ContactList[j],2);  //second contact pair
		}
		else
		{
			if(i == 0)
				ResolvePenetrationConstraint(&CM->ContactList[j],1);
		}
      //ResolvePenetration(&CM->ContactList[j],1,CM);
     // ResolvePenetration(&CM->ContactList[j],2,CM);
    }
  }
  
  // impulse solving position
  /*
  for (unsigned i = 0; i < 5 ; ++i)
  {
    for (unsigned j = 0; j < num ; ++j)
    {

      ResolvePenetrationImpulse(&CM->ContactList[j],1,CM);
    }
  }
  */
  /*
  for (unsigned i = 0; i < CM->NumContacts * 10 ; ++i)
  {
    Contacts* firstResolve = FindMostPenetration(CM);
    if(firstResolve == NULL)
      break;
    ResolveVelocity(firstResolve);
    if(firstResolve->penetration > EPSLON)
    {

    }
    firstResolve->penetration = 0.0f;
  }
  */
}
/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Resolution Class