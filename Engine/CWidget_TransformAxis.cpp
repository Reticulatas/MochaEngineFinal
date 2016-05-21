/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
  -* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CWidget_TransformAxis.h"
#include "CTransform.h"
#include "CWidget_Transform.h"
#include "ColorNode.h"
#include "EditorSubsystem.h"
#include "MeshCollider.h"
#include "Plane.h"

//TODO
//For 2D, the naive way of making a transform axis widget must be done (scale in the direction you want to move, no rotation, etc)
//When we migrate to 3D, keep in mind that the ray casting needs to be updated to properly handle scales on the z axis (move with forward vector)

static bool draggingLocked = false;
static const float LENGTHSCALE = 1.0f;

static const float TRANSLATERIGIDTOLERANCE = 5.0f;

void Widget_TransformAxis::OnInit()
{
	draggingColor = Color(1.0f, 1.0f, 0.5f);
	lastFrameMouseOver = false;
	isDragging = false;
	mSpace = LOCAL;
	isGlobal = false;
	hasSwapped = true;
}

void Widget_TransformAxis::OnStart()
{
	RequireDependency<CDiffuseRenderer>();
	myRenderer = gameObject->GetComponent<CDiffuseRenderer>();
	myRenderer->GetMaterial()->SetTexture("BlankWhite.png");
	myRenderer->mDrawOnTop = true;

    if (gameObject->GetName() == "BASE")
        myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("Sphere.bean"));
    else
	    myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("widget_translate.bean"));

	gameObject->SetFlag("Widget");
    myRenderer->mDrawOnTop = true;
	RequireDependency<CMeshColliderComponent>();
	Rescale(0);

}

