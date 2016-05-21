/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "GameState.h"
#include "GameObject.h"
#include "CMeshRenderer.h"
#include "CSkinMeshRenderer.h"
#include "CPhysics.h"
#include "CTransform.h"
#include "InputManager.h"
#include "CScript.h"
#include "CCamera.h"
#include "CLight.h"
#include "ObjectFactory.h"
#include "TextureNode.h"
#include "CAnimationController.h"
#include "CSound.h"
#include "CAudioGeometry.h"


#define boxsize 3.51747632f

static CPhysics* control = NULL;
float size = 0.22516f; // size 1
//float size = 1;
static bool isFirst = true;
int force = 10;
Vector3 dir[4] = { Vector3(700, 200, 700), Vector3(-700, 200, 700),
Vector3(700, 200, -700), Vector3(-700, 200, -700) };

CPhysics* Camcphy = NULL;
CCamera* c = NULL;
GameObject* cam = NULL;
CPhysics* Playerphy = NULL;
GameObject* Player = NULL;
bool DoItOnce = false;
GameObject* CreateWithTexture(GameState& gs, Vector3& pos, Vector3& halfExtend,
	bool isRotation, ResolutionMaterial& rm, unsigned char type,
	std::string& texture_name, Vector3& rotation, Vector3& scale)
{
	GameObject* g1;
	// setup texture
	if (isFirst)
		g1 = ObjectFactory::getInstance()->MakeNewGameObject("Box1", &gs);
	else if (type == 0)
		g1 = ObjectFactory::getInstance()->MakeNewGameObject("Ground", &gs);
	else
		g1 = ObjectFactory::getInstance()->MakeNewGameObject("Box", &gs);

	// TODOKai find out scale
	CMeshRenderer* rend = g1->AddComponent<CMeshRenderer>();
	rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));
	TextureNode* tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
	tNode->SetTexture("Diffuse.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(1));
	tNode->SetTexture("Wall_normals.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(2));
	tNode->SetTexture("Wall_spec.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
	// setup collider
	g1->transform->SetPosition(pos);
	g1->transform->rotation.Angles = rotation;
	g1->transform->scale = scale;
	return g1;
	//auto cphy1 = g1->AddComponent<CPhysics>();
	//cphy1->CreateRigidBodyComponent(type);
	//cphy1->CreateBoxColliderComponent(Vector3(halfExtend.x * scale.x, halfExtend.y * scale.y, halfExtend.z * scale.z));
	//cphy1->rigidCom->SetLinearVelocity(Vector3(0, 0, 0));
	//cphy1->rigidCom->SetAngularVelocity(Vector3(0, 0, 0));
	//cphy1->rigidCom->SetOrientation(rotation);
	//cphy1->rigidCom->SetPosition(pos);
	//cphy1->mCollisionID = 0;
	//// setup resolution material
	//cphy1->collidarCom->SetMaterial(rm);
	//// setup rotated or not
	//cphy1->rigidCom->SetAllowRotation(isRotation);
	//if (isFirst && type)
	//{
	//	control = cphy1;
	//	isFirst = false;
	//}
	//return cphy1;
}
GameObject* CreateBox(GameState& gs, Vector3& pos, Vector3& halfExtend,
	bool isRotation, ResolutionMaterial& rm, unsigned char type,
	std::string& texture_name, Vector3& rotation, Vector3& scale)
{
	GameObject* g1;
	// setup texturea
	if (isFirst)
		g1 = ObjectFactory::getInstance()->MakeNewGameObject("Box1", &gs);
	else if (type == 0)
		g1 = ObjectFactory::getInstance()->MakeNewGameObject("Ground", &gs);
	else
		g1 = ObjectFactory::getInstance()->MakeNewGameObject("Box", &gs);

	g1->transform->SetPosition(pos);
	g1->transform->scale = scale;
	g1->transform->rotation.Angles = rotation;
	// TODOKai find out scale
	return g1;
	// setup collider
	
	
	//auto cphy1 = g1->AddComponent<CPhysics>();
	//cphy1->CreateRigidBodyComponent(type);
	//cphy1->CreateBoxColliderComponent(Vector3(halfExtend.x * scale.x, halfExtend.y * scale.y, halfExtend.z * scale.z));
	//cphy1->rigidCom->SetLinearVelocity(Vector3(0, 0, 0));
	//cphy1->rigidCom->SetAngularVelocity(Vector3(0, 0, 0));
	////rotation.SetIdentity();
	////cphy1->rigidCom->SetOrientation(rotation);
	//cphy1->rigidCom->SetPosition(pos);
	//cphy1->mCollisionID = 0;
	//// setup resolution material
	//cphy1->collidarCom->SetMaterial(rm);
	//// setup rotated or not
	//cphy1->rigidCom->SetAllowRotation(isRotation);
	//if (isFirst && type)
	//{
	//	control = cphy1;
	//	isFirst = false;
	//}
	//return cphy1;
}

CPhysics* CreateSphere(GameState& gs, Vector3& pos, float radius,
	ResolutionMaterial& rm, unsigned char type,
	std::string& texture_name)
{
	// setup texture
	GameObject* g1 = ObjectFactory::getInstance()->MakeNewGameObject("Sphere", &gs);
	CMeshRenderer* rend = g1->AddComponent<CMeshRenderer>();
	rend->SetMesh(AssetManager::getInstance()->GetAsset("Sphere.bean"));
	TextureNode* tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
	tNode->SetTexture("Wall.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(1));
	tNode->SetTexture("Wall_normals.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(2));
	tNode->SetTexture("Wall_spec.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);

	// TODOKai find out scale

	// setup collider
	auto cphy1 = g1->AddComponent<CPhysics>();
	g1->transform->SetPosition(pos);
	g1->transform->scale = Vector3(radius, radius, radius);
	cphy1->CreateRigidBodyComponent(type);
	cphy1->CreateSphereColliderComponent(radius);
	cphy1->rigidCom->SetLinearVelocity(Vector3(0, 0, 0));
	cphy1->rigidCom->SetAngularVelocity(Vector3(0, 0, 0));
	cphy1->rigidCom->SetPosition(pos);
	cphy1->mCollisionID = 1;
	// setup resolution material
	cphy1->collidarCom->SetMaterial(rm);

	return cphy1;
}

CPhysics* CreateMesh(GameState& gs
	, Vector3& pos
	, const std::string& filename
	, unsigned char type)
{
	GameObject* g1 = ObjectFactory::getInstance()->MakeNewGameObject("Mesh", &gs);
	g1->transform->SetPosition(pos);
	g1->transform->scale = Vector3(20, 20, 20);
	CMeshRenderer* rend = g1->AddComponent<CMeshRenderer>();
	rend->SetMesh(AssetManager::getInstance()->GetAsset("Crate.bean"));
	TextureNode* tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
	tNode->SetTexture("Diffuse.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(1));
	tNode->SetTexture("Normal.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(2));
	tNode->SetTexture("Specular.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);

	auto cphy1 = g1->AddComponent<CPhysics>();
	cphy1->CreateRigidBodyComponent(type);
	cphy1->CreateMeshColliderComponent(filename);
	cphy1->mCollisionID = 2;
	return cphy1;
}

void MakeWall()
{


}

void GameState::LoadState()
{
	//Implement Any asset loading here

}

void GameState::UpdateState()
{
	BaseState::UpdateState();
//	if (Playerphy != NULL && Player != NULL)
//	{
//		float dx = Math::DegToRad(0.1f*static_cast<float>(Input::GetMouseDeltaX()));
//		float dy = Math::DegToRad(0.1f*static_cast<float>(Input::GetMouseDeltaY()));
//	
//		c->Pitch(dy);
//		c->RotateY(dx);
//	}
//	
//	float max = 20.f;
//	float speed = 1.2f;
//	float acce = 15.f;
//	Vector3 dir = Player->transform->GetForwardVector();
//	dir.y = 0;
//	Vector3 right = Player->transform->GetRightVector();
//	right.y = 0;
//	const Vector3 vel = Playerphy->rigidCom->Body().LinearVelocity();
//	Vector3 newVel;
//	bool NoInput = true;
//	if (Input::GetHeld(0, "up"))
//	{
//		if (vel.y == 0)
//		NoInput = false;
//		newVel = vel + dir * speed;
//		newVel.y = 0;
//		if (newVel.LengthSq() < max * max)
//			Playerphy->rigidCom->Body().SetLinearVelocity(newVel);
//		else
//		{
//			newVel = newVel.Normalized() * max;
//			Playerphy->rigidCom->Body().SetLinearVelocity(newVel);
//		}
//	}
//	if (Input::GetHeld(0, "down"))
//	{
//		NoInput = false;
//		newVel = vel - dir * speed;
//		newVel.y = 0;
//		if (newVel.LengthSq() < max * max)
//			Camcphy->rigidCom->Body().SetLinearVelocity(newVel);
//		else
//		{
//			newVel = newVel.Normalized() * max;
//			Camcphy->rigidCom->Body().SetLinearVelocity(newVel);
//		}
//	}
//	if (Input::GetHeld(0, "right"))
//	{
//		NoInput = false;
//		newVel = vel + right * speed;
//		newVel.y = 0;
//		if (newVel.LengthSq() < max * max)
//			Camcphy->rigidCom->Body().SetLinearVelocity(newVel);
//		else
//		{
//			newVel = newVel.Normalized() * max;
//			Camcphy->rigidCom->Body().SetLinearVelocity(newVel);
//		}
//	}
//	if (Input::GetHeld(0, "left"))
//	{
//		NoInput = false;
//		newVel = vel - right * speed;
//		newVel.y = 0;
//		if (newVel.LengthSq() < max * max)
//			Camcphy->rigidCom->Body().SetLinearVelocity(newVel);
//		else
//		{
//			newVel = newVel.Normalized() * max;
//			Camcphy->rigidCom->Body().SetLinearVelocity(newVel);
//		}
//	}
//	if (Input::GetTriggered(0, "Space"))
//	{
//		NoInput = false;
//		Camcphy->rigidCom->Body().ApplyLinearImpulse(Vector3(0, 2500, 0));
//		//Vector3 vel(0, 0, 0);
//		//vel.y = 10;
//		//Camcphy->rigidCom->Body().SetLinearVelocity(vel);
//	}
//	if (NoInput)
//		Camcphy->rigidCom->Body().SetLinearVelocity(Vector3(0, 0, 0));
	if (cam->transform->position().z < 200 && !DoItOnce)
	{
		DoItOnce = true;
		// Boxes
		for (int i = 0; i < 0; ++i)
		{
			for (int j = 0; j < 0; ++j)
			{
				for (int k = 0; k < 0; ++k)
				{
					ResolutionMaterial rm;
					rm.mRestitution = 0.0f; rm.mFriction = 1.0f; rm.mDensity = 1.0f;
					std::string s("Diffuse.png");
					Vector3 rot;
					rot.ZeroOut();

					Vector3 scale; scale.Set(0.8f, 0.8f, 0.8f);
					CreateWithTexture(*this, Vector3(0 + i * boxsize * 1.2f + 0.1f, 5 + j *  boxsize * 1.2f + 0.1f, 100 + k * boxsize * 1.2f + 0.1f),
						Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody::DYNAMIC, s, rot, scale);
				}
			}
		}

		for (int i = 0; i < 0; ++i)
		{
			for (int j = 0; j < 0; ++j)
			{
				for (int k = 0; k < 0; ++k)
				{
					//a
					ResolutionMaterial rm;
					rm.mRestitution = 0.0f; rm.mFriction = 1.0f; rm.mDensity = 1.0f;
					std::string s("Diffuse.png");
					Vector3 rot;
					rot.ZeroOut();

					Vector3 scale; scale.Set(2.0f, 2.0f, 2.0f);
					CreateWithTexture(*this, Vector3(16 + i * boxsize * size + 0.1f, 10 + j *  boxsize * size + 0.1f, 65 + k * boxsize * size + 0.1f),
						Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody::DYNAMIC, s, rot, scale);
				}
			}
		}
	}
}

GameObject* CreateWall(GameState* me, const Vector3& position)
{
	GameObject*g = ObjectFactory::getInstance()->MakeNewGameObject("Wall", me);
	g->transform->SetPosition(position);
	CMeshRenderer*	  rend = g->AddComponent<CMeshRenderer>();
    g->AddComponent<CAudioGeometry>();
	rend->SetMesh(AssetManager::getInstance()->GetAsset("Wall.bean"));
	TextureNode*tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
	tNode->SetTexture("Wall.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(1));
	tNode->SetTexture("Wall_normals.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(2));
	tNode->SetTexture("Wall_spec.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
	return g;
}
void GameState::InitState()
{
	ResolutionMaterial rm;
	std::string s("default.png");
	Vector3 rot;
  Vector3 scale;
	CMeshRenderer* rend;
	TextureNode* tNode;
  GameObject* g;
  //
  //
  //
  //
	////////////////////////////////////
	/////////PHYSICS SCENE
	//////////////////////////////////
	// Ground
	//rm.mRestitution = 0.0f; rm.mFriction = 1.0f; rm.mDensity = 1.0f;
	// scale.Set(5.37179f, 1.75873816f / 3, 5.37179f);
	// CreateWithTexture(*this, Vector3(16, -boxsize + 30, 58),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//////////////////////
	/////Boxes
	//////////////////////
	//for (int i = 0; i < 0; ++i)
	//{
	//	for (int j = 0; j < 0; ++j)
	//	{
	//		for (int k = 0; k < 0; ++k)
	//		{
	//			ResolutionMaterial rm;
	//			rm.mRestitution = 0.0f; rm.mFriction = 1.0f; rm.mDensity = 1.0f;
	//			std::string s("Diffuse.png");
	//			Matrix3 rot;
	//			rot.SetIdentity();
  //
	//			Vector3 scale; scale.Set(1.5f, 1.5f, 1.5f);
	//			CreateWithTexture(*this, Vector3(16 + i * boxsize * size + 0.1f, 10 + j *  boxsize * size + 0.1f, 58 + k * boxsize * size + 0.1f),
	//				Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::DYNAMIC, s, rot, scale);
	//		}
	//	}
	//}
	////////////////////////
	/////Sphere
	//////////////////////
	//for (int i = 0; i < 0; ++i)
  //{
  //	for (int j = 0; j < 0; ++j)
  //	{
  //		for (int k = 0; k < 0; ++k)
  //		{
  //
  //			ResolutionMaterial rm;
  //			rm.mRestitution = 0.5f; rm.mFriction = 1.0f; rm.mDensity = 1.0f;
  //			std::string s("Diffuse.png");
  //
  //			float radius = RandomFloat(0.4f, 1.0f);
  //			CreateSphere(*this, Vector3(0 + i * 5 * radius + 0.1f, 15 + j * 2 + 0.1f, 0 + k * 5 * radius + 0.1f)
  //				, radius, rm, RigidBody3D::DYNAMIC, s);
  //		}
  //	}
  //}
  //
  //for (int i = 0; i < 0; ++i)
  //{
  //	for (int j = 0; j < 0; ++j)
  //	{
  //		for (int k = 0; k < 0; ++k)
  //		{
  //			ResolutionMaterial rm;
  //			rm.mRestitution = 0.0f; rm.mFriction = 1.0f; rm.mDensity = 1.0f;
  //			std::string s("Diffuse.png");
  //			Matrix3 rot;
  //			rot.SetIdentity();
  //
  //			Vector3 scale; scale.Set(size, size, size);
  //			CreateMesh(*this, Vector3(0 + i * 150 * size + 0.1f, 0 + j * 15 * size + 0.1f, 0 + k * 15 * size + 0.1f),
  //				"E://cube.obj", RigidBody3D::DYNAMIC);
  //		}
  //	}
  //}
  //
	////////////////////////////////////
	/////////GameDemo SCENE
	//////////////////////////////////
  //
	//static float GRAPHICSOFFSET = 100.f;
  //
	//rm.mRestitution = 0.0f; rm.mFriction = 0.7f; rm.mDensity = 1.0f;
	//rot.SetIdentity();
  //
  Input::SetMouseLock(true);
  Input::SetMouseVisible(false);
  //	
  // mesh box
//  scale.Set(1.f, 1.f, 1.f);
//  g = CreateBox(*this, Vector3(3, 3, 3),
//	  Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
//  g->AddComponent<CScript>()->AddScript("AddPhyMesh.cs");
//  rend = g->AddComponent<CMeshRenderer>();
//  rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));
//  // mesh box
//  scale.Set(1.f, 1.f, 1.f);
//  g = CreateBox(*this, Vector3(5, 5, 5),
//	  Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
//  g->AddComponent<CScript>()->AddScript("AddPhyMesh.cs");
//  rend = g->AddComponent<CMeshRenderer>();
//  rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));
  //Player = ObjectFactory::getInstance()->MakeNewGameObject("Player2", this);
  //Player->transform->SetPositionY( 5);
  //Player->transform->SetPositionZ( 3);
  //Player->transform->scale.y = 2.f;
  //Player->AddComponent<CScript>()->AddScript("FirstPersonPlayer.cs");
  //rend = Player->AddComponent<CMeshRenderer>();
  //rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));
  //Player->transform->rotation.Angles.y = 1.57f;
	////Player
	Player = ObjectFactory::getInstance()->MakeNewGameObject("Player", this);
	Player->transform->SetPositionY( 50);
	Player->transform->SetPositionZ( 30);
	Player->transform->scale.y = 2.f;
	rend = Player->AddComponent<CMeshRenderer>();
	rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));
	//Player->AddComponent<CScript>()->AddScript("FirstPersonPlayer.cs");


	//// Ground
	scale.Set(200.f, 1.f, 400.f);
	g = CreateWithTexture(*this, Vector3(0, 0, 30),
		Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody::STATIC, s, rot, scale);
	//rend = g->AddComponent<CMeshRenderer>();
	//rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));
	//g->AddComponent<CScript>()->AddScript("AddPhyMesh.cs");

  //
  //
	//Playerphy = Player->AddComponent<CPhysics>();
	//Playerphy->CreateRigidBodyComponent(RigidBody3D::DYNAMIC);
	//Playerphy->CreateBoxColliderComponent(Vector3(boxsize*0.5f * Player->transform->scale.x,
	//	boxsize*0.5f * Player->transform->scale.y,
	//	boxsize*0.5f * Player->transform->scale.z));
	//Playerphy->rigidCom->SetPosition(Player->transform->position);
	//rot.SetIdentity();
	//Playerphy->rigidCom->SetOrientation(rot);
	//Playerphy->rigidCom->SetAllowRotation(0);
	//Playerphy->collidarCom->SetMaterial(rm);
  //  Player->AddComponent<CSound>();

  //  Player->AddComponent<CSound>();

	//Cam
	cam = ObjectFactory::getInstance()->MakeNewGameObject("Camera", this);
	c = cam->AddComponent<CCamera>();
  cam->transform->SetPositionY( 10);
  cam->transform->SetPositionZ( -13);
  cam->transform->rotation.Angles.y = 3.141592f;
  c->mCamType = CCamera::MAIN;
  //Player->AddComponent<CSound>();
	//cam->AddComponent<CScript>()->AddScript("FirstPersonCamera.cs");

  // Cam for oculus
  cam = ObjectFactory::getInstance()->MakeNewGameObject("Camera", this);
  c = cam->AddComponent<CCamera>();
  cam->transform->SetPositionY(6.f);
  cam->transform->SetPositionZ(50);
  c->mCamType = CCamera::VR;
  c->SetRenderTarget(AssetManager::getInstance()->GetAsset("SecondaryRT.mrt"));

  g = ObjectFactory::getInstance()->MakeNewGameObject("quad", this);
  g->transform->SetPositionY( 10);
  g->transform->SetPositionZ( 40);
  g->transform->scale = Vector3(20, 10, 1);
  g->transform->rotation.Angles.y = 3.14159f;
  rend = g->AddComponent<CMeshRenderer>();
  rend->SetMesh(AssetManager::getInstance()->GetAsset("Quad.bean"));
  tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
  tNode->SetTexture(c->GetRenderTargetAssetHandle());
  tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);

  // Test surveillance cam
  cam = ObjectFactory::getInstance()->MakeNewGameObject("CameraTest", this);
  c = cam->AddComponent<CCamera>();
  cam->transform->SetPositionX( -50);
  cam->transform->SetPositionY( 10);
  cam->transform->SetPositionZ( 50);
  cam->transform->rotation.Angles.y = 3.141592f * .5f;

  g = ObjectFactory::getInstance()->MakeNewGameObject("quad", this);
  g->transform->SetPositionY( 10);
  g->transform->SetPositionZ( 10);
  g->transform->scale = Vector3(20, 10, 1);
  g->transform->rotation.Angles.y = 3.14159f;
  rend = g->AddComponent<CMeshRenderer>();
  rend->SetMesh(AssetManager::getInstance()->GetAsset("Quad.bean"));
  tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
  tNode->SetTexture(c->GetRenderTargetAssetHandle());
  tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);

  //tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(2));
  //tNode->SetTexture("MonitorSpec.png");
  //tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  
	// WALLS
	//CreateWall(this, Vector3(0,0,0));	
	//CreateWall(this, Vector3(0, 0, 60));


  //rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));
  //g->AddComponent<CScript>()->AddScript("AddPhy.cs");
  //rend = g->AddComponent<CMeshRenderer>();
  //rend->SetMesh(AssetManager::getInstance()->GetAsset("cube.bean"));


	//
	//// Walls
	//scale.Set(10.75f, 1.f, 90.f);
	//CPhysics* wallphy = CreateBox(*this, Vector3(16, 7, 595.6f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(10.75f, 1.f, 90.f);
	//wallphy = CreateBox(*this, Vector3(-16, 7, 595.6f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
  //
  //
	//// Walls
	//scale.Set(10.75f, 0.6f, 55.f);
	//wallphy = CreateBox(*this, Vector3(78, 7, 340.9),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(10.75f, 0.6f, 55.f);
	//wallphy = CreateBox(*this, Vector3(-82, 7, 340.9),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//// Walls
	//scale.Set(10.75f, 1.f, 42.f);
	//wallphy = CreateBox(*this, Vector3(15.4f, -boxsize * 0.5f, 180.0f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(10.75f, 1.f, 42.f);
	//wallphy = CreateBox(*this, Vector3(-15.4f, -boxsize * 0.5f, 180.0f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //  
	//// Walls
	//scale.Set(20.f, 0.6f, 50.f);
	//wallphy = CreateBox(*this, Vector3(63, -boxsize * 0.5f, 38.0f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(20.f, 0.6f, 50.f);
	//wallphy = CreateBox(*this, Vector3(-63, -boxsize * 0.5f, 38.0f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//// Platform
	//scale.Set(20.f, 0.6f, 50.f);
	//wallphy = CreateBox(*this, Vector3(25, -18.f, 2.2f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.y = 1.57f;
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(20.f, 0.6f, 50.f);
	//wallphy = CreateBox(*this, Vector3(25, -18.f, 51.0f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.y = 1.57f;
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(13.8f, 0.6f, 44.4f);
	//wallphy = CreateBox(*this, Vector3(16.3f, 16.2f, 27.1f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.y = 1.57f;
  //
  //
	//// Walls
	//scale.Set(10.75f, 1.f, 42.f);
	//wallphy = CreateBox(*this, Vector3(-15, -boxsize * 0.5f, -123),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(10.75f, 1.f, 42.f);
	//wallphy = CreateBox(*this, Vector3(15, -boxsize * 0.5f, -123),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
  //
  //
	//// Walls
	//scale.Set(50.f, 0.6f, 55.f);
	//wallphy = CreateBox(*this, Vector3(83, -boxsize * 0.5f, -290.f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
	//scale.Set(50.f, 0.6f, 55.f);
	//wallphy = CreateBox(*this, Vector3(-83, -boxsize * 0.5f, -290.f),
	//	Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
	//wallphy->gameObject->transform->rotation.Angles.z = 1.57f;
  //
    g = ObjectFactory::getInstance()->MakeNewGameObject("BGAudio",this);

    g->transform->SetPositionY( 0);
    g->transform->SetPositionZ( 3);
    CSound* sound = g->AddComponent<CSound>();
  //
  //  g = ObjectFactory::getInstance()->MakeNewGameObject("BGAudio",this);
  //  g->transform->position.z += GRAPHICSOFFSET;
  //  sound = g->AddComponent<CSound>();
  //  g->AddComponent<CScript>()->AddScript("BGAudio.cs");
  //
	//g = ObjectFactory::getInstance()->MakeNewGameObject("DoorFrame", this);
	//g->transform->position.z += GRAPHICSOFFSET;
	//rend = g->AddComponent<CMeshRenderer>();
	//rend->SetMesh(AssetManager::getInstance()->GetAsset("roomDoorFrame.bean"));
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
	//tNode->SetTexture("Door.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("normalTexture"));
	//tNode->SetTexture("Door_normals.png");  
	//tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("specTexture"));
	//tNode->SetTexture("Door_spec.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  //
	//g = ObjectFactory::getInstance()->MakeNewGameObject("Pillars", this);
	//g->transform->position.z += GRAPHICSOFFSET;
	//rend = g->AddComponent<CMeshRenderer>();
	//rend->SetMesh(AssetManager::getInstance()->GetAsset("roomPillars.bean"));
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
	//tNode->SetTexture("Door.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("normalTexture"));
	//tNode->SetTexture("Door_normals.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("specTexture"));
	//tNode->SetTexture("Door_spec.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  //
	//g = ObjectFactory::getInstance()->MakeNewGameObject("Door", this);
	//g->transform->SetPositionY( -0.4f;
	//g->transform->SetPositionZ( 745.3f;
	//g->transform->scale.x = 1.1f; g->transform->scale.y = 1.1f; g->transform->scale.z = 1.1f;
	//g->transform->rotation.Angles.y = 3.1415926f;
	//rend = g->AddComponent<CMeshRenderer>();
	//rend->SetMesh(AssetManager::getInstance()->GetAsset("door.bean"));
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
	//tNode->SetTexture("Door.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("normalTexture"));
	//tNode->SetTexture("Door_normals.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("specTexture"));
	//tNode->SetTexture("Door_spec.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  //  g->AddComponent<CSound>();
  //g->AddComponent<CScript>()->AddScript("Door.cs");
  //
  //g = ObjectFactory::getInstance()->MakeNewGameObject("MochaStudios", this);
  //g->transform->SetPositionX( .5f;
  //g->transform->SetPositionY( 7.f;
  //g->transform->SetPositionZ( 770.f;
  //g->transform->scale = Vector3(3.1f,1.f,.2f);
  //rend = g->AddComponent<CMeshRenderer>();
  //rend->SetMesh(AssetManager::getInstance()->GetAsset("Crate.bean"));
  //tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
  //tNode->SetTexture("SplashScreen.png");
  //tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
  //tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("normalTexture"));
  //tNode->SetTexture("Wall_normals.png");
  //tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
  //tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("specTexture"));
  //tNode->SetTexture("Wall_spec.png");
  //tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  
  g = ObjectFactory::getInstance()->MakeNewGameObject("SSLight", this);
  CLight* light = g->AddComponent<CLight>();
  light->SetLightType(CLight::POINT);
  light->SetRange(9);
  light->SetDiffuse(COLOR::WHITE);
  light->SetAtt(3.5f);
  light->mInnerAngle = 20;
  light->mOuterAngle = 50;
  light->SetSpot(5);
  g->transform->SetPositionY( 7.6f);
  g->transform->SetPositionZ( 770.f);
  g->transform->rotation.Angles.x = -1.f;
  
	//g = ObjectFactory::getInstance()->MakeNewGameObject("SecurityCamera", this);
  //g->transform->SetPositionX( 77.f;
  //g->transform->SetPositionY( 20.f;
  //g->transform->SetPositionZ( -270.f;
	//g->transform->scale.x = 15.f;
	//g->transform->scale.y = 15.f;
	//g->transform->scale.z = 15.f;
  //g->transform->rotation.Angles.y = 1.6f;
  //rend = g->AddComponent<CMeshRenderer>();
  //rend->SetMesh(AssetManager::getInstance()->GetAsset("securityCam.bean"));
  //tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
  //tNode->SetTexture("DoorOpen.png");
	//tNode->SetTexture("DoorOpen.png");
	//g->AddComponent<CScript>()->AddScript("LookAtPlayer.cs");
  
	//corridor lights
	for (int i = 0; i < 3; ++i)
	{
		g = ObjectFactory::getInstance()->MakeNewGameObject("CorridorLight", this);
		CLight* light = g->AddComponent<CLight>();
		light->SetLightType(CLight::LIGHTTYPE::SPOTLIGHT);
		light->SetRange(100);
		light->SetSpot(10.f);
		light->mInnerAngle = 20;
		light->mOuterAngle = 90;
		light->SetDiffuse(Color(1.0f, 0, 0));
		light->SetAtt(4.f);
		g->transform->SetPositionY( -10.f);
		g->transform->SetPositionZ( 740.f - (i*70.f));
		g->transform->rotation.Angles.x = 3.14f;
    //g->AddComponent<CScript>()->AddScript("PulsingLight.cs");
	}

	//g = ObjectFactory::getInstance()->MakeNewGameObject("Pillars", this);
	//g->transform->position.z += GRAPHICSOFFSET;
	//rend = g->AddComponent<CMeshRenderer>();
	//rend->SetMesh(AssetManager::getInstance()->GetAsset("roomPillars.bean"));
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
	//tNode->SetTexture("Door.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("normalTexture"));
	//tNode->SetTexture("Door_normals.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("specTexture"));
	//tNode->SetTexture("Door_spec.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  //
	////////////////////////////////////
	/////////GRAPHICS SCENE
	//////////////////////////////////
	//  rm.mRestitution = 0.0f; rm.mFriction = 1.0f; rm.mDensity = 1.0f;
	//  rot.SetIdentity();
	//  scale.Set(10.75f, 0.6f, 19.3f);
	//  CreateBox(*this, Vector3(0, -boxsize * 0.5f, 47.1),
	//	  Vector3(0.5f * boxsize, 0.5f * boxsize, 0.5f * boxsize), true, rm, RigidBody3D::STATIC, s, rot, scale);
  //
  
	//g = ObjectFactory::getInstance()->MakeNewGameObject("Tad", this);
	//g->transform->SetPositionZ( -50;
  //g->transform->scale = Vector3(.1f, .1f, .1f);
	//CSkinMeshRenderer* srend = g->AddComponent<CSkinMeshRenderer>();  
	//tNode = dynamic_cast<TextureNode*>(srend->GetMaterial()->GetAttribute<TextureNode>(0));
	//tNode->SetTexture("Tad.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	//tNode = dynamic_cast<TextureNode*>(srend->GetMaterial()->GetAttribute("normalTexture"));
	//tNode->SetTexture("Tad.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	//tNode = dynamic_cast<TextureNode*>(srend->GetMaterial()->GetAttribute("specTexture"));
	//tNode->SetTexture("Tad.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
	
  //srend->SetMesh(AssetManager::getInstance()->GetAsset("Man.bean"));
  //CAnimationController* animC = g->AddComponent<CAnimationController>();
  //animC->AddAnimation(AssetManager::getInstance()->GetAsset("ManAgainstWall.anim"));

  g = ObjectFactory::getInstance()->MakeNewGameObject("Doors", this);
  rend = g->AddComponent<CMeshRenderer>();
  rend->SetMesh(AssetManager::getInstance()->GetAsset("Doors.bean"));
  tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
  tNode->SetTexture("Door.png");
  tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
  tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(1));
  tNode->SetTexture("Door_normals.png");
  tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
  tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(2));
  tNode->SetTexture("Door_spec.png");
  tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  //g->AddComponent<CScript>()->AddScript("AddPhyMesh.cs");

	g = ObjectFactory::getInstance()->MakeNewGameObject("Wall", this);
	g->transform->SetPositionZ( 0);
	rend = g->AddComponent<CMeshRenderer>();
	rend->SetMesh(AssetManager::getInstance()->GetAsset("roof1.bean"));
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
	tNode->SetTexture("Wall.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(1));
	tNode->SetTexture("Wall_normals.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(2));
	tNode->SetTexture("Wall_spec.png");
	tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
  
  
	//g = ObjectFactory::getInstance()->MakeNewGameObject("Light", this);
  //light = g->AddComponent<CLight>();
	//light->SetLightType(CLight::POINT);
	//light->SetRange(600);
	//light->SetDiffuse(COLOR::WHITE);
	//light->SetAtt(1.5f);
	//g->transform->SetPositionY( 10.f;
  //g->transform->SetPositionZ( 370;
  
  g = ObjectFactory::getInstance()->MakeNewGameObject("Light", this);
  light = g->AddComponent<CLight>();
  light->SetLightType(CLight::POINT);
  light->SetRange(600);
  light->SetDiffuse(COLOR::WHITE);
  light->SetAtt(1);
  g->transform->SetPositionY( 10.f);
  g->transform->SetPositionZ( 26);

  //g = ObjectFactory::getInstance()->MakeNewGameObject("Light", this);
  //light = g->AddComponent<CLight>();
  //light->SetLightType(CLight::POINT);
  //light->SetRange(600);
  //light->SetDiffuse(COLOR::WHITE);
  //light->SetAtt(1);
  //g->transform->SetPositionY( 10.f);
  //g->transform->SetPositionZ( -270);
  
	//g = ObjectFactory::getInstance()->MakeNewGameObject("turret1", this);
	//rend = g->AddComponent<CMeshRenderer>();
	//rend->SetMesh(AssetManager::getInstance()->GetAsset("turret.bean"));
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
	//tNode->SetTexture("Wall.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("normalTexture"));
	//tNode->SetTexture("Wall_normals.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("specTexture"));
	//tNode->SetTexture("Wall_spec.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
	////g->transform->scale = Vector3(1.9f, .6f, .1f);
	//g->transform->SetPositionX( 62.f);
	//g->transform->SetPositionY( 0.f);
	//g->transform->SetPositionZ( 344.f);
	//g->transform->rotation.Angles.y = 1.57f;
	//g->AddComponent<CScript>()->AddScript("turret.cs");
  //
	////death turret
	//g = ObjectFactory::getInstance()->MakeNewGameObject("turret2", this);
	//rend = g->AddComponent<CMeshRenderer>();
	//rend->SetMesh(AssetManager::getInstance()->GetAsset("turret.bean"));
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("diffuseTexture"));
	//tNode->SetTexture("Wall.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_DIFFUSE);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("normalTexture"));
	//tNode->SetTexture("Wall_normals.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_NORMAL);
	//tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute("specTexture"));
	//tNode->SetTexture("Wall_spec.png");
	//tNode->SetTextureRegister(TextureNode::TEXTURE_SPECULAR);
	////g->transform->scale = Vector3(1.9f, .6f, .1f);
	//g->transform->SetPositionX( -100.f);
	//g->transform->SetPositionY( 0.f);
	//g->transform->SetPositionZ( -275.f);
	//g->transform->rotation.Angles.y = 1.57f;
	//g->AddComponent<CScript>()->AddScript("laserTurret.cs");

	//g = ObjectFactory::getInstance()->MakeNewGameObject("Light5", this);
	//light = g->AddComponent<CLight>();
	//light->SetRange(50);
	//light->SetDiffuse(COLOR::WHITE);
	//light->SetAtt(1.f);
	//g->transform->SetPositionY( 10);
	//g->transform->SetPositionZ( -40 + GRAPHICSOFFSET);

	Renderer::getInstance()->OnResize();
}

void GameState::Draw_State()
{
	//Draw objects here
	BaseState::Draw_State();
}

void GameState::FreeState()
{
	//delete objects here
	Log("GameState Exited, Bye Bye!");
}

void GameState::UnloadState()
{
	//Unload assets here
}

//void EditorState::UpdateState() { BaseState::UpdateState(); }


GameState* GameState::getInstance()
{
	static GameState* instance = 0;

	if (!instance)
		instance = new GameState();

	return instance;
}

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change
