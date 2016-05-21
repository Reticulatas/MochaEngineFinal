using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class LightFlicker : MochaScript
{
    CLight light;
    float timer;
    bool dActivated = false;
    public bool activated
    {
        get { return dActivated; }
        set
        {
            if (mSound != null)
            {
                dActivated = value;
                String lightOnFilename = dActivated ? "F_LIGHT_SWITCH_ON.vente" : "F_LIGHT_SWITCH_OFF.vente";
                mSound.PlayIndependentEvent(lightOnFilename, false, 0);
            }
        }
    }

    float _onTime = 1.0f;
	float _transitionOffTime;
	float _transitionOnfTime;
	float _minOffHoldTime 	;
	float _maxOffHoldTime 	;
	float _minOnHoldTime 	;
	float _maxOnHoldTime 	;
	                        
	float _minoffinten 		;
	float _maxoffinten 		;
	float _minonintern 		;
	float _maxonintern 		;

    public float MinOffIntensity  { get { return _minoffinten; } set { _minoffinten = value; }	}
	public float MaxOffIntensity { get { return _maxoffinten; } set { _maxoffinten = value; } }
    public float MaxOnIntensity { get { return _maxonintern; } set { _maxonintern = value; } }    
	public float MinOnIntensity { get { return _minonintern; } set { _minonintern = value; } }
	public float TransitionOffTime { get { return _transitionOffTime; } set { _transitionOffTime = value; }}
	public float TransitionOnTime { get { return _transitionOnfTime; } set {_transitionOnfTime = value; }}
	public float MinOffHoldTime { get { return _minOffHoldTime; } set { _minOffHoldTime = value; }}
	public float MaxOffHoldTime { get { return _maxOffHoldTime; } set { _maxOffHoldTime = value; }}
	public float MinOnHoldTime { get { return _minOnHoldTime; } set { _minOnHoldTime = value; }}
	public float MaxOnHoldTime { get { return _maxOnHoldTime; } set { _maxOnHoldTime = value; }}

	
    State lighton, lightoff, lighthold;
    float mTargetIntensity, mBeginInten;

    StateMachineController smc;

    CSound mSound;

    public void OnStart()
    {
	_onTime = 1.0f;	
	_transitionOffTime = 0.1f;	
	_transitionOnfTime = 0.2f;	
	_minOffHoldTime 	 = 0.1f;	
	_maxOffHoldTime 	 = 0.5f;	
	_minOnHoldTime 	 = 0.1f;	
	_maxOnHoldTime 	 = 0.5f;	
		
	_minoffinten 		 = 0.1f;	
	_maxoffinten 		 = 0.5f;	
	_minonintern 		 = 0.8f;	
	_maxonintern 		 = 1.1f;	
		
		smc = new StateMachineController();
        light = gameObject.RequireComponent<CLight>();
        timer += MMath.GetRandomFloat() * 2.0f;

		lighthold = new State(smc, null, null, null, MinOnHoldTime, null);
        lighton = new State(smc, S_LightOn_Start, S_Light_Update, null, TransitionOnTime, lighthold);
        lightoff = new State(smc, S_LightOff_Start, S_Light_Update, null, TransitionOffTime, lighthold);
        smc.SetState(lightoff);

        mSound = gameObject.RequireComponent<CSound>();
    }

    public void S_LightOn_Start()
    {
        mTargetIntensity = MMath.GetRandomLimitedFloat(MinOnIntensity, MaxOnIntensity);
        mBeginInten = light.mIntensity;		
		
		lighthold.NextState = lightoff;
		lighthold.MaxTimeInState = MMath.GetRandomLimitedFloat(MinOffHoldTime, MaxOffHoldTime);
    }
    public void S_LightOff_Start()
    {
        mTargetIntensity = MMath.GetRandomLimitedFloat(MinOffIntensity, MaxOffIntensity);
        mBeginInten = light.mIntensity;
		
		lighthold.NextState = lighton;
		lighthold.MaxTimeInState = MMath.GetRandomLimitedFloat(MinOnHoldTime, MaxOnHoldTime);
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