void Widget_TransformAxis::OnUpdate()
{
	ToggleWidgetSpace();

	if (isDragging)
	{
		

		if (Input::GetValue(0, "MouseLClick") == 0.0f)
		{
			isDragging = false;
			draggingLocked = false;
			cameraDragging = 0;
			return;
		}
		if (!cameraDragging)
			return;
		if (myParent->GetAttachedObjects().empty())
			return;

		
		Vector3 new_pt;
		//form arbitrary plane in local space of object
		Vector3 plane_normal = gameObject->transform->GetRightVector();
		Plane plane;
		plane.Normal = plane_normal;
		plane.Pos = gameObject->transform->GetGlobalPosition();

		//intersect with plane
		Vector3 mouse_ray = EditorSubsystem::getInstance()->EditorCamera()->ScreenToWorldPosition(Input::GetMouseX(true), Input::GetMouseY(true));
		Ray3 mouse_ray3;
		mouse_ray3.dir = mouse_ray.Normalized();
		mouse_ray3.pos = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition();

		Vector3 initial_mouse_ray = EditorSubsystem::getInstance()->EditorCamera()->ScreenToWorldPosition((int)initialDragPoint.x, (int)initialDragPoint.y);
		Ray3 initial_mouse_ray3;
		initial_mouse_ray3.dir = initial_mouse_ray.Normalized();
		initial_mouse_ray3.pos = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition();

		Vector3 intersect_pt;
		Vector3 initial_intersect_pt;

		if (Math::Ray3DToPlane(mouse_ray3, plane, intersect_pt))
		{
			Math::Ray3DToPlane(initial_mouse_ray3, plane, initial_intersect_pt);
			//proj point onto local translation axis
			Vector3 translation_axis = this->gameObject->transform->GetForwardVector();

			Vector3 temp_pt = initial_intersect_pt.Project(translation_axis);
			new_pt = intersect_pt.Project(translation_axis);


			switch (mMode)
			{
			case TRANSLATE:
			{
				if (offsetPoint == Vector3::cZero)
					offsetPoint = new_pt - gameObject->transform->GetGlobalPosition();

				new_pt -= offsetPoint;

				Vector3 gobj_delta_pt = myParent->GetAttachedObjects().at(0).second.position - new_pt;

				Vector3 snapper = gameObject->transform->GetForwardVector().Normalized() * 0.5f;

				float sign = 1.0f;
				float distance = 0.0f;

				auto& gp = myParent->GetAttachedObjects().at(0);

				Vector3 newPos = gp.second.position - gobj_delta_pt;

				Vector3 firstVec = temp_pt - gp.second.position;
				Vector3 secondVec = new_pt - gp.second.position;
				
				Vector3 translationVec = snapper * distance * sign;
				if (firstVec.Dot(secondVec) < 0)
					sign *= -1.0f;

				
                
				if (Input::GetHeld(0, "SnapShift"))
				{
                    if (mAxis == BASE)
                    {
                        Vector3 worldUp(0.0f, 1.0f, 0.0f);

                        Plane transPlane;
                        transPlane.Normal = worldUp;
                        transPlane.Pos = gp.second.position;

                        Vector3 translation_intersect_pt;

                        Math::Ray3DToPlane(mouse_ray3, transPlane, translation_intersect_pt);

                        Vector3 difference = translation_intersect_pt - gp.second.position;

                        difference.x = floorf(difference.x);
                        difference.z = floorf(difference.z);

                        gp.first->transform->SetPosition(gp.second.position + difference);

                        for (int i = 1; i < (int)myParent->GetAttachedObjects().size(); ++i)
                        {
                            auto& childs = myParent->GetAttachedObjects().at(i);

                            Vector3 snappedChildPos = childs.second.position + difference;

                            childs.first->transform->SetPosition(snappedChildPos);
                        }

                    }
                    else
                    {
                        distance = floorf(gp.second.position.Distance(&newPos));

                        if (gp.second.position.Distance(&newPos) > 1.0f)
                        {
                            translationVec = snapper * distance * sign;
                            Vector3 snappedPos = gp.second.position + translationVec;
                            gp.first->transform->SetPosition(snappedPos);

                            for (int i = 1; i < (int)myParent->GetAttachedObjects().size(); ++i)
                            {
                                auto& childs = myParent->GetAttachedObjects().at(i);

                                Vector3 snappedChildPos = childs.second.position + translationVec;

                                childs.first->transform->SetPosition(snappedChildPos);
                            }
                        }
                        else
                        {
                            gp.first->transform->SetPosition(gp.second.position);

                            for (int i = 1; i < (int)myParent->GetAttachedObjects().size(); ++i)
                            {
                                auto& childs = myParent->GetAttachedObjects().at(i);

                                childs.first->transform->SetPosition(childs.second.position);
                            }
                        }
                    }
					
				}
				else
				{
                    if (mAxis == BASE)
                    {
                        Vector3 worldUp(0.0f, 1.0f, 0.0f);

                        Plane transPlane;
                        transPlane.Normal = worldUp;
                        transPlane.Pos = gameObject->transform->GetGlobalPosition();

                        Vector3 translation_intersect_pt;
                        //Vector3 translation_initial_intersect_pt;

                        Math::Ray3DToPlane(mouse_ray3, transPlane, translation_intersect_pt);
                        //Math::Ray3DToPlane(initial_mouse_ray3, transPlane, translation_initial_intersect_pt);

                        gp.first->transform->SetPosition(translation_intersect_pt);

                        for (int i = 1; i < (int)myParent->GetAttachedObjects().size(); ++i)
                        {
                            auto& childs = myParent->GetAttachedObjects().at(i);
                            Vector3 childPos = childs.second.position - gobj_delta_pt;
                            childs.first->transform->SetPosition(childPos);
                        }

                    }
                    else
                    {
                        gp.first->transform->SetPosition(newPos);

                        for (int i = 1; i < (int)myParent->GetAttachedObjects().size(); ++i)
                        {
                            auto& childs = myParent->GetAttachedObjects().at(i);
                            Vector3 childPos = childs.second.position - gobj_delta_pt;
                            childs.first->transform->SetPosition(childPos);
                        }
                    }
				}

				

			}
			break;
			case ROTATE:
			{
				//form arbitrary plane in local space of object
				Vector3 p1norm = gameObject->transform->GetUpVector();
				Plane p1;
				p1.Normal = p1norm; // MUST BE p1NORMAL AND UP VECTOR
				p1.Pos = gameObject->transform->GetGlobalPosition();

				//intersect with plane
				Vector3 vray1 = EditorSubsystem::getInstance()->EditorCamera()->ScreenToWorldPosition(Input::GetMouseX(true), Input::GetMouseY(true));
				Ray3 mray;
				mray.dir = mouse_ray.Normalized();
				mray.pos = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition();

				Vector3 initray1 = EditorSubsystem::getInstance()->EditorCamera()->ScreenToWorldPosition((int)initialDragPoint.x, (int)initialDragPoint.y);
				Ray3 initray;
				initray.dir = initial_mouse_ray.Normalized();
				initray.pos = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition();

				Vector3 dragStart;
				Vector3 dragEnd;

				Math::Ray3DToPlane(mray, p1, dragEnd);
				Math::Ray3DToPlane(initray, p1, dragStart);
			
				Renderer::getInstance()->DrawLinePerma(p1.Pos, dragStart, COLOR::GREY);
				Renderer::getInstance()->DrawLinePerma(p1.Pos, dragEnd, COLOR::GREY);

				Vector3 firstVec  = dragStart - p1.Pos;
				Vector3 secondVec = dragEnd - p1.Pos;

				Vector3 sign = firstVec.Cross(secondVec);
				float dot = ArcCos(secondVec.Normalized().Dot(firstVec.Normalized()));
				float test = sign.Dot(p1norm);

                if (test < 0.0f)
                {
                  dot = Math::cPi + Math::cPi - dot;
                  //LogVerbose("NEGATIVE");
                }
                dot = 2.0f * Math::cPi - dot;

                dot = Math::RadToDeg(dot);
                int snapCount = (int)dot / 15;

                if (Input::GetHeld(0, "SnapShift"))
                {
                  if (snapCount == 0)
                    dot = 0;
                  else
                    dot = 15.0f * snapCount;
                  //LogVerbose(std::to_string(snapCount));
                }

                for (auto& gp : myParent->GetAttachedObjects())
                {
                  if (mAxis == X)
                  {
                    EulerAngles ea = EulerAngles(gp.second.rotation);
                    Quaternion q = ToQuaternion(Vector3::cXAxis, DegToRad(dot));
                    q *= ToQuaternion(EulerAnglesRadian(ea));
                    gp.first->transform->SetRotation(q);
                  }
                  if (mAxis == Y)
                  {
                    EulerAngles ea = EulerAngles(gp.second.rotation);
                    Quaternion q = ToQuaternion(Vector3::cYAxis, DegToRad(dot));
                    q *= ToQuaternion(EulerAnglesRadian(ea));
                    gp.first->transform->SetRotation(q);
                  }
                  if (mAxis == Z)
                  {
                    EulerAngles ea = EulerAngles(gp.second.rotation);
                    Quaternion q = ToQuaternion(Vector3::cZAxis, DegToRad(dot));
                    q *= ToQuaternion(EulerAnglesRadian(ea));
                    gp.first->transform->SetRotation(q);
                  }
                }

			} break;
			case SCALE:
			{

                auto& gp = myParent->GetAttachedObjects().at(0);
                Vector3 firstVec = temp_pt - gp.second.position;
                Vector3 secondVec = new_pt - gp.second.position;

                float sign = 1.0f;

                //Vector3 translationVec = snapper * distance * sign;
                if (firstVec.Dot(secondVec) < 0)
                    sign *= -1.0f;

				for (auto& gp : myParent->GetAttachedObjects())
				{
					if (mAxis == X)
					{
						float snapDist = new_pt.x - temp_pt.x;

                        if (abs(translation_axis.Dot(Vector3::cZAxis)) == 1.0f)
                            snapDist = new_pt.z - temp_pt.z;

                        //float snapDist = new_pt.x - temp_pt.x;
                        //temp_pt.
                        if (Input::GetHeld(0, "SnapShift"))
                        {
                            float scaleSnapper = snapDist*sign;

                            snapDist = floorf(snapDist*sign);

                            if (abs(scaleSnapper - snapDist) > 0.5f)
                                snapDist += 0.5f;
                        }

                        //LogVerbose(secondVec - firstVec);
						//if (new_pt != new_pt.cZero && temp_pt != temp_pt.cZero)
                            gp.first->transform->scale = gp.second.scale + Vector3(snapDist, 0, 0);
					}
					if (mAxis == Y)
					{
						float snapDist = new_pt.y - temp_pt.y;

                        if (Input::GetHeld(0, "SnapShift"))
                        {
                            float scaleSnapper = snapDist*sign;

                            snapDist = floorf(snapDist*sign);

                            if (abs(scaleSnapper - snapDist) > 0.5f)
                                snapDist += 0.5f;
                        }

						//if (new_pt != new_pt.cZero && temp_pt != temp_pt.cZero)
                            gp.first->transform->scale = gp.second.scale + Vector3(0, snapDist, 0);
					}
					if (mAxis == Z)
                    {
                        float snapDist = new_pt.z - temp_pt.z;

                        if (abs(translation_axis.Dot(Vector3::cXAxis)) == 1.0f)
                            snapDist = new_pt.x - temp_pt.x;
                        if (Input::GetHeld(0, "SnapShift"))
                        {
                            float scaleSnapper = snapDist*sign;

                            snapDist = floorf(snapDist*sign);

                            if (abs(scaleSnapper - snapDist) > 0.5f)
                                snapDist += 0.5f;
                        }

						//if (new_pt != new_pt.cZero && temp_pt != temp_pt.cZero)
                            gp.first->transform->scale = gp.second.scale + Vector3(0, 0, snapDist);
					}
                    if (mAxis == BASE)
                    {
                        //Vector3 objPos = gameObject->transform->GetGlobalPosition();
                        Vector3 normal = (EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition() - gameObject->transform->GetGlobalPosition()).Normalized();
                        Plane scalePlane;
                        scalePlane.Normal = normal;
                        scalePlane.Pos = gameObject->transform->GetGlobalPosition();
                        
                        Vector3 scaleFinalPt;
                        Vector3 scaleInitPt;

                        Math::Ray3DToPlane(mouse_ray3, scalePlane, scaleFinalPt);
                        Math::Ray3DToPlane(initial_mouse_ray3, scalePlane, scaleInitPt);

                        Vector3 upVec = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetUpVector();
                        Vector3 rightVec = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetRightVector();
                        Vector3 diagVec = (upVec + rightVec).Normalized();

                        float signage = 1.0f;

                        if (scaleFinalPt.Dot(diagVec) - scaleInitPt.Dot(diagVec) < 0)
                            signage *= -1.0f;
                        //planeUp.ProjectToPlane(normal);

                        float scaleAmt = signage*scaleFinalPt.Distance(&scaleInitPt);
                        //float snapDist;

                        if (Input::GetHeld(0, "SnapShift"))
                        {
                            float scaleSnapper = scaleAmt*signage;

                            scaleAmt = floorf(scaleAmt);

                            if (abs(scaleSnapper - scaleAmt) > 0.5f)
                                scaleAmt += 0.5f;
                        }
                        

                        gp.first->transform->scale = gp.second.scale + Vector3(scaleAmt, scaleAmt, scaleAmt);
                        
                    }
				}

			} break;
			}
		}
	}
	else
	{
		offsetPoint.ZeroOut();

		//coloration
		if (lastFrameMouseOver)
			lastFrameMouseOver = false;
		else
		{
			ColorNode* c = reinterpret_cast<ColorNode*>(myRenderer->GetMaterial()->GetAttribute<ColorNode>());
			if (c)
				c->SetColor(originalColor);
		}

		//do uniform scaling based on distance
		float distance = EditorSubsystem::getInstance()->EditorCamera()->gameObject->transform->GetGlobalPosition().Distance(&this->gameObject->transform->position());
		float distscalefactor = Math::Clamp(distance / 15.0f, 0.01f, 1000.0f);

        if (mAxis == BASE)
          distscalefactor *= 0.3f;

        gameObject->transform->SetScale(distscalefactor, distscalefactor, distscalefactor);
	}


	//switch to rotation widgets only when not moving
	if (Input::GetHeld(0, "MouseRClick") == 0.0f) 
	{

		if (Input::GetTriggered(0, "WidgetRotate") == 1.0f)
		{
			isGlobal = false;
			hasSwapped = false;
			mMode = ROTATE;

            if (gameObject->GetName() == "BASE")
                myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("Sphere.bean"));
            else
                myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("widget_rotate.bean"));
			Rescale(0);
		}
		if (Input::GetTriggered(0, "WidgetScale") == 1.0f)
		{
			isGlobal = false;
			hasSwapped = false;
			mMode = SCALE;

            if (gameObject->GetName() == "BASE")
                myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("Sphere.bean"));
            else
                myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("widget_scale.bean"));
			Rescale(0);
		}
		if (Input::GetTriggered(0, "WidgetTranslate") == 1.0f && Input::GetHeld(0, "MouseRClick") == 0.0f)
		{
			isGlobal = false;
			hasSwapped = false;
			mMode = TRANSLATE;

            if (gameObject->GetName() == "BASE")
                myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("Sphere.bean"));
            else
                myRenderer->SetMesh(AssetManager::getInstance()->GetAsset("widget_translate.bean"));
			Rescale(0);
		}
	}
}

