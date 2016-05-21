using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class MonsterEncounter1 : MochaScript
{
    float origRot;
    Random random;

	public void OnStart()
	{
        origRot = gameObject.transform.rotation.Angles.z;
        random = new Random();
	}

	public void OnUpdate()
	{
    }

    public void Advance()
    {
		Rotate();
        gameObject.transform.Translate(0, 0, -10.0f);
    }
	
	public void Rotate()
	{
		gameObject.transform.rotation.Angles.z = origRot + MathHelper.ToDegrees((float)random.NextDouble() * 2.5f - 1.25f);
	}
}