using Microsoft.Xna.Framework;
using MochaInterface;
using System;

public class InteractableObjectBaseClass : MochaScript
{
    static CFontRenderer subtitle = null;
    public const String subtitleName = Common.prefix + "Subtitles";
    public CFontRenderer GetSubtitleFont()
    {
        if (subtitle == null)
        {
            subtitle = GameObject.GetGameObjectByName(subtitleName).RequireComponent<CFontRenderer>();
            subtitle.setEnabled(false);
        }
        return subtitle;
    }

    public virtual void OnTriggered() { }
    public virtual void OnHeldMouse(float mouseMoveX, float mouseMoveY) { }
    public virtual void OnReleased() { }
    public virtual void OnMouseOver() { }
    public virtual void OnMouseOverOff() { }
}
