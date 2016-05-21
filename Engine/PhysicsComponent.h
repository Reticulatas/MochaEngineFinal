/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "Component.h"
#include "Collision.h"
#include "Vector.h"
class CPhysics : public Component
{
	public:
		CPhysics();
		~CPhysics();
	
		virtual void OnStart(void);
		virtual void OnUpdate(void);  
		virtual void OnFree(void);

		void Integrate(float dt);
		void SetVelocity(Vector3D);
		void SetPosition(Vector3D);
		void AddForce(Vector3D force);
		/* Debug draw */

		Vector3D Position;
		Vector3D Velocity;
		Vector3D Acceleration;
		Vector3D PrevPosition;
		Vector3D AccumulatedForce;
		float Mass;
		float I_Mass;
		float Force;
		float Friction;
		float Damping;

		//Collision shape
		Shape * CollisionShape;
		//Static object or not
		bool IsStatic;
};

/////////////////////////////////////
//Original Author: Kai Chuan Hsiao
//Modifications:
//	Date	-	Change
//  13/5/13  -   Physics Class