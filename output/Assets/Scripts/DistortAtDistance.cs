using MochaInterface;
using Microsoft.Xna.Framework;
using System;

public class DistortAtDistance : MochaScript
{
  public float Distance {get; set;}
  CSound sound;
  CArtifactFX fx;
  
  public void OnStart()
  {
    sound = gameObject.RequireComponent<CSound>();
    fx = Common.GetStealthPlayerCamera().RequireComponent<CArtifactFX>();
    sound.PlayIndependentEvent("CREATURE_CLOSE.vente", true, 1);
  }
  
  public void OnUpdate()
  {
    float MAXDIST = Distance;
    float dist = gameObject.transform.position.Distance(fx.gameObject.transform.position);
    dist = MAXDIST - MMath.Clamp(dist, 0.0f, MAXDIST);
    fx.mBlockThreshold = dist/MAXDIST;
    fx.mLineThreshold = (dist/MAXDIST)*2.0f;
  }
}