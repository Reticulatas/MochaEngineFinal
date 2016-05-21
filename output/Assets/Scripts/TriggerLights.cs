using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TriggerLights : MochaScript
{
    CZone myZone;
    StateMachineController smc;
    State unactive, lightson, finished, lightsoff, finale, lightsonwait;
    List<CLight> lights = new List<CLight>();
    int current_light = 0;
    CSound mySound;
    int stage = 0;

    static float[] _lightson = { 0.2f, 0.05f, 0.03f, 0.03f, 0.03f, 0.02f, 0.02f, 0.02f, 0.01f, 0.03f, 0.02f, 0.02f, 0.02f };
    static float[] _lightsoff = { 0.9f, 0.6f, 0.04f, 0.03f, 0.03f, 0.03f, 0.03f, 0.02f, 0.02f, 0.03f, 0.02f, 0.02f, 0.02f };

    public void OnStart()
    {
        myZone = gameObject.RequireComponent<CZone>();
        //mySound.AddSound("RadioStatic.vente");
        //mySound.AddSound("BeginOutro.vente");
        //mySound.AddSound("Heartbeat.vente");
        //mySound.Loop(true, "static.vente");
        mySound = gameObject.RequireComponent<CSound>();
        //mySound.PlayIndependentEvent("static.vente");

        smc = new StateMachineController();
        unactive = new State(smc, null, State_unactive_Update, null);
        lightson = new State(smc, State_lightson_Start, State_lightson_Update, null, _lightson[0]);
        lightsoff = new State(smc, State_lightsoff_Start, null, null, 1.0f, lightson);
        finished = new State(smc);
        finale = new State(smc, State_finale_Start, null, State_finale_End, 5.0f, finished);
        lightsonwait = new State(smc, null, null, null, 1.0f, lightsoff);
        lightson.NextState = lightson;
        smc.SetState(unactive);

        foreach (GameObject g in GameObject.GetGameObjectsWithTag("TLight"))
        {
            lights.Add(g.RequireComponent<CLight>());
            g.RequireComponent<CSound>(); //.AddSound("LightTurnOn.vente");
        }
        foreach (CLight l in lights)
            l.setEnabled(false);

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
        if (myZone.IsPointWithin(Common.GetStealthPlayer().transform.position))
        {
            smc.SetState(lightson);

            mySound.StopAllGlobalEvents();
            mySound.PlayIndependentEvent("static.vente", true, 0);
        }
    }

    public void State_lightson_Start()
    {
        if (current_light < lights.Count)
        {
            CLight light = lights[current_light];
            light.setEnabled(true);
            CSound sound = light.gameObject.RequireComponent<CSound>();
            sound.PlayIndependentEvent("LightTurnOn.vente", false, 1);
        }
        else
        {
            if (stage == 1)
            {
                foreach (GameObject g in GameObject.GetGameObjectsWithTag("FlickerLight"))
                    GetScriptClass<LightFlicker>(g).activated = true;
            }

            //active monster
            lightson.MaxTimeInState = _lightson[stage];
            if (stage != 0)
            {
                MonsterEncounter1 me = GetScript<MonsterEncounter1>(GameObject.GetGameObjectByName("Level4Monster1"));
                me.Advance();
                if (me.gameObject.transform.position.z < 20)
                {
                    lightsoff.NextState = finale;
                    lightsoff.MaxTimeInState = 5.0f;
                }
                smc.SetState(lightsoff);
            }
            else
            {
                MonsterEncounter1 me = GetScript<MonsterEncounter1>(GameObject.GetGameObjectByName("Level4Monster1"));
                me.Advance();
                smc.SetState(lightsonwait);
            }
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
        lightsoff.MaxTimeInState = _lightsoff[stage++];
        current_light = 0;
        foreach (CLight l in lights)
            l.setEnabled(false);
    }

    public void State_finale_Start()
    {
        //mySound.StopAll();
        mySound.PlayIndependentEvent("Heartbeat.vente", false, 0);
        foreach (GameObject g in GameObject.GetGameObjectsWithTag("FlickerLight"))
        {
            g.RequireComponent<CLight>().setEnabled(false);
            GetScriptClass<LightFlicker>(g).activated = false;
        }
    }
    public void State_finale_End()
    {
        mySound.StopAllGlobalEvents();
		Map.LoadMap("Assets/Levels/logo_stinger.mocha");
    }
}
