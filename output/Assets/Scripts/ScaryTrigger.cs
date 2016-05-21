using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class ScaryTrigger : TriggerAreaBaseClass
{

	String scaryObjName;
	String scaryLightName;
	
	CLight light;
	LightFlicker lightScript;
	DistortForTime distortScript;
	CSkinMeshRenderer scaryRender;
	
	CSound _sound;
	String soundName;
	String monsterCroak;
	public String ScaryObject { get { return scaryObjName; }
    set { scaryObjName = value; } }
	public String ScaryLight { get { return scaryLightName; }
    set { scaryLightName = value; } }
	bool activated = false;
    bool finished = false;
	int flickerCount = 7;
	float timer = 2.0f;
	
	bool lit = false;
    public override void OnStart()
    {
		base.OnStart();
		monsterCroak = "C1_PAIN.vente";
		GameObject face = GameObject.GetGameObjectByName(scaryObjName);
		GameObject lightObj = GameObject.GetGameObjectByName(scaryLightName);

        light = lightObj.RequireComponent<CLight>();
		lightScript = GetScript<LightFlicker>(lightObj);
		distortScript = GetScript<DistortForTime>(gameObject);
		scaryRender = face.RequireComponent<CSkinMeshRenderer>();
		scaryRender.setEnabled(false);
		_sound = gameObject.RequireComponent<CSound>();
    }
    
    public void OnUpdate()
    {
        if (!activated || finished)
			return;
		//	
		if(timer > 0.0f)
		{
			timer -= FrameController.DT();
			//break;
		}
		else
		{
			lightScript.activated = false;
			light.setEnabled(true);
			light.SetAtt(0.03f);
			scaryRender.setEnabled(false);
			finished = false;
		}

        //light.setEnabled(false);
    }
	public override void OnAreaEnter()
    {
		if(activated)
			return;
	
		lightScript.activated = true;
        activated = true;
		scaryRender.setEnabled(true);
		light.setEnabled(true);
		distortScript.Activated = true;
		lit = true;
		_sound.PlayIndependentEvent("HORN.vente", false, 0);
		_sound.PlayIndependentEvent(monsterCroak, false, 0);
    }
    public override void OnArea()
    {

    }
}