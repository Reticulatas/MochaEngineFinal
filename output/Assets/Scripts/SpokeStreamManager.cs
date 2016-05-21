using MochaInterface;
using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;

public class SpokeStreamManager : MochaScript
{
	const float LOADDELAY = 60.0f;
	//ORDER DEPENDENT
	int cur_spoke = 0;
	CStreamZone cur_zone = null;
	//ADD STREAMZONES HERE
	string[] spokezones = { "streamspoke1zone", "streamspoke3zone" };
	List<CStreamZone> stream_zones = new List<CStreamZone>();
	
	float loadtimer = 0;
	
	public void OnStart()
	{
		foreach (string sz in spokezones)
		{
			CStreamZone zone = GameObject.GetGameObjectByName(sz).GetComponent<CStreamZone>();
			zone.setEnabled(false);
			stream_zones.Add(zone);
		}
		
		if (stream_zones.Count == 0)
		{
			Logger.Log("ERROR: SpokeStreamManager - No stream zones found");
			return;
		}
		
		ActivateZone(0);
	}
	
	void ActivateZone(int index)
	{
		cur_zone = stream_zones[index];
		stream_zones[index].setEnabled(true);
	}
	
	public void OnUpdate()
	{
		loadtimer += FrameController.DT();
		
		if (cur_zone != null && cur_zone.GetHasFinishedStreaming() && loadtimer > LOADDELAY)
		{
			loadtimer = 0;
			cur_spoke++;
			Logger.Log("NEW SPOKE ACTIVE: " + cur_spoke.ToString());
			if (cur_spoke < stream_zones.Count)
				ActivateZone(cur_spoke);
			else
			{
				cur_zone = null;
				Logger.Log("All spokes finished!");
			}
		}			
	}
}
