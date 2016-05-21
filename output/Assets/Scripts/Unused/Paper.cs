using Microsoft.Xna.Framework;
using System;
using MochaInterface;

public class Paper : InteractableObjectBaseClass
{
    bool isHeldUp;
    public bool GetIsHeldUp() { return isHeldUp; }

    Vector3 startRotation;
    Vector3 startPos;
    GameObject ocuCam;

    public void OnStart()
    {
        startPos = gameObject.transform.position;
    	startRotation = gameObject.transform.rotation.Angles;

        ocuCam = Common.GetSurveillancePlayerCam();
    }

    public void OnUpdate()
    {
        if (isHeldUp)
        {
            Vector3 camPos = ocuCam.transform.position;
            Vector3 camDir = ocuCam.transform.GetForwardVector();
            gameObject.transform.SetPosition(camPos + camDir * 1.0f);
            gameObject.transform.LookAt(camPos);
        }
    }

    public override void OnTriggered()
    {
        isHeldUp = !isHeldUp;
        if (!isHeldUp) // put back down
        {
            gameObject.transform.SetPosition(startPos);
            gameObject.transform.rotation.Angles = startRotation;
        }
    }
    public override void OnMouseOver()
    {
        CFontRenderer sub = GetSubtitleFont();
        if (!sub.isEnabled())
        {
            sub.setEnabled(true);
            sub.mText = "Click to Read Text";
        }
    }
    public override void OnMouseOverOff()
    {
        GetSubtitleFont().setEnabled(false);
    }

#if ZERO
    static bool paperBeingHeld;
    CZone zone;
    Vector3 velocity;
    Vector3 acquiredVelocity;
    PaperViewer paperViewer;
    public string PaperTexture { get; set; }
    bool _isbeingheld;
    bool isBeingHeld { get { return _isbeingheld; } set { _isbeingheld = value; paperBeingHeld = value; } }
    float velocityAccumulator; //for residual velocity
    float initialY, initialZ;
    float changeY;      //slope of forward vector

    public void OnStart()
    {
    	velocity = new Vector3();
        acquiredVelocity = new Vector3();
        GameObject Plane = GameObject.GetGameObjectByName("Plane");
        paperViewer = GetScript<PaperViewer>(Plane);
        gameObject.SetFlag("Paper");
        zone = GameObject.GetGameObjectByName("PaperZone").RequireComponent<CZone>();
        initialY = gameObject.transform.position.y;
        initialZ = gameObject.transform.position.z;
        Vector3 up = gameObject.transform.GetUpVector().Negate();
        changeY = up.Y / up.Z;
    }

    public void OnUpdate()
    {
    	zone.ClampObjectToBounds(gameObject);
    	
    	//paper shuffle
        if (isBeingHeld && Input.GetHeld(0, "MouseRClick") == 1.0f)
    	{
            //reset acquired
            acquiredVelocity = new Vector3(0.0f);
    		acquiredVelocity.X += Input.GetMouseDeltaX() * FrameController.DT()*0.02f;
            acquiredVelocity.Z += -Input.GetMouseDeltaY() * FrameController.DT() * 0.02f;

            velocityAccumulator = 0;
            velocity = acquiredVelocity;
    	}
    	else
    	{
            velocityAccumulator += FrameController.DT();
            velocityAccumulator = MMath.Clamp01(velocityAccumulator);
            velocity.X = MMath.Lerp(acquiredVelocity.X, 0, velocityAccumulator);
            velocity.Z = MMath.Lerp(acquiredVelocity.Z, 0, velocityAccumulator);
    	}
    	  	
    	gameObject.transform.Translate(velocity);
        gameObject.transform.SetPositionY(changeY * (gameObject.transform.position.z - initialZ) + initialY);
    	
    	//reset held
        if (isBeingHeld)
        {
            if (Input.GetReleased(0, "MouseRClick") == 1.0f)
            {
                isBeingHeld = false;
            }
        }
    }

    public void Set()
    {
        if (isBeingHeld)
            return;

        paperViewer.TransitionUp(this);
    }
    
    
    //clicked by oculus camera
    public void OnClicked()
    {
        Set();
    }
    
    public void OnMouseHeld()
    {
        if (!paperBeingHeld)
    	    isBeingHeld = true;
    }
#endif
}