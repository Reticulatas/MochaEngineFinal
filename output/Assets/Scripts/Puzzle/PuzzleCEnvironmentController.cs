using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PuzzleCEnvironmentController : MochaScript
{
    List<CLight> _allLights = new List<CLight>();
	List<CLight> _col0Lights = new List<CLight>();
	List<CLight> _col1Lights = new List<CLight>();
	List<CLight> _col2Lights = new List<CLight>();
	List<CLight> _col3Lights = new List<CLight>();
	
	List<CLight> flickeringLights = new List<CLight>();
	
	float _monsterMaxActiveTime = 0.0f;
	float _monsterMinActiveTime = 0.0f;
	
	float currAliveTimer = 0.0f;
	bool alive = false;
	public float MonsterMaxActiveTime  { get { return _monsterMaxActiveTime; } set { _monsterMaxActiveTime = value; }	}
	public float MonsterMinActiveTime  { get { return _monsterMinActiveTime; } set { _monsterMinActiveTime = value; }	}
	
	DistortForTime distortScript;
	
	GameObject chilloutFace;
	CTransform faceTransform;
	CSkinMeshRenderer faceRender;
	
	float flickerStart = 0.2f;
	
	CSound sound;
	
    public void OnStart()
    {
		foreach (GameObject obj in GameObject.GetGameObjectsWithTag("col0"))
        {
            _col0Lights.Add(obj.RequireComponent<CLight>());
            _allLights.Add(obj.RequireComponent<CLight>());
        }
		foreach (GameObject obj in GameObject.GetGameObjectsWithTag("col1"))
        {
            _col1Lights.Add(obj.RequireComponent<CLight>());
            _allLights.Add(obj.RequireComponent<CLight>());
        }
		foreach (GameObject obj in GameObject.GetGameObjectsWithTag("col2"))
        {
            _col2Lights.Add(obj.RequireComponent<CLight>());
            _allLights.Add(obj.RequireComponent<CLight>());
        }
		foreach (GameObject obj in GameObject.GetGameObjectsWithTag("col3"))
        {
            _col3Lights.Add(obj.RequireComponent<CLight>());
            _allLights.Add(obj.RequireComponent<CLight>());
        }
		
		chilloutFace = GameObject.GetGameObjectByName("lastchilloutface");
        faceRender = chilloutFace.RequireComponent<CSkinMeshRenderer>();
		faceTransform  = chilloutFace.RequireComponent<CTransform>();
		faceRender.setEnabled(false);
		
		distortScript = GetScript<DistortForTime>(gameObject);
		
		currAliveTimer = MMath.GetRandomLimitedFloat(_monsterMinActiveTime, _monsterMaxActiveTime);
		
		sound = gameObject.RequireComponent<CSound>();
    }

    public void OnUpdate()
    {
		if(!alive)
			return;
			
		currAliveTimer -= FrameController.DT();
		if(currAliveTimer < 0.0f)
		{
			faceRender.setEnabled(false);
			alive = false;
		}
		faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL));
		
		
    }
	
	bool TriggerChance()
	{
		return MMath.GetRandomInt(0,10) < 10;
	}
	
	public void TriggerColumnLights(uint col)
	{
		// deactivating flicker
        foreach (CLight obj in _allLights)
		{
            obj.setEnabled(true);
            //LightFlickerTemp script = GetScript<LightFlickerTemp>(obj.gameObject);
            //
            //if(script != null)
            //    script.activated = false;
		}
		flickeringLights.Clear();

		switch(col)
		{
			case 0: 

			// Flicker on col 1
			
	
			// Flicker on col 3

            _col1Lights[MMath.GetRandomInt(0, _col1Lights.Count - 1)].setEnabled(false);

            _col3Lights[MMath.GetRandomInt(0, _col3Lights.Count - 1)].setEnabled(false); 
			break;
			
			
			case 1: 
			
			// Flicker on col 0

			// Flicker on col 2

            _col0Lights[MMath.GetRandomInt(0, _col0Lights.Count - 1)].setEnabled(false);

            _col2Lights[MMath.GetRandomInt(0, _col2Lights.Count - 1)].setEnabled(false);  
			
			break;
			case 2: 

			// Flicker on col 3
			//flickeringLights.Add(_col3Lights[ MMath.GetRandomInt(0, _col3Lights.Count - 1) ] );
			// Flicker on col 0
			//flickeringLights.Add(_col0Lights[ MMath.GetRandomInt(0, _col0Lights.Count - 1) ] );
			_col3Lights[ MMath.GetRandomInt(0, _col3Lights.Count - 1) ].setEnabled(false);

            _col0Lights[MMath.GetRandomInt(0, _col0Lights.Count - 1)].setEnabled(false); 
			break;
			case 3:
			
			// Flicker on col 1
			//flickeringLights.Add(_col1Lights[ MMath.GetRandomInt(0, _col1Lights.Count - 1) ] );
			// Flicker on col 2


            _col1Lights[MMath.GetRandomInt(0, _col1Lights.Count - 1)].setEnabled(false);

            _col2Lights[MMath.GetRandomInt(0, _col2Lights.Count - 1)].setEnabled(false);  
			break;
		}
		
		foreach(CLight obj in flickeringLights)
		{
            obj.setEnabled(false);
            //LightFlickerTemp script = GetScript<LightFlickerTemp>(obj.gameObject);
            //if (script != null)
            //    script.activated = true;
		}
        distortScript.Trigger();
	}
	public void TriggerMonster(Vector3 spawn)
	{
		faceTransform.SetPositionX(spawn.X);
		faceTransform.SetPositionZ(spawn.Z);
		//faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL));
		faceRender.setEnabled(true);
		
		alive = true;
		
		currAliveTimer = MMath.GetRandomLimitedFloat(_monsterMinActiveTime, _monsterMaxActiveTime);
		
		sound.PlayIndependentEvent("C1_LAUGH", false, 1);
	}

    public void TurnOnLights()
    {
        foreach (CLight obj in _allLights)
        {
            obj.setEnabled(true);
        }
    }
    public void TurnOffLights()
    {
        foreach (CLight obj in _allLights)
        {
            obj.setEnabled(false);
        }
    }
    public void ResetState()
    {
        TurnOnLights();
        faceRender.setEnabled(false);
        alive = false;
    }
}
 