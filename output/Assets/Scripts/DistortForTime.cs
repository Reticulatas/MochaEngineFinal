using MochaInterface;
using Microsoft.Xna.Framework;
using System;

public class DistortForTime : MochaScript
{
  public float MaxTime {get; set;}
  public bool Activated {get; set;}

  float _blockthreshold = 0.0f;
  float _linethreshold = 0.0f;
  public float BlockThreshold { get { return _blockthreshold; } set { _blockthreshold = value; } }
  public float LineThreshold { get { return _linethreshold; } set { _linethreshold = value; } }
  float currTime;
  CSound sound;
  CArtifactFX fx;
  
  public void OnStart()
  {
    sound = gameObject.RequireComponent<CSound>();
    fx = Common.GetStealthPlayerCamera().RequireComponent<CArtifactFX>();
    //sound.PlayIndependentEvent("CREATURE_CLOSE.vente", true, 1);
	Activated = false;
	currTime = MaxTime;
    fx.mBlockThreshold = 0.0f;
    fx.mLineThreshold = 0.0f;
  }
  
  public void OnUpdate()
  {
      if (currTime < 0.0f || !Activated)
      {
          //fx.mBlockThreshold = 0.0f;
          //fx.mLineThreshold = 0.0f;

          return;
      }
      else
      {
          currTime -= FrameController.DT();

          fx.mBlockThreshold = (currTime / MaxTime) * _blockthreshold;
          fx.mLineThreshold = (currTime / MaxTime) * _linethreshold;
      }
  }
  
  public void Trigger()
  {
	currTime = MaxTime;
	Activated = true;
  }
}