using MochaInterface;
using System.Collections.Generic;
using System;
using Microsoft.Xna.Framework;	
	
public class Monster4TriggerArea : TriggerAreaBaseClass
{
	//light array
	List<CLight> light_array = new List<CLight>();
	Vector2 light_array_pos = new Vector2(0,3);
	
	StateMachineController smc;
	State idle, lightson, lightsoff;
	
	int light_index;
	
	public int M4LightsToTurnOff = 0;		//see Monster3Light
	
	public override void OnStart()
	{
		smc = new StateMachineController();
        Logger.Log("Monster4 trigger initialized.");
	
		base.OnStart();
		
		CreateLightArray();
		
		idle = new State(smc, S_idle_Start, null, null);
		lightson = new State(smc, S_lightson_Start, null, null, 0.04f, null);
		lightson.NextState = lightson;
		lightsoff = new State(smc, S_lightsoff_Start, null, null, 0.01f, null);
		lightsoff.NextState = lightsoff;
		
		smc.SetState(idle);
	}
	
	public void OnUpdate()
	{
		smc.Update();
	}

    void StartLightFlicker(int index, bool setOn)
    {
        light_array[index].setEnabled(setOn);
        LightFlicker flickerScript = GetScript<LightFlicker>(light_array[index].gameObject);
        if (flickerScript != null)
            flickerScript.activated = false;//setOn;
    }	
	
	bool firstlight = false;
	public void M4LightTurnedOn()
	{
        if (!firstlight)
        {
            OcuConsoleScreen screen = Common.GetConsoleScreenScript();

            screen.ChangeHeader("Subject 54");
            screen.QueueMessage("ENTERING SUBJECT 54 CONTAINMENT AREA", 0.0f, false, true);
            screen.QueueMessage("HANDLER PROCEED WITH CAUTION", 0.0f, false, false);
            screen.QueueMessage("-----------------------", 0.0f, false, false);
            screen.QueueMessage("Subject 54", 0.0f, false, false);
            screen.QueueMessage("Class A Lethal", 0.0f, false, false);
            screen.QueueMessage("Subject exhibits aggressive tendencies.",0.0f, false, false);
            screen.QueueMessage("Subject recovered from mainland Russia.",0.0f,false,false);
            screen.QueueMessage("after night slaughter of cattle. Cattle",0.0f,false,false);
            screen.QueueMessage("covered in unknown substance, lab tests", 0.0f, false, false);
            screen.QueueMessage("show cows dismembered themselves.      ", 0.0f, false, false);
            screen.QueueMessage("<#0,0,1,1#>HANDLER CORRUPTION LEVEL WILL BE TRACKED", 0.0f, true, false);
            screen.QueueMessage("<#0,0,1,1#>FOR DURATION OF EXPOSURE.", 1.0f, true, false);

            GetScript<Monster4Logic>(GameObject.GetGameObjectByName("Monster3")).IsActive = true;
        }
		firstlight = true;
		
		M4LightsToTurnOff--;
		
		Logger.Log("Lights Left for M4: " + M4LightsToTurnOff.ToString());
		
		if (M4LightsToTurnOff == 0)
		{
			//ooga booga, you beat the boss-a
			smc.SetState(lightson);
			GetScript<Monster4Logic>(GameObject.GetGameObjectByName("Monster3")).Kill();
		}
	}
	
	#region STATES	
	
	public void S_idle_Start()
	{
		light_index = 0;
	}
	
	public void S_lightson_Start()
	{
		if (light_index >= 0 && light_index < light_array.Count)
		{
			StartLightFlicker(light_index,true);
			light_array[light_index].SetAtt(light_array[light_index].GetAtt()*2);
			light_index++;
		}
		else
			smc.SetState(idle);
	}
	
	public void S_lightsoff_Start()
	{
		if (light_index >= 0 && light_index < light_array.Count)
		{
			StartLightFlicker(light_index,false);
			
			light_index++;
		}
		else
			smc.SetState(idle);
	}
	
	#endregion
	
	bool entered = false;
	public override void OnAreaEnter()
    {
		if (entered)
			return;
		entered = true;
		
		Logger.Log("Entered M4 Area");
        OcuConsoleScreen screen = Common.GetConsoleScreenScript();
        screen.QueueMessage("<#1,0,0,1#>Containment Area Light Malfunction.", 0.0f, true, true);
        screen.QueueMessage("<#1,1,0,1#>Activate Local Lights to Restore Power.", 0.0f, true, true);
		
		smc.SetState(lightsoff);		       
    }
    public override void OnArea()
    {

    }
	
	void CreateLightArray()
	{
        GameObject lightmarker = GameObject.GetGameObjectByName("M4LightArrayMarker");

		for (int x = -150; x < 100; x += 20)
		{
			for (int z = -150; z < 100; z += 20)
			{
				GameObject l = ObjectFactory.getInstance().Instantiate("M4LightArrayPrefab");
				l.transform.SetPositionX(lightmarker.transform.position.x + light_array_pos.X + x);
				l.transform.SetPositionZ(lightmarker.transform.position.z + light_array_pos.Y + z);
                l.transform.SetPositionY(lightmarker.transform.position.y);

				//l.transform.SetPositionY(-7.0f);
				//GetScript<LightFlicker>(l).activated = true;
	
				l.GetComponent<CLight>().setEnabled(true);
				light_array.Add(l.GetComponent<CLight>());
			}
		}
		Shuffle<CLight>(light_array);
	}	

	public void Shuffle<T>(IList<T> list)  
	{  
		Random rng = new Random();  
		int n = list.Count;  
		while (n > 1) {  
			n--;  
			int k = rng.Next(n + 1);  
			T value = list[k];  
			list[k] = list[n];  
			list[n] = value;  
		}  
	}	
}