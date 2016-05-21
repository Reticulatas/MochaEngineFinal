using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PauseMenuSelector : MochaScript
{
	CTransform trans_;
	int item_;
	List<Vector2> itemPos_;
	
	enum Items
	{
		I_RESUME,
		I_HOWTOPLAY,
		I_OPTIONS,
		I_MAINMENU,
		I_QUIT,
		I_TOTAL
	};
	
	 
	public void OnStart()
	{
		trans_ = gameObject.RequireComponent<CTransform>();
		item_ = 0;
		itemPos_ = new List<Vector2>();
		
		// Quit
		itemPos_.Insert(0,new Vector2(0.14f,-0.045f));
		// Main Menu
		itemPos_.Insert(0,new Vector2(0.14f,-0.012f));
		// Options
		itemPos_.Insert(0,new Vector2(0.14f,0.022f));
		// How To Play
		itemPos_.Insert(0,new Vector2(0.14f,0.032f));
		// Resume
		itemPos_.Insert(0,new Vector2(0.16f,0.042f));
	}

	public void OnUpdate()
	{
		float upVal = Math.Max(Input.GetTriggered(0,"up"),Input.GetTriggered(0,"arrowUp"));
		float downVal = Math.Max(Math.Abs(Input.GetTriggered(0,"down")),Input.GetTriggered(0,"arrowDown"));
		
		// supports xbox controller's analogue mapping and keyboard binary inputs
		if(upVal > 0.0f)
		{
			if(item_ == 0)
				item_ = (int)Items.I_TOTAL - 1;
			else
				item_--;
		}
		else if(downVal > 0.0f || upVal < 0.0f )
		{
			if(item_ == (int)Items.I_TOTAL - 1)
				item_ = 0;
			else
				item_++;
		}
		else if(Input.GetTriggered(0,"Exit") > 0.0f || Input.GetTriggered(0,"MenuBack") > 0.0f)
		{
			Engine.getInstance().SetPauseMode(false);
			Map.UnloadMap();
		}
		else if(Input.GetTriggered(0,"MenuSelect") > 0.0f)
		{
			switch(item_)
			{
			case (int)Items.I_RESUME:
				Engine.getInstance().SetPauseMode(false);
				Map.UnloadMap();
			break;
			case (int)Items.I_HOWTOPLAY:
				Map.LoadMapDontUnload("Assets/Levels/howToPlay_Stealth.mocha");
			break;
			case (int)Items.I_OPTIONS:
			break;
			case (int)Items.I_MAINMENU:
				Map.LoadMapDontUnload("Assets/Levels/pauseMenu_MainMenu.mocha");
			break;
			case (int)Items.I_QUIT:
				Map.LoadMapDontUnload("Assets/Levels/pauseMenu_Quit.mocha");
			break;
			}
		}
		trans_.SetPosition(itemPos_[item_].X,itemPos_[item_].Y,0.1f);
	}
}
