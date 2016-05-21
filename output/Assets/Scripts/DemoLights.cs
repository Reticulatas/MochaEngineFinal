using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class DemoLights : MochaScript
{
    CZone myZone;
    StateMachineController smc;
    State unactive, lightson, finished, lightsoff, finale, lightsonwait;
    List<CLight> lights = new List<CLight>();
    int current_light = 0;
    CSound mySound;
    int stage = 0;
	Random rand;

    public void OnStart()
    {
		rand = new Random();
        myZone = gameObject.RequireComponent<CZone>();
        //mySound.AddSound("RadioStatic.vente");
        //mySound.AddSound("BeginOutro.vente");
        //mySound.AddSound("Heartbeat.vente");
        //mySound.Loop(true, "static.vente");
        mySound = gameObject.RequireComponent<CSound>();
        //mySound.PlayIndependentEvent("static.vente");

        smc = new StateMachineController();
        unactive = new State(smc, null, State_unactive_Update, null);
        lightson = new State(smc, State_lightson_Start, State_lightson_Update, null, 1.0f);
        lightsoff = new State(smc, State_lightsoff_Start, null, null, 1.0f, lightson);
        lightsonwait = new State(smc, null, null, null, 1.0f, lightsoff);
        lightson.NextState = lightson;
        smc.SetState(unactive);

        foreach (GameObject g in GameObject.GetGameObjectsWithTag("TLight"))
        {
            lights.Add(g.RequireComponent<CLight>());
            g.RequireComponent<CSound>(); //.AddSound("LightTurnOn.vente");
        }
        foreach (GameObject g in GameObject.GetGameObjectsWithTag("FlickerLight")) {
			GetScriptClass<LightFlicker>(g).activated = true;
		}
        
		MonsterEncounter1 me = GetScript<MonsterEncounter1>(GameObject.GetGameObjectByName("Monster1"));
        me.gameObject.RequireComponent<CMeshRenderer>().setEnabled(false);
    }

    public void OnUpdate()
    {
        if (lights.Count == 0)
        {
            Logger.Log("Invalid number of TLight tagged objects for Trigger Lights to run");
            gameObject.Destroy();
        }
        smc.Update();
    }

    public void State_unactive_Update()
    {
        //if (myZone.IsPointWithin(Common.GetStealthPlayer().transform.position))
        //{
            smc.SetState(lightson);

            mySound.StopAllGlobalEvents();
            mySound.PlayIndependentEvent("static.vente", true, 0);
        //}
    }

    public void State_lightson_Start()
    {
		MonsterEncounter1 me = GetScript<MonsterEncounter1>(GameObject.GetGameObjectByName("Monster1"));
		
		if (current_light == 0)
			lightson.MaxTimeInState = (float)rand.NextDouble()+0.4f;
		
        if (current_light < lights.Count)
        {
            CLight light = lights[current_light];
            light.setEnabled(true);
            CSound sound = light.gameObject.RequireComponent<CSound>();
            sound.PlayIndependentEvent("LightTurnOn.vente", false, 1);
        }
        else
        {         
			if (!me.gameObject.RequireComponent<CMeshRenderer>().isEnabled())	
				lightson.MaxTimeInState = (float)rand.NextDouble()*15.0f+5.0f;           
			lightson.NextState = lightsoff;
            return;
        }

        current_light++;
    }

    public void State_lightson_Update()
    {
        //wait for next cycle
    }

    public void State_lightsoff_Start()
    {
		MonsterEncounter1 me = GetScript<MonsterEncounter1>(GameObject.GetGameObjectByName("Monster1"));
		if (me.gameObject.RequireComponent<CMeshRenderer>().isEnabled())
		{
			//load demo subject redacted
			Map.LoadMap("Assets/Levels/logo_stinger.mocha");
			return;
		}
		me.gameObject.RequireComponent<CMeshRenderer>().setEnabled(false);

		//if (rand.Next(0,15) == 5) 
		{
			me.gameObject.RequireComponent<CMeshRenderer>().setEnabled(true);
			me.Rotate();
		}
		lightson.NextState = lightson;
        lightsoff.MaxTimeInState = ((float)rand.NextDouble()*8.0f)+1.0f;
        current_light = 0;
        foreach (CLight l in lights)
            l.setEnabled(false);
    }
}
