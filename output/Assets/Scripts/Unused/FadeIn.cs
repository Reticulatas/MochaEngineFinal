using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class FadeIn : MochaScript
{
    CMeshRenderer cr;
    float timer;

	public void OnStart()
	{
        cr = gameObject.RequireComponent<CMeshRenderer>();
        cr.GetMaterial().SetColor(new Color(0.0f, 0.0f, 0.0f, 1.0f));
        timer = 0;
	}

	public void OnUpdate()
	{
        timer += FrameController.DT();

        float rgb = MMath.Cerp(0.0f, 1.0f, MMath.Clamp01(timer / 3.0f));
        cr.GetMaterial().SetColor(new Color(rgb, rgb, rgb, 1.0f));
	}
}
