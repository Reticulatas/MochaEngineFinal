using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class Mouse : MochaScript
{
#if ZERO
    GameObject PickCam1;
    GameObject PickCam2;
    GameObject PickCam3;
    GameObject PickCam4;
    GameObject PickCam5;
    GameObject PickCam6;
    GameObject DoorCode1;
    GameObject DoorCode2;
    GameObject Plane;
    PaperViewer paperViewer;
    CMeshRenderer MapScreen;

    CCamera SurvCam;
    CCamera ocuCam;

    GameObject Cursor;
    Vector3 zAxis;
    Vector3 MouseVec;
    float MouseLength;
#endif

    GameObject playerCamera;


    public void OnStart()
    {
        playerCamera = Common.GetStealthPlayerCamera();
#if ZERO
        Cursor = GameObject.GetGameObjectByName("Mouse");
        //Cursor.SetParent(gameObject);
        Cursor.transform.SetPosition(0, 0, 1.7f, CTransform.TransformSpace.LOCAL);

        zAxis = new Vector3(0, 0, 1);

        MapScreen = GameObject.GetGameObjectByName("MapScreen").RequireComponent<CMeshRenderer>();

        Plane = GameObject.GetGameObjectByName("Plane");
        paperViewer = GetScript<PaperViewer>(Plane);

        SurvCam = GameObject.GetGameObjectByName("Cam_Surv").RequireComponent<CCamera>();
        ocuCam = GameObject.GetGameObjectByName("Cam_Oculus").RequireComponent<CCamera>();

        DoorCode1 = GameObject.GetGameObjectByName("DoorCodes1");
        DoorCode2 = GameObject.GetGameObjectByName("DoorCodes2");

        PickCam1 = GameObject.GetGameObjectByName("PickCam1");
        PickCam2 = GameObject.GetGameObjectByName("PickCam2");
        PickCam3 = GameObject.GetGameObjectByName("PickCam3");
        PickCam4 = GameObject.GetGameObjectByName("PickCam4");
        PickCam5 = GameObject.GetGameObjectByName("PickCam5");
        PickCam6 = GameObject.GetGameObjectByName("PickCam6");

        PickCam1.RequireComponent<CMeshRenderer>().setEnabled(false);
        PickCam2.RequireComponent<CMeshRenderer>().setEnabled(false);
        PickCam3.RequireComponent<CMeshRenderer>().setEnabled(false);
        PickCam4.RequireComponent<CMeshRenderer>().setEnabled(false);
        PickCam5.RequireComponent<CMeshRenderer>().setEnabled(false);
        PickCam6.RequireComponent<CMeshRenderer>().setEnabled(false);

        Input.SetMouseLock(true);
        Input.SetMouseVisible(false);
#endif
    }

    public GameObject RayCastWithTag(String tag)
    {
        GameObject gobj = PhysicEngine.getInstance().RayCast3DWithTag(
            Common.GetStealthPlayerCamera().transform.position, Common.GetStealthPlayerCamera().transform.GetForwardVector(), 10000.0f, tag);
        return gobj;
    }

    public void OnUpdate()
    {
        // Vector3 camPos = playerCamera.transform.position;
        // Vector3 camDir = (Vector3)playerCamera.transform.GetForwardVector();
        // gameObject.transform.SetPosition(camDir + camPos);
        // gameObject.transform.LookAt(camPos); // have the cursor look back at camera

#if ZERO
        MouseVec = Cursor.transform.GetGlobalPosition().Sub(ocuCam.gameObject.transform.position);
        MouseVec.Normalize();

        gameObject.transform.SetPositionX(MMath.Clamp(gameObject.transform.position.x, -1.54f, 2.5f));
        gameObject.transform.SetPositionY(MMath.Clamp(gameObject.transform.position.y, 0.89f, 2.5f));


        //Renderer.getInstance().DrawLine(MouseVec.Add(gameObject.transform.position), gameObject.transform.position, col);

        //Cursor.LookAt(gameObject);

        //float angle = zAxis.Dot(MouseVec) / MouseLength;

        //if(Math.Abs(Math.Acos(angle)) <= .3f)
        //{
        gameObject.transform.Translate(Input.GetMouseDeltaX() * FrameController.DT() * .025f,
        -Input.GetMouseDeltaY() * FrameController.DT() * .025f, 0);
        //}

        GameObject gobj = PhysicEngine.getInstance().RayCast3DWithTag(ocuCam.gameObject.transform.position,
                                                          MouseVec, 10000.0f, "Raycast");

		if (Input.GetHeld(0, "MouseRClick") == 1.0f)
		{
			if (gobj != null)
				Invoke(gobj, "OnMouseHeld", false);
		}
        
        if (Input.GetReleased(0, "MouseLClick") == 1.0f)
        {
            //un click paper if currently there is one up 
            if (PaperViewer.isPaperSelected)
            {
                paperViewer.TransitionDown();
                return;
            }

            if (gobj != null)
            {
                if (gobj.GetID() == PickCam1.GetID())
                {
                    MapScreen.mMaterial.SetTexture("map_lvl1_cam1.png");

                    SurvCam.FoVmodifier(1.5f);
                    SurvCam.gameObject.transform.SetPosition(-4.58f, 3.38f, -5.06f);
                    SurvCam.gameObject.transform.rotation.Angles.x = .62f;
                    SurvCam.gameObject.transform.rotation.Angles.y = .85f;
                    SurvCam.gameObject.transform.rotation.Angles.z = -.47f;
                }

                else if (gobj.GetID() == PickCam2.GetID())
                {
                    MapScreen.mMaterial.SetTexture("map_lvl1_cam2.png");

                    SurvCam.FoVmodifier(1.7f);
                    SurvCam.gameObject.transform.SetPosition(-17.05f, 7.48f, 47.15f);
                    SurvCam.gameObject.transform.rotation.Angles.x = .48f;
                    SurvCam.gameObject.transform.rotation.Angles.y = .80f;
                    SurvCam.gameObject.transform.rotation.Angles.z = -.31f;
                }

                else if (gobj.GetID() == PickCam3.GetID())
                {
                    MapScreen.mMaterial.SetTexture("map_lvl1_cam3.png");

                    SurvCam.FoVmodifier(1.5f);
                    SurvCam.gameObject.transform.SetPosition(-3.65f, 5.94f, 88.70f);
                    SurvCam.gameObject.transform.rotation.Angles.x = .54f;
                    SurvCam.gameObject.transform.rotation.Angles.y = .58f;
                    SurvCam.gameObject.transform.rotation.Angles.z = -.27f;
                }

                else if (gobj.GetID() == PickCam4.GetID())
                {
                    MapScreen.mMaterial.SetTexture("map_lvl2_cam1.png");

                    SurvCam.FoVmodifier(1.7f);
                    SurvCam.gameObject.transform.SetPosition(-31.73f, 45.63f, 144.12f);
                    SurvCam.gameObject.transform.rotation.Angles.x = -.53f;
                    SurvCam.gameObject.transform.rotation.Angles.y = 2.49f;
                    SurvCam.gameObject.transform.rotation.Angles.z = .24f;
                }

                else if (gobj.GetID() == PickCam5.GetID())
                {
                    MapScreen.mMaterial.SetTexture("map_lvl2_cam3.png");

                    SurvCam.FoVmodifier(1.4f);
                    SurvCam.gameObject.transform.SetPosition(-42.57f, 41.39f, 159.66f);
                    SurvCam.gameObject.transform.rotation.Angles.x = -.46f;
                    SurvCam.gameObject.transform.rotation.Angles.y = 2.59f;
                    SurvCam.gameObject.transform.rotation.Angles.z = .36f;
                }

                else if (gobj.GetID() == PickCam6.GetID())
                {
                    MapScreen.mMaterial.SetTexture("map_lvl2_cam2.png");

                    SurvCam.FoVmodifier(1.2f);
                    SurvCam.gameObject.transform.SetPosition(2.48f, 41.47f, 159.75f);
                    SurvCam.gameObject.transform.rotation.Angles.x = -.66f;
                    SurvCam.gameObject.transform.rotation.Angles.y = 4.03f;
                    SurvCam.gameObject.transform.rotation.Angles.z = -.51f;
                }

                else
                {
                    //set paper if it's a paper
                    if (gobj.GetFlag("Paper"))
                    {
                        GetScript<Paper>(gobj).Set();
                    }
                    else
                    {
                    	Invoke(gobj, "OnClicked", false);
                    }
                }
            }
        }
#endif
    }
}
