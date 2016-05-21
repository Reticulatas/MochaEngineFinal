using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class BoobyTrapControllerScript : MochaScript
{
    BoobyTrapDeactivator panel1;
    BoobyTrapDeactivator panel2;
    BoobyTrapDeactivator panel3;
    BoobyTrapDeactivator panel4;
	GameObject trapLightW;
    GameObject trapLightR1; PulsingLightScript trapLightR1script;
    GameObject trapLightR2; PulsingLightScript trapLightR2script;
    GameObject trapLightR3; PulsingLightScript trapLightR3script;
    GameObject trapLightR4; PulsingLightScript trapLightR4script;
	GameObject poisonGas;
	CParticleEmitter gas;
	TransformRiserScript gasScript;
    BoobyTrapDeactivatorController controllerScript;
	bool activated = false;
	bool hasCompleted = false;

    CSound mSound;

	float gasTimerShutdown;
	
    public void OnStart()
    {
        GameObject panel = GameObject.GetGameObjectByName("PuzzleButton1").transform.GetChild(0).gameObject;
        panel1 = GetScript<BoobyTrapDeactivator>(panel);
        panel = GameObject.GetGameObjectByName("PuzzleButton2").transform.GetChild(0).gameObject;
        panel2 = GetScript<BoobyTrapDeactivator>(panel);
        panel = GameObject.GetGameObjectByName("PuzzleButton3").transform.GetChild(0).gameObject;
        panel3 = GetScript<BoobyTrapDeactivator>(panel);
        panel = GameObject.GetGameObjectByName("PuzzleButton4").transform.GetChild(0).gameObject;
        panel4 = GetScript<BoobyTrapDeactivator>(panel);

		trapLightW = GameObject.GetGameObjectByName("TrapLightW");
		trapLightR1 = GameObject.GetGameObjectByName("TrapLightR1");
        trapLightR1script = GetScript<PulsingLightScript>(trapLightR1);
		trapLightR2 = GameObject.GetGameObjectByName("TrapLightR2");
        trapLightR2script = GetScript<PulsingLightScript>(trapLightR2);
		trapLightR3 = GameObject.GetGameObjectByName("TrapLightR3");
        trapLightR3script = GetScript<PulsingLightScript>(trapLightR3);
		trapLightR4 = GameObject.GetGameObjectByName("TrapLightR4");
        trapLightR4script = GetScript<PulsingLightScript>(trapLightR4);
		
		poisonGas = GameObject.GetGameObjectByName("TSR_PoisonGas");
		gas = poisonGas.GetComponent<CParticleEmitter>();
		gasScript = GetScript<TransformRiserScript>(poisonGas);

        GameObject boobyControllerObj = GameObject.GetGameObjectByName("TSR_TrapController");//trapControllerName);
        controllerScript = GetScript<BoobyTrapDeactivatorController>(boobyControllerObj);
		
		gasTimerShutdown = gas.mEmitterProperty.mMaxLifetime;

        mSound = gameObject.RequireComponent<CSound>();
    }
    
    public void OnUpdate()
    {
		if(hasCompleted)
		{
			gasTimerShutdown -= FrameController.DT();
			

            if (gasTimerShutdown < 0.0f)
            {
                gas.setEnabled(false);
                hasCompleted = false;
            }
		}
    }

    public void ResetTrap(bool triggerOn)
    {
        trapLightW.GetComponent<CLight>().setEnabled(!triggerOn);
        trapLightR1.GetComponent<CLight>().setEnabled(triggerOn);
        trapLightR2.GetComponent<CLight>().setEnabled(triggerOn);
        trapLightR3.GetComponent<CLight>().setEnabled(triggerOn);
        trapLightR4.GetComponent<CLight>().setEnabled(triggerOn);
        trapLightR1script.isActive = triggerOn;
        trapLightR2script.isActive = triggerOn;
        trapLightR3script.isActive = triggerOn;
        trapLightR4script.isActive = triggerOn;
        gas.setEnabled(triggerOn);
        activated = triggerOn;
        gasScript.IsActive = triggerOn;
		if(!triggerOn)
			gasScript.Reset();
    }
	public void TriggerTrap()
	{
		if(activated)
			return;

        mSound.PlayIndependentEvent("PERCUSSIVE_A.vente", false, 0); // low pitched stab

        controllerScript.StartTrap();

        panel1.ActivateButton();
        panel2.ActivateButton();
        panel3.ActivateButton();
        panel4.ActivateButton();

        ResetTrap(true);
	}
	public void DeactivateTrap()
	{
		trapLightR1script.isActive = false;
        trapLightR2script.isActive = false;
        trapLightR3script.isActive = false;
        trapLightR4script.isActive = false;
		trapLightR1.GetComponent<CLight>().SetAtt(0.35f);
		trapLightR2.GetComponent<CLight>().SetAtt(0.35f);
		trapLightR3.GetComponent<CLight>().SetAtt(0.35f);
		trapLightR4.GetComponent<CLight>().SetAtt(0.35f);
		trapLightR1.GetComponent<CLight>().SetDiffuse(SharpMocha.YELLOW);
	    trapLightR2.GetComponent<CLight>().SetDiffuse(SharpMocha.YELLOW);
	    trapLightR3.GetComponent<CLight>().SetDiffuse(SharpMocha.YELLOW);
        trapLightR4.GetComponent<CLight>().SetDiffuse(SharpMocha.YELLOW);
		gas.mEmitterProperty.mIsEmitting = false;
		
		hasCompleted = true;
	}
}