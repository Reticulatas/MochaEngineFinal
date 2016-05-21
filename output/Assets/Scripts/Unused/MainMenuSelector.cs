using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MainMenuSelector : MochaScript
{
	CTransform trans_;
	int item_;
	List<Vector2> itemPos_;
	
	enum Items
	{
		I_START,
		I_HOWTOPLAY,
		I_OPTIONS,
		I_CREDITS,
		I_QUIT,
		I_TOTAL
	};
	
	 
	public void OnStart()
	{
		trans_ = gameObject.RequireComponent<CTransform>();
		item_ = 0;
		itemPos_ = new List<Vector2>();
		
		// Quit
		itemPos_.Insert(0,new Vector2(0.047f,-0.075f));
		// Credits
		itemPos_.Insert(0,new Vector2(0.04f,-0.048f));
		// Options
		itemPos_.Insert(0,new Vector2(0.04f,-0.025f));
		// How To Play
		itemPos_.Insert(0,new Vector2(0.057f,0.0f));
		// Start
		itemPos_.Insert(0,new Vector2(0.03f,0.025f));
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
		else if(Input.GetTriggered(0,"MenuSelect") > 0.0f)
		{
			switch(item_)
			{
			case (int)Items.I_START:
				Map.LoadOnlyMap("Assets/Levels/Episode1_5.mocha");
			break;
			case (int)Items.I_HOWTOPLAY:
				Map.LoadMapDontUnload("Assets/Levels/howToPlay_Stealth.mocha");
			break;
			case (int)Items.I_OPTIONS:
			break;
			case (int)Items.I_CREDITS:
				Map.LoadMapDontUnload("Assets/Levels/credits.mocha");
			break;
			case (int)Items.I_QUIT:
				Map.LoadMapDontUnload("Assets/Levels/mainMenu_Quit.mocha");
			break;
			}
		}
		trans_.SetPosition(itemPos_[item_].X,itemPos_[item_].Y,0.1f);
	}
}
