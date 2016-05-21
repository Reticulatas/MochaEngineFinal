using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class Demo_RecordingFont : MochaScript
{
    CFontRenderer font;
	float timer = 0;
	public string text { get; set;}
	int index;
	
	public void OnStart()
	{
        font = gameObject.RequireComponent<CFontRenderer>();
		text = "[live] recording...";
	}

	public void OnUpdate()
	{
		timer += FrameController.DT();
		
		if (timer > 0.5f)
		{
			timer = 0;
			index++;
			font.mText = text.Substring(0, (text.Length - 3) + index);
			if (index > 2)
				index = -1;
		}
    }

}