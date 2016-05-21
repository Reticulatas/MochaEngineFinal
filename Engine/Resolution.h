/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */


#pragma once
#include"Collision.h"


class ContactsManager
{
public:
	ContactsManager();
	
  static const unsigned MaxNum = 200;
  unsigned NumContacts;
  Contacts ContactList[MaxNum];
};

class Resolution
{
public:
  static void ResolveContactList(ContactsManager* CM);
private:
  static float PositionConstrain(Contacts* C, int numPene);
  static float VelocityConstraint(Contacts* C, int numPene);
  static void UpdateWorldMatrix(Contacts* C);
  static float GenerateImpulse(Contacts* C, Vector*normal, Vector*r1, Vector*r2, int numContact);
  static Contacts* FindMostPenetration(ContactsManager* CM);
  static void ResolveVelocity(Contacts* C, int numPene);
  static void ResolveVelocityImpulse(Contacts* C, int numPene);
  static void ResolveFriction(Contacts* C, int numContact);
  static Vector GetRelativeVelocity(Contacts* C, int numContact);
  static void ResolvePenetrationImpulse(Contacts* C, int numPene,ContactsManager* CM);
  static void ResolvePenetrationConstraint(Contacts* C, int numPene);
  static void UpdatePenetration(Contacts* C, ContactsManager* CM);
  static void ResetLamda(ContactsManager* CM);
  static float ClampLamda(float lamda);
};
/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Resolution Class