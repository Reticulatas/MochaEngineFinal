using Microsoft.Xna.Framework;
using MochaInterface;using System;

public class BoobyTrapButton : StealthInteractableObject
{
    CMeshRenderer buttonMesh;
    Color buttonColor;

    CSound sound;
	
    GameObject boobyTrapControllerObj;
    BoobyTrapControllerScript boobyTrapScript;
	

    public override void OnStart()
    {
        base.OnStart();
		buttonMesh = gameObject.RequireComponent<CMeshRenderer>();
		
        boobyTrapControllerObj = GameObject.GetGameObjectByName("TSR_SeqScrambler");
        boobyTrapScript = GetScript<BoobyTrapControllerScript>(boobyTrapControllerObj);
            //buttonMesh.GetMaterial().SetColor(SharpMocha.CORNFLOWER_BLUE);

        sound = gameObject.RequireComponent<CSound>();
    }
    
    public override void OnUpdate()
    {
	
    }



    public override void OnStealthPlayerMouseEnter()
    {

    }
	public override void OnStealthPlayerMouseLeave()
    {

    }
	public override void OnStealthPlayerMouseInteract()
    {
        boobyTrapScript.TriggerTrap();

        sound.PlayIndependentEvent("BUTTON_RED.vente", false, 1);
    }
}