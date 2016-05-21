using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class SinWaveY : MochaScript
{
    public float amplitude { get { return _amplitude; } set { _amplitude = value; } }
    float _amplitude = 1.0f;

    public float frequency { get { return _frequency; } set { _frequency = value; } }
    float _frequency = 1.0f;

    float currStep;
    float initialY;

	public void OnStart()
    {
        initialY = gameObject.transform.position.y;
    }

	public void OnUpdate()
    {
        currStep += 0.1f * _frequency;
        gameObject.transform.SetPositionY(initialY + (float)Math.Sin(currStep) * _amplitude);
    }
	
}