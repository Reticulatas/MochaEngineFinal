using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class PauseMenuInGame : MochaScript
{
	public void OnStart()
	{
	}

	public void OnUpdate()
	{
		if(!Engine.getInstance().GetIsEditorMode() &&
		   (Input.GetTriggered(0,"exit") > 0.0f ||!Engine.getInstance().IsGameInFocus()))
		{
			Engine.getInstance().SetPauseMode(true);
			// Sound.PauseAllAudio();
			Map.LoadMapDontUnload("Assets/Levels/pauseMenu.mocha");
		}
	}
}
