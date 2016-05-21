using MochaInterface;
using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;

public class MonsterHelper
{
	List<CLight> clights = new List<CLight>();
	GameObject stealth_player;
	
	public MonsterHelper()
	{
		stealth_player = Common.GetStealthPlayer();
		
		foreach (GameObject obj in GameObject.GetGameObjectsWithTag("M3Light"))
        {
            clights.Add(obj.RequireComponent<CLight>());
        }
        Logger.Log("Found " + clights.Count + " CLights to use for Monster");
	}
	
	#region HELPERS
 
	public bool CheckLights(GameObject gameObject)
    {
        foreach (CLight cl in clights)
        {
			if (!cl.isEnabled())
				continue;
            Vector3 vec_to_light = (Vector3)gameObject.transform.position - (Vector3)cl.gameObject.transform.position;
            float dist_to_light = cl.gameObject.transform.position.Distance(gameObject.transform.position);

            if (cl.mLightType == CLight.LIGHTTYPE.POINT)
            {
                //point light check
                if (dist_to_light < cl.mRange *0.99f)
                {
                    return true;
                }
            }
            if (cl.mLightType == CLight.LIGHTTYPE.SPOTLIGHT)
            {
                if (dist_to_light < cl.mRange)
                {
                    float angtol = 
                        MMath.GetAngleBetweenVectors(-(Vector3)cl.gameObject.transform.GetUpVector(), vec_to_light);
                    angtol = MathHelper.ToDegrees(angtol);
                    if (angtol < (cl.mOuterAngle)*1.1f)
                        return true;
                }
            }
        }

        return false;
    }


    #endregion
}