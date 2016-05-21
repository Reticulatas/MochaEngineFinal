using Microsoft.Xna.Framework;
using MochaInterface;using System;

public class GenericButton : StealthInteractableObject
{
    CMeshRenderer buttonMesh;
    Color buttonColor;

    bool isButtonActive = true;
    float buttonTimer = 0.0f;

    CSound sound;

    int keyCode;

    GameObject seqControllerObj;
    AccessSequenceControllerScript seqController;

    public override void OnStart()
    {
        base.OnStart();
		buttonMesh = gameObject.RequireComponent<CMeshRenderer>();
        buttonColor = buttonMesh.GetMaterial().GetColor();

        sound = gameObject.RequireComponent<CSound>();

        if (gameObject.GetName()[0] == 'r')
            keyCode = 1;
        else if (gameObject.GetName()[0] == 'g')
            keyCode = 2;
        else
            keyCode = 3;

        seqControllerObj = GameObject.GetGameObjectByName("SequenceScrambler");
        seqController = GetScript<AccessSequenceControllerScript>(seqControllerObj);
            //buttonMesh.GetMaterial().SetColor(SharpMocha.CORNFLOWER_BLUE);
    }
    
    public override void OnUpdate()
    {
	    
    }

    public void ResetButton()
    {
        isButtonActive = true;
        buttonMesh.GetMaterial().SetColor(buttonColor);
        buttonMesh.mEmissiveColor = SharpMocha.BLACK;
    }

    public override void OnStealthPlayerMouseEnter()
    {
        if (isButtonActive)
            buttonMesh.mEmissiveColor = buttonColor; // Brighter
    }
	public override void OnStealthPlayerMouseLeave()
    {
        buttonMesh.mEmissiveColor = SharpMocha.BLACK;
    }
	public override void OnStealthPlayerMouseInteract()
    {
        isButtonActive = false;
        buttonMesh.GetMaterial().SetColor(buttonColor.Mul(0.3f));
        buttonMesh.mEmissiveColor = SharpMocha.BLACK;

        bool isValidCode = seqController.AddPlayerInput(keyCode, gameObject);

        String buttonSoundName = isValidCode ? "PINPAD_INPUT.vente" : "BUTTON_RED.vente";
        sound.PlayIndependentEvent(buttonSoundName, false, 1);
    }
}