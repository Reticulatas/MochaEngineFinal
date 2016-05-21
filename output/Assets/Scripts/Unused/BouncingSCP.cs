using MochaInterface;
using Microsoft.Xna.Framework;
using System;

public class BouncingSCP : MochaScript
{
  float speedmod = 1.0f;
  float timer;
  CZone zone;
  Random rand;
  Vector3 speed;
  Vector3? target;
  Vector3 beginPos;
  CSound sound;
  CArtifactFX fx;
  
  public void OnStart()
  {
    zone = GameObject.GetGameObjectByName("scp_zone").RequireComponent<CZone>();
    rand = new Random();
    target = null;
    speed = new Vector3(.3f,.8f,-.5f);
    sound = gameObject.RequireComponent<CSound>();
    fx = GameObject.GetGameObjectByName("Camera").RequireComponent<CArtifactFX>();
    sound.PlayIndependentEvent("CREATURE_CLOSE.vente", true, 1);
    
  }
  
  public Vector3 GetBoundsOnSphere()
  {
    float x = MMath.GetRandomFloat()*zone.width + zone.GetLeftBound();
    float y = MMath.GetRandomFloat()*zone.height + zone.GetBottomBound();
    float z = MMath.GetRandomFloat()*zone.depth + zone.GetFrontBound();
    
    x = MMath.Clamp(x, zone.GetLeftBound(), zone.GetRightBound());
    y = MMath.Clamp(y, zone.GetBottomBound(), zone.GetTopBound());
    z = MMath.Clamp(z,zone.GetFrontBound(), zone.GetBackBound());
    return new Vector3(x,y,z);
  }
  
  public void OnUpdate()
  {
    timer += FrameController.DT();
    float scaletimer = timer + FrameController.DT();
    //slowly modulate speed
    speedmod = (float)Math.Sin(scaletimer*0.7f) + 2.0f;
    //gameObject.transform.scale = new Vector3(speedmod*0.1f);
    
    float timermod = Math.Abs((float)Math.Sin(scaletimer*0.1f))+8.0f;
    timer += FrameController.DT() * timermod;
    
    if (!target.HasValue)
      target = GetBoundsOnSphere();
    else
    {
      gameObject.transform.SetPosition(MMath.LerpVector(beginPos, target.Value, timer));
      if (timer > 1.0f)
      {
        beginPos = gameObject.transform.position;
        timer = 0;
        target = null;
        sound.PlayIndependentEvent("RC_CAR_MOVE.vente", false, 1);
      }
    }
    
    const float MAXDIST = 7.0f;
    float dist = gameObject.transform.position.Distance(fx.gameObject.transform.position);
    dist = MAXDIST - MMath.Clamp(dist, 0.0f, MAXDIST);
    fx.mBlockThreshold = dist/MAXDIST;
    fx.mLineThreshold = (dist/MAXDIST)*2.0f;
    
    /*
    if (!zone.IsPointWithin(gameObject.transform.position))
    {
      float rand_var_x = 1.0f;//(((float)rand.NextDouble()-0.5f)*0.9f) + 1.0f;
      float rand_var_y = 1.0f;//(((float)rand.NextDouble()-0.5f)*0.9f) + 1.0f;
      float rand_var_z = 1.0f;//(((float)rand.NextDouble()-0.5f)*0.9f) + 1.0f;
      Vector3 newSpeed = new Vector3();
      newSpeed.X = -speed.X * rand_var_x;
      newSpeed.Z = -speed.Z * rand_var_z;
      newSpeed.Y = -speed.Y * rand_var_y;
      
      newSpeed.Normalize();
      speed.Normalize();
      
      speed = newSpeed;
      
      gameObject.transform.Translate(speed);
    }
    
    if (speed.X > 0)
      speed.X = MMath.Clamp(speed.X, 0.1f, 0.7f);
    else
      speed.X = MMath.Clamp(speed.X, -0.1f, -0.7f);
    if (speed.Y > 0)
      speed.Y = MMath.Clamp(speed.Y, 0.1f, 0.7f);
    else
      speed.Y = MMath.Clamp(speed.Y, -0.1f, -0.7f);
    if (speed.Z > 0)
      speed.Z = MMath.Clamp(speed.Z, 0.1f, 0.7f);
    else
      speed.Z = MMath.Clamp(speed.Z, -0.1f, -0.7f);
    
    
    gameObject.transform.Translate(speed*speedmod);
    */
  }
}