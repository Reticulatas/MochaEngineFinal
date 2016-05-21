using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PuzzleBEnvironmentController : MochaScript
{
	
	float _monsterMaxRestTime = 0.0f;
	float _monsterMinRestTime = 0.0f;
	float _monsterMaxActiveTime = 0.0f;
	float _monsterMinActiveTime = 0.0f;
	
	public float MonsterMaxInactiveTime  { get { return _monsterMaxRestTime; } set { _monsterMaxRestTime = value; }	}
	public float MonsterMinInactiveTime  { get { return _monsterMinRestTime; } set { _monsterMinRestTime = value; }	}
	
	public float MonsterMaxActiveTime  { get { return _monsterMaxActiveTime; } set { _monsterMaxActiveTime = value; }	}
	public float MonsterMinActiveTime  { get { return _monsterMinActiveTime; } set { _monsterMinActiveTime = value; }	}
	
	float currAliveTimer = 0.0f;
	float currDeadTimer = 0.0f;
	bool  isAlive = false;
	public bool activated = false;
	public bool finished = false;
	GameObject chilloutFace;
	CTransform faceTransform;
	CSkinMeshRenderer faceRender;
	
	int _region = 0;
	
	Dictionary<int, SpawnZone>        _spawnZones = new Dictionary<int, SpawnZone>();
	
	DistortForTime distortScript;
	
    public void OnStart()
    {
        //base.OnStart();
        currAliveTimer = MMath.GetRandomLimitedFloat(_monsterMinActiveTime, _monsterMaxActiveTime);
        currDeadTimer = MMath.GetRandomLimitedFloat(_monsterMinRestTime, _monsterMaxRestTime);
		
		chilloutFace = GameObject.GetGameObjectByName("maxChill");
        faceRender = chilloutFace.RequireComponent<CSkinMeshRenderer>();
		faceTransform  = chilloutFace.RequireComponent<CTransform>();
		faceRender.setEnabled(false);
		
		distortScript = GetScript<DistortForTime>(gameObject);
    }

    public void OnUpdate()
    {
		if(!activated || finished)
			return;
	
		float dt = FrameController.DT();
		
		if(isAlive)
		{
			currAliveTimer -= dt;
			faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL));
			if(currAliveTimer < 0.0f)
			{
				currAliveTimer = MMath.GetRandomLimitedFloat(_monsterMinActiveTime, _monsterMaxActiveTime);
				isAlive = false;
				faceRender.setEnabled(false);
			}
		}
		else
		{
			currDeadTimer -= dt;
		
			if(currDeadTimer < 0.0f)
			{
				currDeadTimer = MMath.GetRandomLimitedFloat(_monsterMinRestTime, _monsterMaxRestTime);
				isAlive = true;
				ActivateMonster();
				distortScript.Trigger();
			}
		}
    }
	
	public void ActivateRegion(int region)
	{
	    //1 == A
		//2 == B
		//3 == C
		//4 == D
		_region = region;
	}
	public void AddSpawnZone(int region, SpawnZone spawn)
	{
		_spawnZones.Add(region, spawn);
	}
	
	
	void ActivateMonster()
	{
		//1 == A
		//2 == B
		//3 == C
		//4 == D
		switch(_region)
		{
			case 1: 
			{
				faceTransform.SetPositionX(_spawnZones[_region].GetRandomPosition().X);
				faceTransform.SetPositionZ(_spawnZones[_region].GetRandomPosition().Z);
				faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL));
				faceRender.setEnabled(true);
				break; 
			}
			case 2: 
			{ 
				faceTransform.SetPositionX(_spawnZones[_region].GetRandomPosition().X);
				faceTransform.SetPositionZ(_spawnZones[_region].GetRandomPosition().Z);
				faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL));
				faceRender.setEnabled(true);
				break; 
			}
			case 3: 
			{ 
				faceTransform.SetPositionX(_spawnZones[_region].GetRandomPosition().X);
				faceTransform.SetPositionZ(_spawnZones[_region].GetRandomPosition().Z);
				faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL));
				faceRender.setEnabled(true);
				break; 
			}
			case 4: 
			{ 
				faceTransform.SetPositionX(_spawnZones[_region].GetRandomPosition().X);
				faceTransform.SetPositionZ(_spawnZones[_region].GetRandomPosition().Z);
				faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL));
				faceRender.setEnabled(true);
				break; 
			}
			default: 
			{ 
				//faceTransform.SetPosition(_spawnZones[_region]);
				break; 
			}
		}
	}

}
 