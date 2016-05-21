using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class DeathPanel : TriggerAreaBaseClass
{
    int mPanelNumber = -1; // default -1 means not safe
    int mPanelID = 0;
    DeathPanelController controllerScript;
    CForwardRenderer screen;
    CFontRenderer numberText;

    const float alphaValue = 0.16f;
    Color defaultColor;
    Color greenColor;
    Color redColor;

    bool isInit = false;

    void Init()
    {
        if (!isInit)
        {
            isInit = true;

            defaultColor = SharpMocha.WHITE;
            defaultColor.a = alphaValue;
            greenColor = SharpMocha.GREEN;
            greenColor.a = alphaValue;
            redColor = SharpMocha.RED;
            redColor.a = alphaValue;

            GameObject controller = GameObject.GetGameObjectByName(Common.prefix + "PuzzleRoom3", false);
            if (controller != null)
                controllerScript = GetScript<DeathPanelController>(controller);
            else
                Logger.Log("Unable to find DeathPanelController");

            GameObject obj = Common.GetChildByName(gameObject, Common.prefix + "DeathPanelScreen");
            screen = obj.RequireComponent<CForwardRenderer>();
            obj = Common.GetChildByName(gameObject, Common.prefix + "DeathPanelText");
            numberText = obj.RequireComponent<CFontRenderer>();
        }
    }
    public override void OnStart()
    {
        base.OnStart();

        Init();
    }

    public void SetNumber(int num, int id)
    {
        Init();

        mPanelNumber = num; // Which panel in the sequence to be stepped on
        mPanelID = id;

        if (mPanelNumber < 0)
        {
            numberText.setEnabled(false); // no number for unsafe panels

            screen.GetMaterial().SetColor(redColor);
        }
        else
        {
            numberText.setEnabled(true);
            int number = mPanelNumber + 1;
            numberText.mText = number.ToString();

            screen.GetMaterial().SetColor(defaultColor);
        }
    }
    public void SetGreen()
    {
        screen.GetMaterial().SetColor(greenColor);
    }
    public void SetDisable()
    {
        isActive = false;
        screen.setEnabled(false);
        numberText.setEnabled(false);
    }

    public void OnUpdate()
    {
        
    }

    public override void OnAreaEnter()
    {
        if (isactive)
        {
            if (controllerScript != null)
                controllerScript.OnPanelTriggered(mPanelID);

            isactive = false;
            // gameObject.Destroy();
        }
    }
    public override void OnArea()
    {

    }
}
