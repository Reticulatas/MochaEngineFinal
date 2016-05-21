using Microsoft.Xna.Framework;
using System;
using MochaInterface;

public class PaperViewer : MochaScript
{
#if ZERO
    public static bool isPaperSelected;

    StateMachineController smc;
    State idle;
    State paperDisplayed;
    State transitionUp;
    State transitionDown;
    Vector3 upPos;              //const
    Vector3 upRot;          //const
    Vector3 downPos;            //set at transition up begin
    Vector3 downRot;

    CMeshRenderer paperMesh;

    public void OnStart()
    {
        smc = new StateMachineController();
        idle = new State(smc);
        paperDisplayed = new State(smc, State_PaperDisplayed_Enter, null, null);
        transitionUp = new State(smc, null, State_TransitionUp_Update, null, 1.0f, paperDisplayed);
        transitionDown = new State(smc, null, State_TransitionDown_Update, null, 1.0f, idle);

        downPos = new Vector3();
        downRot = new Vector3();
        upPos = gameObject.transform.position;
        upRot = gameObject.transform.rotation.Angles;

        paperMesh = gameObject.RequireComponent<CMeshRenderer>();
        paperMesh.setEnabled(false);

        smc.SetState(idle);
    }

    public void OnUpdate()
    {
        smc.Update();
    }

    void State_Idle_Enter()
    {
        paperMesh.setEnabled(false);
        isPaperSelected = false;
    }

    void State_PaperDisplayed_Enter()
    {
    }

    void State_TransitionUp_Update()
    {
        float t = transitionUp.GetPercentageTime();

        gameObject.transform.SetPosition(MMath.CerpVector(downPos, upPos, t));
        gameObject.transform.rotation.Angles = MMath.CerpVector(downRot, upRot, t);
    }
    void State_TransitionDown_Update()
    {
        float t = transitionDown.GetPercentageTime();

        gameObject.transform.SetPosition(MMath.CerpVector(upPos, downPos, t));
        gameObject.transform.rotation.Angles = MMath.CerpVector(upRot, downRot, t);
    }

    /// /////////

    public void TransitionUp(Paper p)
    {
        if (smc.GetCurrentState() != idle)
            return;
        //takes the place of the state_start
        paperMesh.setEnabled(true);
        paperMesh.mMaterial.SetTexture(p.PaperTexture);

        //might be reference checks
        gameObject.transform.SetPosition(p.gameObject.transform.position);
        downPos = p.gameObject.transform.position;
        gameObject.transform.rotation.Angles = p.gameObject.transform.rotation.Angles;
        downRot = gameObject.transform.rotation.Angles;

        smc.SetState(transitionUp);
        isPaperSelected = true;
    }

    public void TransitionDown()
    {
        if (smc.GetCurrentState() != paperDisplayed)
            return;
        //assumed to be called after Transition Up, so pos are the same

        smc.SetState(transitionDown);
    }
#endif
}