void Widget_TransformAxis::OnFree()
{
	CWidget::OnFree();
}

void Widget_TransformAxis::OnMouseEnter(CCamera* cam)
{
	ColorNode* c = reinterpret_cast<ColorNode*>(myRenderer->GetMaterial()->GetAttribute<ColorNode>());
	c->SetColor(draggingColor);
	lastFrameMouseOver = true;
}

void Widget_TransformAxis::OnMouseClick(CCamera* cam)
{

	initialDragPoint.x = (float)Input::GetMouseX(true);
	initialDragPoint.y = (float)Input::GetMouseY(true);

	if (draggingLocked)
		return;
	isDragging = true;
	draggingLocked = true;
	ColorNode* c = reinterpret_cast<ColorNode*>(myRenderer->GetMaterial()->GetAttribute<ColorNode>());
	c->SetColor(draggingColor);
	cameraDragging = cam;

	for (auto& gp : myParent->GetAttachedObjects())
	{
		GameObject* g = gp.first;
		CWidget_Transform::GameObjectTransformPack p;
		p.position = g->transform->GetGlobalPosition();
		p.rotation = g->transform->GetRotation();
        p.scale = g->transform->GetScale();
		gp.second = p;
	}
	//ToggleWidgetSpace();


	//initialDragPoint.x = Input::GetMouseX(true);
	//initialDragPoint.y = Input::GetMouseY(true);
}

