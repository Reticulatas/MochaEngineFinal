using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class OculusPlayer : MochaScript
{
	//StateMachineController smc = new StateMachineController();
	//State idle = new State(smc);
    //State move = new State(smc);
    //State jump = new State(smc);
    //State crouch = new State(smc);
	GameObject PickCam1;
	CCamera oculusCam;
	CCamera gameCam;
	//float oculusWidth = 1280.0f;
	//float oculusHeight = 800.0f;
	float oculusWidth = Engine.getInstance().GetVRWnd().GetWindowWidth();
	float oculusHeight = Engine.getInstance().GetVRWnd().GetWindowHeight();

	public void OnStart()
	{
		//idle.OnEnter = State_Idle_Enter;
		//idle.OnUpdate = State_Idle_Update;
		//idle.OnExit = State_Idle_Exit;
        //
        //jump.OnEnter = State_Jump_Enter;
        //jump.OnUpdate = State_Jump_Update;
        //jump.OnExit = State_Jump_Exit;
        //
        //crouch.OnEnter = State_Crouch_Enter;
        //crouch.OnUpdate = State_Crouch_Update;
        //crouch.OnExit = State_Crouch_Exit;
        //
        //move.OnEnter = State_Move_Enter;
        //move.OnUpdate = State_Move_Update;
        //move.OnExit = State_Move_Exit;
        //
		//smc.SetState(idle);
		//
		
		// Self
		oculusCam = gameObject.RequireComponent<CCamera>();
		
		gameCam = GameObject.GetGameObjectByName("Camera").RequireComponent<CCamera>();
		
		PickCam1 = GameObject.GetGameObjectByName("PickCam1");		 

	    CMeshRenderer mesh = PickCam1.RequireComponent<CMeshRenderer>();
	    mesh.setEnabled(false); 
	}
	
    public void OnCollisionEnter(CollisionData data)
    {

    }
  
    
	public void OnUpdate()
	{
        //smc.Update();
        
        float mouseX = Input.GetMouseX(false);
        float mouseY = Input.GetMouseY(false);
        Vector3 pos;
        
        if(mouseX > oculusWidth * .5f)
        {
        	//mouseX -= oculusWidth * .5f;  
            //mouseX *= 2.0f;
            pos = OculusVR.getInstance().GetEyePos(1);             
        }
        else
        {
        	pos = OculusVR.getInstance().GetEyePos(0);
       		//mouseX *= 2.0f; 
        }
        
        
          
        //GameObject gobj = PhysicEngine.getInstance().RayCast3DWithTag(pos,
        //                  gameObject.transform.GetForwardVector(), oculusCam, (int)mouseX,
        //                  (int)mouseY, 10000.0f, "Raycast");
                          
        GameObject gobj = PhysicEngine.getInstance().RayCast3D(pos,
                          gameObject.transform.GetForwardVector(), gameCam, (int)mouseX,
                          (int)mouseY, 10000.0f);
                              
        if(Input.GetTriggered(0, "MouseLClick") == 1.0f)
        {       
          
        
        
        
         
                              
          if (gobj != null)
          {    
          		 CMeshRenderer mesh = gobj.RequireComponent<CMeshRenderer>();
          		 mesh.setEnabled(!mesh.isEnabled()); 
          }
       }
	}
}
