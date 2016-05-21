using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class MinimapTracker : MochaScript 
{
	Vector3 mapOrigin = new Vector3(119F, 20F, 0);
    public Vector3 mapOriginVec3 { get { return mapOrigin; }
        set { mapOrigin = (Vector3)value; } }
	Vector3 mapBounds = new Vector3(187F, 95F, 0);
    public Vector3 mapBoundsVec3 { get { return mapBounds; }
        set { mapBounds = (Vector3)value; } }
	Vector3 mapOffset = new Vector3(0F, 0F, 0);
    public Vector3 mapOffsetVec3 { get { return mapOffset; }
        set { mapOffset = (Vector3)value; } }
	Vector3 mapSkew = new Vector3(1F, 1F, 0);
    public Vector3 mapSkewVec3 { get { return mapSkew; }
        set { mapSkew = (Vector3)value; } }

    Vector3 unscaledOffset;

    public int mLevelNumber { get; set; }
	
    GameObject thePlayer;
    GameObject thePlayerBlip;

	// Use this for initialization
	public void OnStart () 
    {
        thePlayer = Common.GetStealthPlayer();
        thePlayerBlip = GameObject.GetGameObjectByName("StealthPlayerBlip");

        unscaledOffset = Vector3.Zero;

        UpdateNewMinimapBounds();
	}
    public void UpdateNewMinimapBounds()
    {
        switch (mLevelNumber)
        {
            case 0: // nothing 
                break;
            case 1: // Level tutorial
                mapOriginVec3 = new Vector3(130.20f, 18.4f, 0.0f);// (112.50f, 19.0f, 0.0f);
                mapBoundsVec3 = new Vector3(224.0f, 124.5f, 0.0f);// (174.0f, 95.0f, 0.0f);
                mapOffsetVec3 = new Vector3(0.0f, 0.0f, 0.0f);// (0.06f, 0.0f, 0.0f);
                mapSkewVec3 =   new Vector3(1.0f, 1.0f, 0.0f);// (0.84f, 1.4f, 0.0f);
                break;
            case 2: // Josh'
                mapOriginVec3 = new Vector3(9.60f, -51.30f, 0.0f);
                mapBoundsVec3 = new Vector3(230.0f, 139.0f, 0.0f); 
                mapOffsetVec3 = new Vector3(0.0f, 0.0f, 0.0f);   
                mapSkewVec3 =   new Vector3(1.0f, 1.0f, 0.0f);   
                break;
            case 3: //chris 
                mapOriginVec3 = new Vector3(54.0f, 29.0f, 0.0f);
                mapBoundsVec3 = new Vector3(225.0f, 177.0f, 0.0f); 
                mapOffsetVec3 = new Vector3(0.0f, 0.0f, 0.0f);   
                mapSkewVec3 =   new Vector3(1.0f, 1.0f, 0.0f);   
                break;
            case 4: //fuller m4
                mapOriginVec3 = new Vector3(-43.0f, -30.0f, 0.0f);
                mapBoundsVec3 = new Vector3(540.0f, 400.0f, 0.0f); 
                mapOffsetVec3 = new Vector3(0.0f, 0.0f, 0.0f);   
                mapSkewVec3 =   new Vector3(1.0f, 1.0f, 0.0f);   
                break;
            case 5:
                mapOriginVec3 = new Vector3(23.50f, 24.70f, 0.0f);
                mapBoundsVec3 = new Vector3(131.0f, 79.6f, 0.0f); 
                mapOffsetVec3 = new Vector3(0.0f, 0.0f, 0.0f);   
                mapSkewVec3 =   new Vector3(1.0f, 1.0f, 0.0f);  
                break;
        }

        string numberStr = mLevelNumber.ToString();
        GameObject levelMeasure = GameObject.GetGameObjectByName(Common.nameForLevelOrigin + numberStr);
        if (levelMeasure != null)
        {
            Vector3 offset = levelMeasure.transform.position;
            unscaledOffset = offset;
            unscaledOffset.Y = unscaledOffset.Z;
            unscaledOffset.Z = 0.0f;
        }
    }
	
	// Update is called once per frame
	public void OnUpdate () 
    {
		WorldToMinimap(thePlayer, thePlayerBlip, 0.2F);
        if (Common.GetRCCar() != null && Common.GetRCCarCamInMap() != null)
        {
            WorldToMinimap(Common.GetRCCar(), Common.GetRCCarCamInMap(), 0.1F);
        }
        if (Common.GetStickyCamera() != null && Common.GetStickyCamInMap() != null &&
            Common.GetStickyCamScript().isActive)
        {
            WorldToMinimap(Common.GetStickyCamera(), Common.GetStickyCamInMap(), 0.1F);
        }

		for(int i = 0; i < Common.GetCamsInMap().Count; i++)
		{
            WorldToMinimap(Common.GetCamsInWorld()[i], Common.GetCamsInMap()[i], 0.1F);
		}
	}
	
	public void WorldToMinimap (GameObject translateThis, GameObject translateBlip, float zOffset)
	{
		Vector3 translatedPosition_2D = translateThis.transform.position;
        translatedPosition_2D.Y = translateThis.transform.position.z;

        Vector3 fromOrigin_WorldPosition = translatedPosition_2D - (mapOriginVec3 + unscaledOffset);
		
		Vector3 fromOrigin_MinimapPosition = fromOrigin_WorldPosition;
		fromOrigin_MinimapPosition.X /= mapBoundsVec3.X * -1;
		fromOrigin_MinimapPosition.Y /= mapBoundsVec3.Y;
		
		Vector3 appliedAspectRatio_MinimapPosition = fromOrigin_MinimapPosition;
		// if(mapBoundsVec3.X/mapBoundsVec3.Y > 1)
		// 	appliedAspectRatio_MinimapPosition.Y /= (mapBoundsVec3.X/mapBoundsVec3.Y);
		// else
		// 	appliedAspectRatio_MinimapPosition.X /= (mapBoundsVec3.Y/mapBoundsVec3.X);
		
		Vector3 addOffset_MiniMapPosition = appliedAspectRatio_MinimapPosition;
        addOffset_MiniMapPosition.X += mapOffsetVec3.X;
        addOffset_MiniMapPosition.Y += mapOffsetVec3.Y;
		
		Vector3 addSkew_MiniMapPosition = addOffset_MiniMapPosition;
        addSkew_MiniMapPosition.X *= mapSkewVec3.X;
        addSkew_MiniMapPosition.Y *= mapSkewVec3.Y;
		
		Vector3 final_MiniMapPosition = addSkew_MiniMapPosition;
        final_MiniMapPosition.Z = zOffset;
		
        translateBlip.transform.SetPosition(final_MiniMapPosition, CTransform.TransformSpace.LOCAL);
        translateBlip.transform.Rotate(0, 0, translateThis.transform.GetConstrainedRotation().y, CTransform.TransformSpace.LOCAL);
	}
}