Widget_TransformAxis::Widget_TransformAxis(WidgetAxis axis, WidgetMode mode, CWidget_Transform* parent) : mAxis(axis), mMode(mode), myParent(parent)
{
}

//Must support g == 0 as a valid c all
void Widget_TransformAxis::Rescale(GameObject* g)
{
	gameObject->transform->rotation.Angles = Vector3::cZero;
	Vector3 l;

	if (!myParent->GetAttachedObjects().empty())
	{
		switch (mAxis)
		{
		case X:
			originalColor = Color(1.0f, 0.0f, 0.0f);
			parentRight = myParent->GetAttachedObjects()[0].first->transform->GetRightVector();
			if (mMode == ROTATE)
			{

                ResetXRotationWidget();
				
			}
			else {
				if (mSpace == LOCAL)
					l = myParent->GetAttachedObjects()[0].first->transform->GetRightVector();
				else
					l = Vector3::cXAxis;
				l += gameObject->transform->GetGlobalPosition();
				gameObject->transform->LookAt(&l);
			}

			
			break;
		case Y:
			originalColor = Color(0.0f, 1.0f, 0.0f);
			parentUp = myParent->GetAttachedObjects()[0].first->transform->GetUpVector();
			if (mMode == ROTATE)
			{

                ResetYRotationWidget();
				

			}
			else {
				if (mSpace == LOCAL) {
					l = myParent->GetAttachedObjects()[0].first->transform->GetUpVector();
				}
				else
					l = Vector3::cYAxis;
				l += gameObject->transform->GetGlobalPosition();
				l.x -= 0.01f;
				l.y -= 0.01f;
				l.z -= 0.01f;
				gameObject->transform->LookAt(&l);
			}
			
			break;
		case Z:
			originalColor = Color(0.0f, 0.0f, 1.0f);
			parentLookAt = myParent->GetAttachedObjects()[0].first->transform->GetForwardVector();
			if (mMode == ROTATE)
			{
                ResetZRotationWidget();
			}
			else {
				if (mSpace == LOCAL)
					l = myParent->GetAttachedObjects()[0].first->transform->GetForwardVector();
				else
					l = Vector3::cZAxis;
				l += gameObject->transform->GetGlobalPosition();
				gameObject->transform->LookAt(&l);
			}
			
			break;
        case BASE:
            originalColor = Color(0.5f, 0.5f, 0.5f);
            break;
		default:
			Log("No axis specified for TransformAxis Widget");
			break;

		}
		oldPos = myParent->GetAttachedObjects()[0].first->transform->GetGlobalPosition();
		
	}
	
	//resend the color
	CDiffuseRenderer* cmesh = gameObject->GetComponent<CDiffuseRenderer>();
	//originalColor = Color(1,1,1);
	void* cv = &originalColor;
	//cmesh->GetMaterial()->SendMessageW("ChangeColor", cv);
}

