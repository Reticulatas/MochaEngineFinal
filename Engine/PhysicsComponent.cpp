/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "PhysicsComponent.h"
#include  "PhysicsEngine.h"

void CPhysics::OnStart(void)
{
	 // Read in mass, friction, position, 
	 // Set up position
	 /* Position = GetPosition() */
	 /* Set up collision tye */
	 float mass;
	 if(mass > 0)
	 {
		 I_Mass = 1 / mass;
		 IsStatic = false;
	 }

	 else
	 {
		 I_Mass = 0.0f;
		 IsStatic = true;
	 }
	 physicsEngine->CPhysicsList.push_back(this);	 


}
void CPhysics::OnUpdate(void)
{
}
void CPhysics::OnFree(void)
{
}
CPhysics::CPhysics()
{
	 Acceleration = Vector3D(0,0);
	 Position = Vector3D(0,0);
	 PrevPosition = Vector3D(0,0);
	 Velocity = Vector3D(0,0);
	 AccumulatedForce = Vector3D(0,0);
	 Mass = 0.0f;
	 I_Mass = 0.0f;
	 Damping = 0.9f;
	 CollisionShape = NULL;
	 Friction = 0.0f;
	 IsStatic = false;
}
CPhysics::~CPhysics()
{

}


void CPhysics::AddForce(Vector3D force)
{
	 AccumulatedForce += force;
}

void CPhysics::SetPosition(Vector3D position)
{
	 Position = position;
}
void CPhysics::SetVelocity(Vector3D velocity)
{
	 Velocity = velocity;
}

void CPhysics::Integrate(float dt)
{
	if(IsStatic == true)
		return;

	PrevPosition = Position;

	Position = PrevPosition + Velocity * dt;

	Acceleration = physicsEngine->Gravity;

	Velocity = (Velocity + AccumulatedForce * I_Mass + Acceleration) * dt;

	Velocity *= Damping;

	AccumulatedForce = (0, 0);
}