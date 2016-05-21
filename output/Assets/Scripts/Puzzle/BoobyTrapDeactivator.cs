using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class BoobyTrapDeactivator : StealthInteractableObject
{
    GameObject boobyControllerObj;
    BoobyTrapDeactivatorController controllerScript;

    float mInitZ;
    float mZMoved;
    const float mAmountToMoveZ = 0.20f;
    const float mTimeToMove = 0.6f;
    // 0 - not moving, 1 - pushing in, 2 - pushing out, 3 - pushing in to pushing out, 4 - out to in
    int mStateOfPushedIn = 0;
    bool mIsPushedIn = false;
    bool isTrapUnlocked = false;
    bool isActivated = false; // If trap is triggered, and these buttons are clickable

    CMeshRenderer buttonMesh;
    Color buttonColor;

    CSound sound;

    int keyCode;
    public int KeyCode
    {
        get { return keyCode; }
        set { keyCode = value; }
    }

    void SetPushIn()
    {
        Color pushedColor = buttonColor.Mul(0.15f);
        SetPushIn(pushedColor);
    }
    void SetPushIn(Color newButtonColor)
    {
        buttonMesh.mEmissiveColor = newButtonColor;
        mZMoved = 0.0f;
        mStateOfPushedIn = mStateOfPushedIn == 2 ? 4 : 1;
        mIsPushedIn = true;
    }
    void SetPushOut()
    {
        SetPushOut(buttonColor);
    }
    void SetPushOut(Color newButtonColor)
    {
        buttonMesh.mEmissiveColor = newButtonColor;
        mZMoved = 0.0f;
        mStateOfPushedIn = mStateOfPushedIn == 1 ? 3 : 2;
        mIsPushedIn = false;
    }
    void UpdatePushing()
    {
        float dt = FrameController.DT();

        if (mStateOfPushedIn != 0)
        {
            mZMoved += mAmountToMoveZ / mTimeToMove * dt;
            if (mZMoved >= mAmountToMoveZ)
            {
                mZMoved = mAmountToMoveZ;
            }

            Vector3 currPos = gameObject.transform.GetPosition(CTransform.TransformSpace.LOCAL);
            currPos.Z = mInitZ + mZMoved; // pushing in
            if (mStateOfPushedIn == 2 || mStateOfPushedIn == 4) // pushing out
            {
                currPos.Z = mInitZ + mAmountToMoveZ - mZMoved;
            }
            gameObject.transform.SetPosition(currPos, CTransform.TransformSpace.LOCAL);

            if (mZMoved == mAmountToMoveZ)
            {
                if (mStateOfPushedIn == 3) // If in to out, immediately follow up with out
                    SetPushOut(buttonMesh.mEmissiveColor);
                else if (mStateOfPushedIn == 4) // If out to in, immediately follow up with in
                    SetPushIn(buttonMesh.mEmissiveColor);
                else
                {
                    if (!isTrapUnlocked && buttonMesh.mEmissiveColor.Comp(SharpMocha.RED) )
                        buttonMesh.mEmissiveColor = buttonColor; // Reset back to white when pushed out
                    mStateOfPushedIn = 0;
                }
            }
        }
    }

    public void ActivateButton()
    {
        buttonMesh.setEnabled(true);
        isActivated = true;
    }
    public void Reset()
    {
        mStateOfPushedIn = 0;
        mIsPushedIn = false;
        isTrapUnlocked = false;
        isActivated = false;

        Vector3 currPos = gameObject.transform.GetPosition(CTransform.TransformSpace.LOCAL);
        currPos.Z = mInitZ;
        gameObject.transform.SetPosition(currPos, CTransform.TransformSpace.LOCAL);
        mZMoved = 0.0f;

        buttonMesh = gameObject.RequireComponent<CMeshRenderer>();
        buttonColor = SharpMocha.WHITE;
        buttonMesh.GetMaterial().SetColor(SharpMocha.BLACK);
        buttonMesh.mEmissiveColor = buttonColor;
        buttonMesh.setEnabled(false);
    }
    public override void OnStart()
    {
        base.OnStart();

        mInitZ = gameObject.transform.GetPosition(CTransform.TransformSpace.LOCAL).z;
        Reset();

        boobyControllerObj = GameObject.GetGameObjectByName("TSR_TrapController");//trapControllerName);
        controllerScript = GetScript<BoobyTrapDeactivatorController>(boobyControllerObj);
        controllerScript.allPanels.Add(gameObject); // Add myself to the controller list of buttons

        sound = gameObject.RequireComponent<CSound>();
    }

    public override void OnUpdate()
    {
        UpdatePushing();
    }

    public void SetLockdownState() // Called by controller when time is up
    {
        SetPushIn(SharpMocha.RED);
    }
    public void ChangeState(bool isPassed)
    {
        Color nextClr = isPassed ? SharpMocha.GREEN : SharpMocha.RED;
        isTrapUnlocked = isPassed;
        SetPushOut(nextClr);
    }

    public override void OnStealthPlayerMouseEnter()
    {

    }
    public override void OnStealthPlayerMouseLeave()
    {

    }
    public override void OnStealthPlayerMouseInteract()
    {
        if (!isActivated)
            return;

        if (!isTrapUnlocked && !mIsPushedIn)
        {
            SetPushIn();

            sound.PlayIndependentEvent("PINPAD_INPUT.vente", false, 1);

            controllerScript.CheckSequence(keyCode, gameObject);
            //controllerScript.AddPlayerInput(keyCode);
            //Console.Write(keyCode.ToString());
        }
    }
}
