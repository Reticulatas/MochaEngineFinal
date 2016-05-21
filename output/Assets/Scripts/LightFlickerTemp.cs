using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class LightFlickerTemp : MochaScript
{
    CLight light;
    float timer;
    public bool activated { get; set; }

    float _onTime = 1.0f;
	float _transitionOffTime;
	float _transitionOnTime;
	float _minOffHoldTime 	;
	float _maxOffHoldTime 	;
	float _minOnHoldTime 	;
	float _maxOnHoldTime 	;
	                        
	float _minoffinten 		;
	float _maxoffinten 		;
	float _minoninten 		;
	float _maxoninten 		;

	
    State lighton, lightoff, lighthold;
    float mTargetIntensity, mBeginInten;

    StateMachineController smc;

    public void OnStart()
    {
	_onTime = 1.0f;	
	_transitionOffTime = 0.0f;	
	_transitionOnTime = 0.1f;	
	_minOffHoldTime 	 = 1.05f;	
	_maxOffHoldTime 	 = 1.15f;	
	_minOnHoldTime 	 = 1.1f;	
	_maxOnHoldTime 	 = 1.9f;	
		
	_minoffinten 		 = 0.3f;	
	_maxoffinten 		 = 0.3f;	
	_minoninten 		 = 0.7f;	
	_maxoninten 		 = 1.0f;	
		
		smc = new StateMachineController();
        light = gameObject.RequireComponent<CLight>();
        timer += MMath.GetRandomFloat() * 2.0f;

        lighthold = new State(smc, null, null, null, _minOnHoldTime, null);
        lighton = new State(smc, S_LightOn_Start, S_Light_Update, null, _transitionOnTime, lighthold);
        lightoff = new State(smc, S_LightOff_Start, S_Light_Update, null, _transitionOffTime, lighthold);
        smc.SetState(lightoff);
    }

    public void S_LightOn_Start()
    {
        mTargetIntensity = MMath.GetRandomLimitedFloat(_minoninten, _maxoninten);
        mBeginInten = light.mIntensity;		
		
		lighthold.NextState = lightoff;
        lighthold.MaxTimeInState = MMath.GetRandomLimitedFloat(_minOffHoldTime, _maxOffHoldTime);
    }
    public void S_LightOff_Start()
    {
        mTargetIntensity = MMath.GetRandomLimitedFloat(_minoffinten, _maxoffinten);
        mBeginInten = light.mIntensity;
		
		lighthold.NextState = lighton;
        lighthold.MaxTimeInState = MMath.GetRandomLimitedFloat(_minOnHoldTime, _maxOnHoldTime);
    }

    public void S_Light_Update()
    {
        light.mIntensity = MMath.Cerp(mBeginInten, mTargetIntensity, smc.GetCurrentState().GetPercentageTime());
    }

    public void OnUpdate()
    {
        if (activated)
        {
            smc.Update();
        }
    }
}
