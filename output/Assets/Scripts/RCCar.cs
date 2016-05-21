using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class RCCar : InteractableObjectBaseClass
{
    CMeshRenderer surveillanceMouseMesh;
    OcuMouse surveillanceMouseScript;

    bool ismouseheld = false;
    public bool isMouseHeld { get { return ismouseheld; }
        set 
        { 
            ismouseheld = value; 
            if (surveillanceMouseMesh != null)
                surveillanceMouseMesh.setEnabled(!value); 
        } 
    }
    Vector2 centerPos;
    Vector2 unitMovement; // centerPos scaled back to [0,1]
    Vector2 quadraticMovement; // The actual amount applied to car

    const float mouseLimitAmt = 0.25f;
    Vector2 mouseLimit = new Vector2(mouseLimitAmt); // limit for how far the mouse can move from the center
    Vector2 mouseDeadZone = new Vector2(mouseLimitAmt / 3.0f, mouseLimitAmt / 2.5f);
    const float speedAmt = 10.0f;
    const float rotSpeedAmt = 1.3f;
    float currYRotation;

    GameObject rcCar;
    RigidBodyComponent rcCarRigidCom;
    CPhysics carPhysics;
    float joystickRotationLimit = MathHelper.ToRadians(45.0f);

    bool foundCar = false;

    public void OnStart()
    {
        surveillanceMouseMesh = Common.GetSurveillancePlayerMouse().RequireComponent<CMeshRenderer>();
        surveillanceMouseScript = GetScript<OcuMouse>(Common.GetSurveillancePlayerMouse());

        rcCar = Common.GetRCCar();
        if (rcCar != null)
            InitCar();
    }

    void InitCar()
    {
        foundCar = true;
        carPhysics = rcCar.RequireComponent<CPhysics>();
        carPhysics.mColliderType = 1;

        currYRotation = rcCar.transform.GetRotation().y;

        rcCarRigidCom = carPhysics.rigidCom;
    }

    public void OnUpdate()
    {
        if (!foundCar)
        {
            rcCar = Common.GetRCCar();
            if (rcCar != null)
                InitCar();
            return;
        }

        if (!isMouseHeld || FrameController.DT() > (1.0f/5.0f)) // When mouse is not held down
        {
            // Reset the 'center' of the mouse position when first clicked on RC Car Controller
            SetMovementAmount(new Vector2(0.0f, 0.0f));

            // Manually slow down the car
            Vector3 currVel = (Vector3)carPhysics.rigidCom.Body().LinearVelocity();
            if (currVel.LengthSquared() > 1.00f)
            {
                currVel *= 0.5f;
                carPhysics.rigidCom.Body().SetLinearVelocity(currVel);
            }
            else
            {
                carPhysics.rigidCom.Body().SetLinearVelocity(Vector3.Zero);
            }
        }
        else
        {
            Vector2 mouseMove = surveillanceMouseScript.mCurrMouseMovement;
            UpdateOnMouseMove(mouseMove.X, mouseMove.Y);

            float dt = FrameController.DT();

            // Ensure some dead zone
            if (Math.Abs(centerPos.Y) > mouseDeadZone.Y)
            {
                Vector3 forward = rcCar.transform.GetForwardVector(); // y mouse means accel or deccel

                Vector3 currVel = forward * quadraticMovement.Y; // Make it a quadratic curve for slow speeds
                currVel.Y = 0.0f;
                carPhysics.rigidCom.Body().SetLinearVelocity(currVel);
                //Logger.Log("vel: " + currVel.ToString());
            }
            if (Math.Abs(centerPos.X) > mouseDeadZone.X)
            {
                currYRotation += MathHelper.ToDegrees(quadraticMovement.X * dt); // rotate by mouse x movement
                if (currYRotation < 0.0f) // Clamp rotation angle to [0, 360]
                    currYRotation += 360.0f;
                else if (currYRotation > 360.0f)
                    currYRotation -= 360.0f;

                rcCar.transform.Rotate(0.0f, currYRotation, 0.0f);
                //Logger.Log("rot angle: " + rcCar.transform.rotation.Angles.y.ToString());
            }
        } // End of if mouse is held down
        
        // isMouseHeld = false; // Set to false again, to be set back to true if OnHeldMouse

        rcCar.transform.Rotate(0.0f, currYRotation, 0.0f); // Prevent it from turning on the stop in idle

        // Update the model joystick's rotation based on unitMovement
        Vector3 currRotationAngle = new Vector3();
        currRotationAngle.X = MathHelper.ToDegrees(unitMovement.Y * joystickRotationLimit); // [-1,1] -> [-limit, limit]
        currRotationAngle.Z = MathHelper.ToDegrees(-unitMovement.X * joystickRotationLimit);
        gameObject.transform.Rotate(currRotationAngle);
        //gameObject.transform.rotation.Angles.x = unitMovement.Y * joystickRotationLimit; // [-1,1] -> [-limit, limit]
        //gameObject.transform.rotation.Angles.z = -unitMovement.X * joystickRotationLimit;

        //snap player to ground if there is one
        Vector3 ray_pos = rcCar.transform.position;
        //ray_pos.Y += 1.0f;
        RayCastResult output;
        output = PhysicEngine.getInstance().RayCast3D_Output(ray_pos, new Vector3(0, -1, 0), 1.0f);
        if (output.hit)
        {
            //we found the ground
            rcCar.transform.SetPositionY(output.intersection.y + 0.35f);

            Vector3 currVel = rcCarRigidCom.Body().LinearVelocity();
            currVel.Y = 0.0f;

            rcCarRigidCom.Body().SetLinearVelocity(currVel);
            //Logger.Log(output.go.GetName());
            //Logger.Log(output.intersection.y.ToString());
        }
    }

    public override void OnTriggered()
    {
        if (!foundCar)
            return;
        isMouseHeld = true;
        currYRotation = rcCar.transform.GetRotation().y;
    }
    public override void OnHeldMouse(float mouseMoveX, float mouseMoveY)
    {
        // isMouseHeld = true;
        // UpdateOnMouseMove(mouseMoveX, mouseMoveY);
    }
    public override void OnMouseOver()
    {
        if (foundCar)
        {
            CFontRenderer sub = GetSubtitleFont();
            if (!sub.isEnabled())
            {
                sub.setEnabled(true);
                sub.mText = "Click Drag to Move RC Car";
            }
        }
    }
    public override void OnMouseOverOff()
    {
        if (!foundCar)
            return;
        GetSubtitleFont().setEnabled(false);
    }

    void UpdateOnMouseMove(float mouseMoveX, float mouseMoveY)
    {
        if (mouseMoveX == 0.0f && mouseMoveY == 0.0f)
            return;

        float newX = MathHelper.Clamp(mouseMoveX + centerPos.X, -mouseLimit.X, mouseLimit.X);
        float newY = MathHelper.Clamp(mouseMoveY + centerPos.Y, -mouseLimit.Y, mouseLimit.Y);
        SetMovementAmount(new Vector2(newX, newY));
    }
    void SetMovementAmount(Vector2 newCenterPos)
    {
        centerPos = newCenterPos;

        unitMovement.X = centerPos.X / mouseLimit.X; // scale back to [0,1]
        unitMovement.Y = centerPos.Y / mouseLimit.Y;

        float signAmtY = (centerPos.Y < 0.0f ? -1.0f : 1.0f);
        float withinDeadAndLimit = 0.0f;
        if (Math.Abs(centerPos.Y) > mouseDeadZone.Y) // Only when past dead zone will it be > 0
        {
            withinDeadAndLimit = signAmtY * (Math.Abs(centerPos.Y) - mouseDeadZone.Y) /
                (mouseLimit.Y - mouseDeadZone.Y); // scale back to [0,1]
        }
        quadraticMovement.Y = signAmtY * withinDeadAndLimit * withinDeadAndLimit * speedAmt;

        float signAmtX = (centerPos.X < 0.0f ? -1.0f : 1.0f);
        withinDeadAndLimit = 0.0f;
        if (Math.Abs(centerPos.X) > mouseDeadZone.X) // Only when past dead zone will it be > 0
        {
            withinDeadAndLimit = signAmtX * (Math.Abs(centerPos.X) - mouseDeadZone.X) /
                (mouseLimit.X - mouseDeadZone.X); // scale back to [0,1]
        }
        quadraticMovement.X = signAmtX * withinDeadAndLimit * withinDeadAndLimit * rotSpeedAmt;
    }
}