void Widget_TransformAxis::SetSpace(WidgetSpace space)
{
	mSpace = space;
	Rescale(0);
}

void Widget_TransformAxis::ToggleWidgetSpace(void)
{
	if (Input::GetTriggered(0, "LocalToGlobal") == 1.0f)
	{
		isGlobal = !isGlobal;
		hasSwapped = false;
	}

	if (isGlobal)
	{
		if (!hasSwapped)
		{
			if (mAxis == X)
			{
				if (mMode == TRANSLATE || mMode == SCALE)
				{
					Vector3 pos = gameObject->transform->GetPosition();
					pos.x += 1.0f;
					gameObject->transform->LookAt(&pos);
				}
				else
				{
					ResetXRotationWidget();
				}
			}
			if (mAxis == Y)
			{
				if (mMode == TRANSLATE || mMode == SCALE)
				{
					Vector3 pos = gameObject->transform->GetPosition();
					pos.y += 1.0f;
					gameObject->transform->LookAt(&pos);
				}
				else
				{
					ResetYRotationWidget();
				}
			}
			if (mAxis == Z)
			{
				if (mMode == TRANSLATE || mMode == SCALE)
				{
					Vector3 pos = gameObject->transform->GetPosition();
					pos.z += 1.0f;
					gameObject->transform->LookAt(&pos);
				}
				else
				{
					ResetZRotationWidget();
				}
			}
		}

		hasSwapped = true;

	}
	else
	{
		if (!hasSwapped)
		{

			if (!myParent->GetAttachedObjects().empty())
			{
				
				if (mAxis == X)
				{
					if (mMode == TRANSLATE)
					{
						oldPos = myParent->GetAttachedObjects()[0].first->transform->GetGlobalPosition();
						parentRight = myParent->GetAttachedObjects()[0].first->transform->GetRightVector();
						gameObject->transform->LookAt(&(oldPos + parentRight));
					}
					//gameObject->transform->rotation.Angles.x = parentLookAt.x;
					else if (mMode == ROTATE)
					{


						Vec3 localRot = myParent->GetAttachedObjects()[0].first->transform->rotation.Angles;

						
						ResetXRotationWidget();
					}
				}
				if (mAxis == Y)
				{
					if (mMode == TRANSLATE)
					{
						oldPos = myParent->GetAttachedObjects()[0].first->transform->GetGlobalPosition();
						parentUp = myParent->GetAttachedObjects()[0].first->transform->GetUpVector();
						gameObject->transform->LookAt(&(oldPos + parentUp));
					}
					//gameObject->transform->rotation.Angles.y = parentLookAt.y;
					else if (mMode == ROTATE)
					{

						Vec3 localRot = myParent->GetAttachedObjects()[0].first->transform->rotation.Angles;


						ResetYRotationWidget();

					}
				}
				if (mAxis == Z)
				{
					if (mMode == TRANSLATE)
					{
						oldPos = myParent->GetAttachedObjects()[0].first->transform->GetGlobalPosition();
						parentLookAt = myParent->GetAttachedObjects()[0].first->transform->GetForwardVector();
						gameObject->transform->LookAt(&(oldPos + parentLookAt));
					}
					//gameObject->transform->rotation.Angles.z = parentLookAt.z;
					else if (mMode == ROTATE)
					{
						//Vec3 localRot = myParent->GetAttachedObjects()[0].first->transform->GetUpVector().Cross(myParent->GetAttachedObjects()[0].first->transform->GetRightVector());
						//
						//gameObject->transform->rotation.Angles = localRot.Normalized();
						Vec3 localRot = myParent->GetAttachedObjects()[0].first->transform->rotation.Angles;

						ResetZRotationWidget();
					}
				}
			}
		}
		hasSwapped = true;
	}
}

void Widget_TransformAxis::ResetXRotationWidget(void)
{
    //gameObject->transform->Rotate(Vector3)
	gameObject->transform->rotation.Angles = Vector3(0.0f);
	gameObject->transform->rotation.Angles.z = 90;
}

void Widget_TransformAxis::ResetYRotationWidget(void)
{
	gameObject->transform->rotation.Angles.x = 0.0f;
	gameObject->transform->rotation.Angles.z = 0.0f;
}

void Widget_TransformAxis::ResetZRotationWidget(void)
{
	gameObject->transform->rotation.Angles = Vector3(0.0f);
	gameObject->transform->rotation.Angles.x = 90;
}

meta_define(Widget_TransformAxis);

//comment
