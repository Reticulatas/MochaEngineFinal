using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class PulsingLight : MochaScript
{
    public float amplitude { get { return _amplitude; } set { _amplitude = value; } }
    float _amplitude = 1.0f;

    public float frequency { get { return _frequency; } set { _frequency = value; } }
    float _frequency = 1.0f;

	CLight light;
	float timer;
	public bool activated {get;set;}

	public void OnStart()
	{
		light = gameObject.RequireComponent<CLight>();
		Random r = new Random();
		timer += (float)r.NextDouble()*2.0f;
        activated = true;
	}

	public void OnUpdate()
	{
		if (activated) {
            timer += FrameController.DT() * _frequency;
            light.SetAtt(((float)Math.Sin(timer) + 1.0f) * _amplitude);
		}
	}

    public void OnEditorUpdate()
    {
        OnUpdate();
    }
}
