using MochaInterface;
using Microsoft.Xna.Framework;
using System;

public class SpinScanner : MochaScript
{	
	CTransform trans_;
	CTransform upperTrans_;
	CTransform lowerTrans_;
	
	CSound leftSound_;
	CSound rightSound_;
	
    StateMachineController smc;
    State startup, riseup, lowerdown, end;
	
	float transitionTime_;
	
	float startupTimer_;
	float startupTime_;
	float shutdownTime_;
	
	float riseupTimer_;
	float riseupTime_;
	
	float revInitTime_;
	float revFinalTime_;
	float revTime_;
	float revInc_;
	
	public void OnStart()
	{	
		trans_ = gameObject.RequireComponent<CTransform>();
		upperTrans_ = GameObject.GetGameObjectByName("spinner_upper").RequireComponent<CTransform>();
		lowerTrans_ = GameObject.GetGameObjectByName("spinner_lower").RequireComponent<CTransform>();
		trans_.SetPositionY(lowerTrans_.position.y);
		
		leftSound_ = GameObject.GetGameObjectByName("spinner_left").RequireComponent<CSound>();
		rightSound_ = GameObject.GetGameObjectByName("spinner_right").RequireComponent<CSound>();
		
		smc = new StateMachineController();
        startup = new State(smc, null, State_startup_Update, null);
        riseup = new State(smc, State_riseup_OnStart, State_riseup_Update, null);
        lowerdown = new State(smc, State_lowerdown_OnStart, State_lowerdown_Update, null);
        end = new State(smc, null, State_end_Update, null);
        smc.SetState(startup);
        
        transitionTime_ = 3;
        
		startupTimer_ = 0;
		startupTime_ = 5;
		shutdownTime_ = 7;
		
		riseupTimer_ = 0;
		riseupTime_ = 5;
		
		revInitTime_ = 20.0f;
		revFinalTime_ = 1.0f;
	}

	public void OnUpdate()
	{
		smc.Update();
   	}
   	
   	public void State_startup_Update()
   	{
   		float dt = FrameController.DT();
		startupTimer_ += dt;
		
		if(startupTimer_ <= startupTime_)
		{
			revTime_ = MMath.Cerp(revInitTime_,revFinalTime_,startupTimer_/startupTime_);
			revInc_ = 360.0f/revTime_;
		}
		else if(startupTimer_ >= startupTime_ + transitionTime_)
			smc.SetState(riseup);
			
		trans_.RotateAround(0,1,0,revInc_ * dt);
   	}
   	
   	public void State_riseup_OnStart()
   	{
		riseupTimer_ = 0;
   	}
   	
   	public void State_riseup_Update()
   	{
   		float dt = FrameController.DT();
		riseupTimer_ += dt;
		
		if(riseupTimer_ <= riseupTime_)
		{
			trans_.SetPositionY(MMath.Cerp(lowerTrans_.position.y,upperTrans_.position.y,riseupTimer_/riseupTime_));
		}
		else if(riseupTimer_ >= riseupTime_ + transitionTime_)
			smc.SetState(lowerdown);
		trans_.RotateAround(0,1,0,revInc_ * dt);
   	}
   	
   	public void State_lowerdown_OnStart()
   	{
		riseupTimer_ = 0;
		startupTimer_ = 0;
		revFinalTime_ = revTime_;
   	}
   	
   	public void State_lowerdown_Update()
   	{
   		float dt = FrameController.DT();
				
		if(riseupTimer_ <= riseupTime_)
		{
			riseupTimer_ += dt;
			trans_.SetPositionY(MMath.Cerp(upperTrans_.position.y,lowerTrans_.position.y,riseupTimer_/riseupTime_));
		}
		
		if(startupTimer_ <= shutdownTime_)
		{
			startupTimer_ += dt;
			revTime_ = MMath.Lerp(revFinalTime_,50,startupTimer_/shutdownTime_);
			revInc_ = 360.0f/revTime_;
		}
		
		if(riseupTimer_ > riseupTime_ && startupTimer_ > shutdownTime_)
			smc.SetState(end);
			
		//else if(riseupTimer_ >= riseupTime_ + transitionTime_)
		//	smc.SetState(riseup);
		trans_.RotateAround(0,1,0,revInc_ * dt);
   	}
   	
   	public void State_end_Update()
   	{
   	}
}