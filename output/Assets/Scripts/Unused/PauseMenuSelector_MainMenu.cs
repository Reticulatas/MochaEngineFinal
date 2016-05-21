using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class PauseMenuSelector_MainMenu : MochaScript
{
	CTransform trans_;
	int item_;
	List<Vector2> itemPos_;
	
	enum Items
	{
		I_YES,
		I_NO,
		I_TOTAL
	};
	
	 
	public void OnStart()
	{
		trans_ = gameObject.RequireComponent<CTransform>();
		item_ = (int)Items.I_NO;
		itemPos_ = new List<Vector2>();
		
		// No
		itemPos_.Insert(0,new Vector2(0.105f,-0.038f));
		// Yes
		itemPos_.Insert(0,new Vector2(0.105f,-0.029f));
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
			case (int)Items.I_YES:
				Engine.getInstance().SetPauseMode(false);
				// Sound.StopAllAudio();
				Map.LoadOnlyMap("Assets/Levels/mainMenu.mocha");
			break;
			case (int)Items.I_NO:
				Map.UnloadMap();
			break;
			}
		}
		else if(Input.GetTriggered(0,"Exit") > 0.0f || Input.GetTriggered(0,"MenuBack") > 0.0f)
		{
			Map.UnloadMap();
		}
		trans_.SetPosition(itemPos_[item_].X,itemPos_[item_].Y,0.1f);
	}
}
