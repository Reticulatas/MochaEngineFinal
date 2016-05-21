using System;
using System.Collections.Generic;
using MochaInterface;

public class State
{
    public Action OnEnter {get; set;}
    public Action OnUpdate {get; set;}
    public Action OnExit {get; set;}

    private float _timeInState;
    public float MaxTimeInState { get; set;}
    public State NextState { get; set; }
    public StateMachineController MyStateMachineController { get; set; }

    public State(StateMachineController smc, Action _OnEnter, Action _OnUpdate, Action _OnExit, float _MaxTimeInState = 0.0f, State _nextState = null)
    {
        MyStateMachineController = smc;
        OnEnter = _OnEnter;
        OnUpdate = _OnUpdate;
        OnExit = _OnExit;
        MaxTimeInState = _MaxTimeInState;
        NextState = _nextState;
    }
    public State(StateMachineController smc) { MyStateMachineController = smc; }

    public float GetTimeInState() { return _timeInState; }
    public float GetMaxTimeInState() { return MaxTimeInState; }
    public float GetPercentageTime() {
        if (MaxTimeInState == 0.0f)
            return 0;
        return _timeInState / MaxTimeInState; 
    }

    //do not call publicly
    public void Reset()
    {
        _timeInState = 0;
    }

    public void Update()
    {
        _timeInState += MochaInterface.FrameController.DT();
        if (MaxTimeInState != 0.0f && _timeInState > MaxTimeInState)
        {
            if (NextState == null)
            {
                string updatemethod = "";
                if (OnUpdate != null)
                    updatemethod = OnUpdate.Method.Name;
                throw new MochaScriptSystem.ScriptException("SMC Error: " + updatemethod + " could not set next state because it is null.");
            }
            MyStateMachineController.SetState(NextState);
            return;
        }

        if (OnUpdate != null)
            OnUpdate();
    }
}

public class StateMachineController
{
    State current_state = null;

    public State GetCurrentState()
    {
        return current_state;
    }

    public void SetState(State s)
    {
        State old_state = current_state;
        if (s != null)
            current_state = s;
        else
            current_state = null;
        if (old_state != null)
            if (old_state.OnExit != null)
                old_state.OnExit();
        if (current_state != null)
        {
            current_state.Reset();
            if (current_state.OnEnter != null)
                current_state.OnEnter();
        }
    }

    public void Update()
    {
            if (current_state != null)
                current_state.Update();
    }
}