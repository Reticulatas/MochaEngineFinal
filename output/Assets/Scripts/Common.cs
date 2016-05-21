using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class Common
{
    public const String prefix = "";//"Level1Ed";
    public const String tagForInteractables = "interactable";
    public const String tagForStealthInteractables = "StealthInteractable";
    public const String tagForPinPad = "pinpad";
    public const String nameForCamLight = "CamLight";
    public const String nameForCamPointLight = "CamPointLight";
    public const String tagForCamInMap = "CamInMap";
    public const String nameForCamInMap = prefix + "CamInMap";
    public const String nameForCamInMapArea = prefix + "CamInMapArea";
    public const String tagForCamInWorld = "CamInWorld";
    public const String nameForCamInWorldParent = prefix + "CamInWorld";
    public const String nameForCamInWorld = prefix + "CamInWorldLens";
    public const String nameForStickyCamInMap = prefix + "StickyCamInMap";
    public const String nameForStickyCamInWorld = prefix + "StickyCamInWorldLens";
    public const String nameForRCCarCamInMap = prefix + "RCCarCamInMap";
    public const String nameForRCCarCamInWorld = prefix + "RCCarCamInWorld";
    public const String nameForLeftDoor = "leftdown";
    public const String nameForRightDoor = "rightup";
    public const String nameForLevelOrigin = prefix + "OriginPoint";
    public const String nameForHubCamTrigger = prefix + "HubCamTrigger";
    public const String nameForHubAmbientEnter = prefix + "GAmbientTriggerHubEnter";
    public const String nameForHubAmbientLeave = prefix + "GAmbientTriggerHubLeave";

    public const string nameForDigitalText = "TemplateConsoleText";
    public const string nameForFixedWidthText = "FixedWidthConsoleText";

    static GameObject pauseScreen = null;
    public static GameObject GetPauseScreen()
    {
        if (pauseScreen == null)
            pauseScreen = GameObject.GetGameObjectByName(prefix + "pauseScreen");
        return pauseScreen;
    }
    static PauseMenu pauseScreenScript = null;
    public static PauseMenu GetPauseScreenScript()
    {
        if (pauseScreenScript == null)
            pauseScreenScript = MochaScript.GetScript<PauseMenu>(GetPauseScreen());
        return pauseScreenScript;
    }
    public static bool isPaused = false;
    public static bool isOptions = false;

    static GameObject stealth_player = null;
    public static GameObject GetStealthPlayer()
    {
        if (stealth_player == null)
            stealth_player = GameObject.GetGameObjectByName(prefix + "PlayerBoxCollider");
        return stealth_player;
    }
    static FirstPersonPlayer stealthPlayerScript = null;
    public static FirstPersonPlayer GetStealthPlayerScript()
    {
        if (stealthPlayerScript == null)
            stealthPlayerScript = MochaScript.GetScript<FirstPersonPlayer>(GetStealthPlayer());
        return stealthPlayerScript;
    }
    static GameObject stealthPlayerMesh = null;
    public static GameObject GetStealthPlayerMesh()
    {
        if (stealthPlayerMesh == null)
            stealthPlayerMesh = GameObject.GetGameObjectByName(prefix + "Player");
        return stealthPlayerMesh;
    }

    static GameObject stealthPlayerCamera = null;
    public static GameObject GetStealthPlayerCamera()
    {
        if (stealthPlayerCamera == null)
            stealthPlayerCamera = GameObject.GetGameObjectByName(prefix + "Camera");
        return stealthPlayerCamera;
    }
    static FirstPersonCamera stealthPlayerCamScript = null;
    public static FirstPersonCamera GetStealthPlayerCameraScript()
    {
        if (stealthPlayerCamScript == null)
            stealthPlayerCamScript = MochaScript.GetScript<FirstPersonCamera>(GetStealthPlayerCamera());
        return stealthPlayerCamScript;
    }
    static GameObject stealthPlayerFadeScreen = null;
    public static GameObject GetStealthFadeScreen()
    {
        if (stealthPlayerFadeScreen == null)
            stealthPlayerFadeScreen = GameObject.GetGameObjectByName(prefix + "StealthFadeScreen");
        return stealthPlayerFadeScreen;
    }

    static GameObject stealthPlayerMouse = null;
    public static GameObject GetStealthPlayerMouse()
    {
        if (stealthPlayerMouse == null)
            stealthPlayerMouse = GameObject.GetGameObjectByName(prefix + "Mouse");
        return stealthPlayerMouse;
    }

    static GameObject surveillancePlayerCam = null;
    public static GameObject GetSurveillancePlayerCam()
    {
        if (surveillancePlayerCam == null)
            surveillancePlayerCam = GameObject.GetGameObjectByName(prefix + "Cam_Oculus");
        return surveillancePlayerCam;
    }
    static GameObject surveillancePlayerStaticCameraCam= null;
    public static GameObject GetSurveillancePlayerStaticCameraCam()
    {
        if (surveillancePlayerStaticCameraCam == null)
            surveillancePlayerStaticCameraCam = GameObject.GetGameObjectByName(prefix + "CameraCam");
        return surveillancePlayerStaticCameraCam;
    }
    static CameraCam cameraCamScript = null;
    public static CameraCam GetCameraCamScript()
    {
        if (cameraCamScript == null)
        {
            cameraCamScript = MochaScript.GetScript<CameraCam>(GetSurveillancePlayerStaticCameraCam());
        }
        return cameraCamScript;
    }

    static GameObject surveillancePlayerMouse = null;
    public static GameObject GetSurveillancePlayerMouse()
    {
        if (surveillancePlayerMouse == null)
            surveillancePlayerMouse = GameObject.GetGameObjectByName(prefix + "OcuMouse");
        return surveillancePlayerMouse;
    }
    static GameObject surveillancePlayerConsoleScreen = null;
    public static GameObject GetSurveillancePlayerConsoleScreen()
    {
        if (surveillancePlayerConsoleScreen == null)
            surveillancePlayerConsoleScreen = GameObject.GetGameObjectByName(prefix + "ConsoleScreen");
        return surveillancePlayerConsoleScreen;
    }
    static OcuConsoleScreen consoleScreenScript = null;
    public static OcuConsoleScreen GetConsoleScreenScript()
    {
        if (consoleScreenScript == null)
            consoleScreenScript = MochaScript.GetScript<OcuConsoleScreen>(GetSurveillancePlayerConsoleScreen());
        return consoleScreenScript;
    }
    static GameObject surveillancePlayerConsoleText = null;
    public static GameObject GetSurveillancePlayerConsoleText()
    {
        if (surveillancePlayerConsoleText == null)
            surveillancePlayerConsoleText = GameObject.GetGameObjectByName(prefix + "ConsoleText");
        return surveillancePlayerConsoleText;
    }
    static GameObject surveillancePlayerConsoleCam = null;
    public static GameObject GetConsoleCam()
    {
        if (surveillancePlayerConsoleCam == null)
            surveillancePlayerConsoleCam = GameObject.GetGameObjectByName(prefix + "ConsoleCam");
        return surveillancePlayerConsoleCam;
    }
    static GameObject surveillancePlayerConsoleBlinkingLight = null;
    public static GameObject GetConsoleBlinkingLight()
    {
        if (surveillancePlayerConsoleBlinkingLight == null)
            surveillancePlayerConsoleBlinkingLight = GameObject.GetGameObjectByName(prefix + "ConsoleBlinkingLight");
        return surveillancePlayerConsoleBlinkingLight;
    }
    
    static GameObject stickCam = null;
    public static GameObject GetStickyCamera()
    {
        return stickCam; // Will to set to valid gameObject if stickyCam is added to list in AddCameraObjectToList
    }
    static GameObject stickCamInMap = null;
    public static GameObject GetStickyCamInMap()
    {
        return stickCamInMap; // Will to set to valid gameObject if stickyCam is added to list in AddCameraObjectToList
    }
    static GameObject stickCamInMapArea = null;
    public static GameObject GetStickyCamInMapArea() // For the script
    {
        return stickCamInMapArea; // Will to set to valid gameObject if stickyCam is added to list in AddCameraObjectToList
    }
    static CameraInMap stickyCamScript = null;
    public static CameraInMap GetStickyCamScript()
    {
        if (stickyCamScript == null)
        {
            if (stickCamInMapArea != null)
            {
                stickyCamScript = MochaScript.GetScript<CameraInMap>(stickCamInMapArea);
            }
        }
        return stickyCamScript;
    }

    static GameObject camInMapSelector = null;
    public static GameObject GetCameraInMapSelector()
    {
        if (camInMapSelector == null)
            camInMapSelector = GameObject.GetGameObjectByName(prefix + "CamInMapSelector");
        return camInMapSelector;
    }
    static GameObject noiseStatic = null;
    public static GameObject GetNoiseStatic()
    {
        if (noiseStatic == null)
            noiseStatic = GameObject.GetGameObjectByName(prefix + "NoiseStatic");
        return noiseStatic;
    }
    static GameObject rcCar = null;
    public static GameObject GetRCCar()
    {
        if (rcCar == null)
            rcCar = GameObject.GetGameObjectByName(prefix + "RCCar", false);
        return rcCar;
    }
    static GameObject rcCarCam = null;
    public static GameObject GetRCCarCam()
    {
        return rcCarCam; // Will to set to valid gameObject if RCCarCam is added to list in AddCameraObjectToList
    }
    static GameObject rcCarCamInMap = null;
    public static GameObject GetRCCarCamInMap()
    {
        return rcCarCamInMap; // Will to set to valid gameObject if RCCarCam is added to list in AddCameraObjectToList
    }
    static GameObject rcCarJoystick = null;
    public static GameObject GetRCCarJoystick()
    {
        if (rcCarJoystick == null)
            rcCarJoystick = GameObject.GetGameObjectByName(prefix + "OcuJoystick", false);
        return rcCarJoystick;
    }

    private static int CompareWordsAfterLength(String lhs, String rhs, int lengthOfWord)
    {
        // return lhs.GetName().CompareTo(rhs.GetName());
        String lhsIndexStr = lhs.Substring(lengthOfWord);
        String rhsIndexStr = rhs.Substring(lengthOfWord);

        int lhsIndex = Convert.ToInt32(lhsIndexStr);
        int rhsIndex = Convert.ToInt32(rhsIndexStr);

        return lhsIndex - rhsIndex;
    }
    public static int CompareCamInMap(GameObject lhs, GameObject rhs)
    {
        return CompareWordsAfterLength(lhs.GetName(), rhs.GetName(), nameForCamInMap.Length);
    }
    public static int CompareCamInMapArea(GameObject lhs, GameObject rhs)
    {
        return CompareWordsAfterLength(lhs.GetName(), rhs.GetName(), nameForCamInMapArea.Length);
    }
    public static int CompareCamInWorld(GameObject lhs, GameObject rhs)
    {
        return CompareWordsAfterLength(lhs.GetName(), rhs.GetName(), nameForCamInWorld.Length);
    }

	static List<GameObject> camInMap = null;
	public static List<GameObject> GetCamsInMap()
    {
        if (camInMap == null)
        {
            camInMap = new List<GameObject>();
            /*
            // Populate the list based on CamInWorld objects
            GameObject mapScreen = GetMapScreen();
            camInMap = new List<GameObject>();
            int index = 0;
            foreach (GameObject go in GetCamsInWorld())
            {
                GameObject camIcon = ObjectFactory.getInstance().Instantiate("CamInMap");
                camIcon.SetName(nameForCamInMap + index.ToString());
                for (uint i = 0; i < camIcon.transform.GetNumberOfChildren(); ++i)
                {
                    if (camIcon.transform.GetChild(i).gameObject.GetName() == nameForCamInMapArea)
                    {
                        camIcon.transform.GetChild(i).gameObject.SetName(nameForCamInMapArea + index.ToString());
                    }
                }

                camIcon.transform.SetParent(mapScreen.transform.GetParent());

                camInMap.Add(camIcon);
                ++index;
            }
            */
        }
		return camInMap;
    }
    static List<GameObject> camInMapArea = null;
    public static List<GameObject> GetCamsInMapArea()
    {
        if (camInMapArea == null)
        {
            camInMapArea = new List<GameObject>();
            /*
            GetCamsInMap(); // Make sure to instantiate all the CamInMapArea first

            camInMapArea = new List<GameObject>();
            GameObjectVector camInMapVect = GameObject.GetGameObjectsWithTag(tagForCamInMap);
            for (int i = 0; i < camInMapVect.Count; ++i)
            {
                if (camInMapVect[i].GetName().StartsWith(Common.nameForCamInMapArea))
                    camInMapArea.Add(camInMapVect[i]);
            }
            camInMapArea.Sort( // Sort the physical CamInMapArea# by name, so [0] == CamInMapArea1 and so on, with StickyCamInMap being the last
                delegate(GameObject lhs, GameObject rhs)
                {
                    String lhsIndexStr = lhs.GetName().Substring(nameForCamInMapArea.Length);
                    String rhsIndexStr = rhs.GetName().Substring(nameForCamInMapArea.Length);

                    int lhsIndex = Convert.ToInt32(lhsIndexStr);
                    int rhsIndex = Convert.ToInt32(rhsIndexStr);

                    return lhsIndex - rhsIndex;
                });
            */
        }
        return camInMapArea;
    }
	static List<GameObject> camInWorld = null;
	public static List<GameObject> GetCamsInWorld()
    {
        if (camInWorld == null)
        {
            camInWorld = new List<GameObject>();
            /*
            GameObjectVector camInWorldVect = GameObject.GetGameObjectsWithTag(tagForCamInWorld);
            for (int i = 0; i < camInWorldVect.Count; ++i)
            {
                if (camInWorldVect[i].GetName().StartsWith(Common.nameForCamInWorld))
                    camInWorld.Add(camInWorldVect[i]);
            }
            camInWorld.Sort( // Sort the physical CamInWorld# by name, so [0] == CamInMap1 and so on, with StickyCamInMap being the last
            delegate(GameObject lhs, GameObject rhs)
            {
                // return lhs.GetName().CompareTo(rhs.GetName());
                String lhsIndexStr = lhs.GetName().Substring(nameForCamInWorld.Length);
                String rhsIndexStr = rhs.GetName().Substring(nameForCamInWorld.Length);

                int lhsIndex = Convert.ToInt32(lhsIndexStr);
                int rhsIndex = Convert.ToInt32(rhsIndexStr);

                return lhsIndex - rhsIndex;
            });
            */
        }
		return camInWorld;
    }

	public static GameObject GetChildByName(GameObject parent, string name, int substring = 0)
	{
		for (uint i = 0; i < parent.transform.GetNumberOfChildren(); ++i)
		{
			int goToIndex;
			if(substring != 0)
				goToIndex = substring;
			else
				goToIndex = parent.transform.GetChild(i).gameObject.GetName().Length;
			
			if (parent.transform.GetChild(i).gameObject.GetName().Substring(0,goToIndex) == name)
			{
				return parent.transform.GetChild(i).gameObject;
			}
		}
		
		return null;
	}
	
    // Adds to the camInMap, camInMapArea, and camInWorld lists
    public static void AddCameraObjectToList(GameObject inCamInWorld, out GameObject retCamInMap, out GameObject retCamInMapArea)
    {
        // Ensure they are initialized
        GetCamsInMap();
        GetCamsInMapArea();
        GetCamsInWorld();

        retCamInMap = null;
        retCamInMapArea = null;

        if (inCamInWorld.GetName().StartsWith(nameForCamInWorld))
        {
            // Sort and change name based on CamInWorld (parent)'s name
            String camIndexStr1 = inCamInWorld.transform.GetParent().gameObject.GetName();
            camIndexStr1 = camIndexStr1.Substring(nameForCamInWorldParent.Length);
            int index = Convert.ToInt32(camIndexStr1);

            inCamInWorld.SetName(nameForCamInWorld + camIndexStr1);
            //String camIndexStr = inCamInWorld.GetName().Substring(nameForCamInWorld.Length);
            //int index = Convert.ToInt32(camIndexStr);

            camInWorld.Add(inCamInWorld);
            // Sort the physical CamInWorld# by name, so [0] == CamInMap1 and so on, with StickyCamInMap being the last
            camInWorld.Sort(CompareCamInWorld);

            retCamInMapArea = null;

            retCamInMap = ObjectFactory.getInstance().Instantiate("CamInMap");
            retCamInMap.SetName(nameForCamInMap + index.ToString());
            for (uint i = 0; i < retCamInMap.transform.GetNumberOfChildren(); ++i)
            {
                if (retCamInMap.transform.GetChild(i).gameObject.GetName() == nameForCamInMapArea)
                {
                    retCamInMapArea = retCamInMap.transform.GetChild(i).gameObject;
                    retCamInMapArea.SetName(nameForCamInMapArea + index.ToString());
                    camInMapArea.Add(retCamInMapArea);
                    break;
                }
            }
            retCamInMap.transform.SetParent(GetMapScreen().transform.GetParent());
            camInMap.Add(retCamInMap);
            camInMap.Sort(CompareCamInMap);

            camInMapArea.Sort(CompareCamInMapArea);
        }
        else
        {
            Logger.Log("Adding camera to list: name not normal: " + inCamInWorld.GetName());

            if (inCamInWorld.GetName() == nameForStickyCamInWorld)
            {
                stickCam = inCamInWorld;
                retCamInMap = ObjectFactory.getInstance().Instantiate("CamInMap");
                retCamInMap.SetName(nameForStickyCamInMap);
                for (uint i = 0; i < retCamInMap.transform.GetNumberOfChildren(); ++i)
                {
                    if (retCamInMap.transform.GetChild(i).gameObject.GetName() == nameForCamInMapArea)
                    {
                        retCamInMapArea = retCamInMap.transform.GetChild(i).gameObject;
                        retCamInMapArea.SetName(nameForStickyCamInMap + "Area");
                        stickCamInMapArea = retCamInMapArea;
                        break;
                    }
                }
                retCamInMap.transform.SetParent(GetMapScreen().transform.GetParent());

                stickCamInMap = retCamInMap;
            }
            else if (inCamInWorld.GetName() == nameForRCCarCamInWorld)
            {
                rcCarCam = inCamInWorld;
                retCamInMap = ObjectFactory.getInstance().Instantiate("CamInMap");
                retCamInMap.SetName(nameForRCCarCamInMap);
                for (uint i = 0; i < retCamInMap.transform.GetNumberOfChildren(); ++i)
                {
                    if (retCamInMap.transform.GetChild(i).gameObject.GetName() == nameForCamInMapArea)
                    {
                        retCamInMapArea = retCamInMap.transform.GetChild(i).gameObject;
                        retCamInMapArea.SetName(nameForRCCarCamInMap + "Area");
                        break;
                    }
                }
                retCamInMap.transform.SetParent(GetMapScreen().transform.GetParent());

                rcCarCamInMap = retCamInMap;
            }
        }
    }
    // Remove from the camInMap, camInMapArea, and camInWorld lists
    public static void RemoveCameraObjectFromList(GameObject inCamInWorld)
    {
        // Ensure they are initialized
        GetCamsInMap();
        GetCamsInMapArea();
        GetCamsInWorld();

        if (inCamInWorld.GetName().StartsWith(nameForCamInWorld))
        {
            String camIndexStr = inCamInWorld.GetName().Substring(nameForCamInWorld.Length);
            int index = Convert.ToInt32(camIndexStr);

            if (!camInWorld.Remove(inCamInWorld))
                Logger.Log("RemoveCameraObjectFromList: unable to find " + inCamInWorld.GetName());
            else
                camInWorld.Sort(CompareCamInWorld);

            String inMapName = nameForCamInMap + index.ToString();
            String inMapAreaName = nameForCamInMapArea + index.ToString();
            GameObject camInMapObj = null;
            foreach (GameObject gObj in camInMap)
            {
                if (gObj.GetName() == inMapName)
                {
                    if (!camInMap.Remove(gObj))
                        Logger.Log("RemoveCameraObjectFromList: unable to find " + inMapName);
                    else
                    {
                        camInMap.Sort(CompareCamInMap);
                        camInMapObj = gObj;
                    }
                    break;
                }
            }
            GameObject camInMapAreaObj = null;
            foreach (GameObject gObj in camInMapArea)
            {
                if (gObj.GetName() == inMapAreaName)
                {
                    if (!camInMapArea.Remove(gObj))
                        Logger.Log("RemoveCameraObjectFromList: unable to find " + inMapAreaName);
                    else
                    {
                        camInMapArea.Sort(CompareCamInMapArea);
                        camInMapAreaObj = gObj;
                    }
                    break;
                }
            }

            if (camInMapAreaObj != null)
            {
                if (CameraInMap.currEnabledCamInMap != null) // Make sure currEnabled is forgotten
                {
                    CameraInMap.currEnabledCamInMap = null;
                }
            }
            if (camInMapObj != null)
            {
                camInMapObj.Destroy();
            }
        }
        else
        {
            Logger.Log("Remove camera from list: name not normal: " + inCamInWorld.GetName() + ", so do not affect list");
        }

        //Logger.Log("Cams in map now: " + camInMap.Count.ToString());
        //Logger.Log("Cams in map world now: " + camInWorld.Count.ToString());
        //Logger.Log("Cams in map area now: " + camInMapArea.Count.ToString());
    }
	
	static GameObject cameraScreen = null;
    public static GameObject GetCameraScreen()
    {
        if (cameraScreen == null)
            cameraScreen = GameObject.GetGameObjectByName(prefix + "CameraScreen");
        return cameraScreen;
    }
    static GameObject mapScreen = null;
    public static GameObject GetMapScreen()
    {
        if (mapScreen == null)
            mapScreen = GameObject.GetGameObjectByName(prefix + "MapScreen");
        return mapScreen;
    }

    /*** For Menu Stuff ***/
    static GameObject pauseOptionsScreen = null;
    public static GameObject GetPauseOptionsScreen()
    {
        if (pauseOptionsScreen == null)
        {
            pauseOptionsScreen = GameObject.GetGameObjectByName(prefix + "optionsScreen");
        }
        return pauseOptionsScreen;
    }
    static CDiffuseRenderer fullscreenX = null;
    public static CDiffuseRenderer GetMenuFullscreenX()
    {
        if (fullscreenX == null)
        {
            GameObject xObj = GameObject.GetGameObjectByName(prefix + "FullscreenX");
            fullscreenX = xObj.RequireComponent<CDiffuseRenderer>();
        }
        return fullscreenX;
    }
    static CDiffuseRenderer muteX = null;
    public static CDiffuseRenderer GetMenuMuteX()
    {
        if (muteX == null)
        {
            GameObject xObj = GameObject.GetGameObjectByName(prefix + "MuteX");
            muteX = xObj.RequireComponent<CDiffuseRenderer>();
        }
        return muteX;
    }
    static CFontRenderer volumeFont = null;
    public static CFontRenderer GetMenuVolumeFont()
    {
        if (volumeFont == null)
        {
            GameObject obj = GameObject.GetGameObjectByName(prefix + "VolumeText");
            volumeFont = obj.RequireComponent<CFontRenderer>();
        }
        return volumeFont;
    }


    public static bool TriggeredUp()
    {
        if (Input.GetTriggered(0, "ArrowUp") != 0.0f)
            return true;
        if (Input.GetTriggered(0, "LThumbY") != 0.0f)
        {
            return Input.GetValue((char)0, "LThumbY") > 0.0f;
        }
        return false;
    }
    public static bool TriggeredDown()
    {
        if (Input.GetTriggered(0, "ArrowDown") != 0.0f)
            return true;
        if (Input.GetTriggered(0, "LThumbY") != 0.0f)
        {
            return Input.GetValue((char)0, "LThumbY") < 0.0f;
        }
        return false;
    }
    public static bool TriggeredLeft()
    {
        if (Input.GetTriggered(0, "ArrowLeft") != 0.0f)
            return true;
        if (Input.GetTriggered(0, "LThumbX") != 0.0f)
        {
            return Input.GetValue((char)0, "LThumbX") < 0.0f;
        }
        return false;
    }
    public static bool TriggeredRight()
    {
        if (Input.GetTriggered(0, "ArrowRight") != 0.0f)
            return true;
        if (Input.GetTriggered(0, "LThumbX") != 0.0f)
        {
            return Input.GetValue((char)0, "LThumbX") > 0.0f;
        }
        return false;
    }

    public static void SetOpacityToAllUI(float opacity, GameObject obj)
    {
        SetOpacityToGameObject(opacity, obj);

        for (uint i = 0; i < obj.transform.GetNumberOfChildren(); ++i)
        {
            SetOpacityToAllUI(opacity, obj.transform.GetChild(i).gameObject);
        }
    }
    public static void SetOpacityToGameObject(float opacity, GameObject obj)
    {
        CFontRenderer font = obj.GetComponent<CFontRenderer>();
        if (font != null)
        {
            font.mColor.a = opacity;
            if (opacity <= 0.0f)
                font.setEnabled(false);
            else
                font.setEnabled(true);
        }
        CDiffuseRenderer diffuse = obj.GetComponent<CDiffuseRenderer>();
        if (diffuse != null)
        {
            Color clr = diffuse.GetMaterial().GetColor();
            clr.a = opacity;
            diffuse.GetMaterial().SetColor(clr);

            if (opacity <= 0.0f)
                diffuse.setEnabled(false);
            else
                diffuse.setEnabled(true);
        }
    }

    public static String ConvertColorToStringTag(Color clr)
    {
        String colorTag = "<#" + clr.r.ToString() + "," + clr.g.ToString() + "," +
            clr.b.ToString() + "," + clr.a.ToString() + "#>";
        return colorTag;
    }

    public static void Reset()
    {
        isPaused = false;
        isOptions = false;

        pauseScreen = null;
        pauseScreenScript = null;
        stealth_player = null;
        stealthPlayerScript = null;
        stealthPlayerMesh = null;
        stealthPlayerCamera = null;
        stealthPlayerCamScript = null;
        stealthPlayerMouse = null;
        surveillancePlayerCam = null;
        surveillancePlayerStaticCameraCam = null;
        cameraCamScript = null;
        surveillancePlayerMouse = null;
        surveillancePlayerConsoleScreen = null;
        consoleScreenScript = null;
        surveillancePlayerConsoleText = null;
        surveillancePlayerConsoleCam = null;
        surveillancePlayerConsoleBlinkingLight = null;
        stealthPlayerFadeScreen = null;
        stickCam = null;
        stickCamInMap = null;
        stickCamInMapArea = null;
        stickyCamScript = null;
        camInMapSelector = null;
        noiseStatic = null;
        rcCar = null;
        rcCarCam = null;
        rcCarCamInMap = null;
        rcCarJoystick = null;

        camInMap = null;
        camInMapArea = null;
        camInWorld = null;

        cameraScreen = null;
        mapScreen = null;

        pauseOptionsScreen = null;
        fullscreenX = null;
        muteX = null;
        volumeFont = null;
    }
}