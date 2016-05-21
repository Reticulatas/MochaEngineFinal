using MochaInterface;
using Microsoft.Xna.Framework;
using System;

public class MonkeyInput
{
	public static bool mx_orient = false;
	public static bool mz_orient = false;
	public static bool l_orient = false;
	
	public static float GetMovementXFloat()
	{
		if (MMath.GetRandomInt(0, 500) == 10)
            mx_orient = !mx_orient;

        if (mx_orient == false)
			return MMath.GetRandomLimitedFloat(-1.0f,0.0f);
		return MMath.GetRandomLimitedFloat(0.0f,1.0f);
	}
	public static float GetMovementZFloat()
	{
		if (MMath.GetRandomInt(0, 500) == 5)
            mz_orient = !mz_orient;

        if (mz_orient == false)
			return MMath.GetRandomLimitedFloat(-1.0f,0.0f);
		return MMath.GetRandomLimitedFloat(0.0f,1.0f);
	}
	
	public static float GetLookFloat()
	{
		return 0.0f;
		/*
		if (MMath.GetRandomInt(0, 6) == 5)
			l_orient = !l_orient;
		
		if (l_orient == false)
			return MMath.GetRandomLimitedFloat(-1.0f,0.0f);
		return MMath.GetRandomLimitedFloat(0.0f,1.0f);*/
	}
}