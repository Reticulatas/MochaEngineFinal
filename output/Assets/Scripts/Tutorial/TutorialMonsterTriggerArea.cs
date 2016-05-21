using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class TutorialMonsterTriggerArea : TriggerAreaBaseClass
{
    DoorMain door;
    GameObject monster;
    Vector3 monsterInitPos;
    GameObject glass;
    List<CLight> flickeringLights;

    CSound mSound;
    TutorialGlassSound glassScript;

    float timer;
    float originalIntensity;
    public float mDistanceToGlass { get; set; }
    public float mDistanceToRun { get; set; }

    int scriptedState = 0;
    float scriptTimer;
    Vector3 mDirectionToRun;
    Vector3 mDirectionTowardGlass;

    bool isActivated = false; // has player stepped into the area

	public override void OnStart()
    {
        base.OnStart();

        mSound = gameObject.RequireComponent<CSound>();

        flickeringLights = new List<CLight>();
        GameObject lightObj = GameObject.GetGameObjectByName("LightFlicker");
        CLight flickeringLight = lightObj.RequireComponent<CLight>();
        originalIntensity = flickeringLight.GetAtt();
        flickeringLights.Add(flickeringLight);

        for (int index = 2; index <= 6; ++index)
        {
            string lightName = "LightFlicker " + index.ToString();
            lightObj = GameObject.GetGameObjectByName(lightName);
            flickeringLight = lightObj.RequireComponent<CLight>();
            flickeringLights.Add(flickeringLight);
        }
        foreach (CLight light in flickeringLights)
        {
            light.setEnabled(false); // Turn off all lights at start
        }
        
        glass = GameObject.GetGameObjectByName("GlassMonster");
        glassScript = GetScript<TutorialGlassSound>(glass);

        monster = GameObject.GetGameObjectByName("Monster");
        monsterInitPos = monster.transform.position;
        GameObject doorObj = GameObject.GetGameObjectByName("door6");
        door = GetScript<DoorMain>(doorObj);

        mDirectionToRun = gameObject.transform.GetForwardVector();
        mDirectionTowardGlass = gameObject.transform.GetRightVector();
    }

    public void OnUpdate()
    {
        float dt = FrameController.DT();

        if (isActivated) // Start scripted monster moving and disappearing and light controlled flicker
        {
            scriptTimer -= dt;
            
            Vector3 glassToPlayer = (Vector3)Common.GetStealthPlayer().transform.position - (Vector3)gameObject.transform.position;
            float distZ = Vector3.Dot(glassToPlayer, mDirectionToRun); // distance player is from trigger
            float distX = Vector3.Dot(glassToPlayer, mDirectionTowardGlass);

            switch (scriptedState)
            {
            case 0:
                // As player walks towards glass, turn on more lights to flicker
                if (distX > 5.0f)
                {
                    StartLightFlicker(2, true);
                    ++scriptedState;
                }
                break;
            case 1:
                if (distX > 11.0f)
                {
                    // after some time, 
                    StartLightFlicker(1, true); // nearer to monster
                    // Play sound at monster position
                    mSound.PlayIndependentEventStatic3D("C1_ANGRY.vente", monsterInitPos, false);

                    scriptTimer = 0.8f;
                    ++scriptedState;
                }
                break;
            case 2:
                if (scriptTimer <= 0.0f)
                {
                    StartLightFlicker(0, true); // shows the monster
                    scriptTimer = 1.2f;
                    ++scriptedState;
                }
                break;
            case 3:
                if (scriptTimer <= 0.0f)
                {
                    StartLightFlicker(4, true); // first light along corridor
                    scriptTimer = 0.6f;
                    ++scriptedState;
                }
                break;
            case 4:
                if (scriptTimer <= 0.0f)
                {
                    StartLightFlicker(5, true); // second light along corridor
                    scriptTimer = 1.0f;
                    ++scriptedState;
                }
                break;
            case 5: 			
                if (scriptTimer <= 0.0f)
				{
					Logger.Log("Happening");
                    StartLightFlicker(0, false);
					StartLightFlicker(1, false);
					scriptTimer = 1.0f;
					++scriptedState;
				}              
                break;
            case 6:// This is when player should be running towards hub
				if (distZ > 4.0f)      
				{
					StartLightFlicker(3, false);
                    StartLightFlicker(2, false);
                    StartLightFlicker(1, false);
                    scriptTimer = 1.0f;
                    ++scriptedState;
                }
                break;
            case 7:
                if (scriptTimer <= 0.0f)
                {
                    StartLightFlicker(0, false);
                    scriptTimer = 1.0f;
                    ++scriptedState;
                }
                break;
			case 8:

				break;
            }

            // As player walks to run, turn off lights behind
			/*
#if ZERO
            if (scriptTimer <= 0.0f && scriptedState <= 6)
            {
                switch (scriptedState)
                {
                    case 0: // 0 - turn off
                        flickeringLight.SetAtt(0.0f);
                        flickeringLight2.SetAtt(0.3f);
                        scriptTimer = timer0;
                        break;
                    case 1: // 1 - monster in front
                    {
                        float xPos = glass.transform.position.x - monsterInitPos.X;
                        xPos = monsterInitPos.X + xPos * 0.65f; // Halfway towards the glass
                        Vector3 newPos = monsterInitPos;
                        newPos.X = xPos;
                        monster.transform.SetPosition(newPos);

                        flickeringLight.SetAtt(originalIntensity);
                        allowFlickering = true;
                        timer = 0.1f;
                        scriptTimer = timer1;
                        break;
                    }
                    case 2: // 2 - turn off
                        flickeringLight.SetAtt(0.0f);
                        monster.transform.SetPosition(monsterInitPos);

                        allowFlickering = false;
                        scriptTimer = timer2;
                        break;
                    case 3: // 3 - monster back with flickering
                        flickeringLight2.SetAtt(originalIntensity);
                        allowFlickering = true;
                        scriptTimer = timer3;
                        break;
                    case 4: // 4 - turn off
                    {
                        flickeringLight.SetAtt(0.0f);
                        flickeringLight2.SetAtt(0.3f);

                        allowFlickering = false;
                        scriptTimer = timer4;
                        break;
                    }
                    case 5: // 5 - monster at glass with flickering
                    {
                        float xPos = glass.transform.position.x - monsterInitPos.X;
                        xPos = monsterInitPos.X + xPos * 0.9f; // At the glass
                        Vector3 newPos = monsterInitPos;
                        newPos.X = xPos;
                        newPos.Z = glass.transform.position.z;
                        monster.transform.SetPosition(newPos);

                        allowFlickering = true;
                        break;
                    }
                }
                ++scriptedState; // Move on to the next stage
            }
#endif
			*/
        }
    }
    public void OnEditorUpdate()
    {
        //OnUpdate();
    }
/*
    void Flicker()
    {
        return;

        float dt = FrameController.DT();
        timer -= dt;
        if (timer <= 0.0f)
        {           
            timer = MMath.GetRandomLimitedFloat(minTimeInterval, maxTimeInterval); // Reset the timer
            float newIntensity = originalIntensity;

            switch (flickerState)
            {
                case 0: // idle (first time triggered into flickering)
                    timer = 0.0f;
                    flickerState = 1; // Go to bright next frame
                    break;
                case 1: // bright
                    newIntensity = MMath.GetRandomLimitedFloat(originalIntensity, maxIntensity);

                    flickerState = 2;
                    break;
                case 2: // dim
                    newIntensity = MMath.GetRandomLimitedFloat(minIntensity, originalIntensity);

                    flickerState = 1;
                    break;
            }
            if (flickeringLights.Count > 0)
                flickeringLights[0].SetAtt(newIntensity);
        }
    }
*/

    void StartLightFlicker(int index, bool setOn)
    {
        flickeringLights[index].setEnabled(setOn);
        LightFlicker flickerScript = GetScript<LightFlicker>(flickeringLights[index].gameObject);
        if (flickerScript != null)
        {
            flickerScript.activated = setOn;
        }
    }
    public override void OnAreaEnter()
    {
        if (isactive)
        {
            scriptTimer = 0.0f;
            isActivated = true; // Collision just activates the scripted light behavior

            // Start the first light to flicker
            StartLightFlicker(3, true);

            if (glassScript != null)
            {
                glassScript.TriggerStartGlassSound();
            }

            isactive = false; // Only allow 1 trigger
        }
    }
    public override void OnArea()
    {

    }
}
