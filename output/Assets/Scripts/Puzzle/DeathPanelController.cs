using Microsoft.Xna.Framework;
using MochaInterface;
using System;
using System.Collections.Generic;

public class DeathPanelController : MochaScript
{
    const int mRows = 3;
    const int mCols = 4;
    const int mAdditional = 2; // 1 at the start and 1 at the end
    const uint mSequenceTotal = 6;
    List<DeathPanel> mDeathPanels = new List<DeathPanel>();
    List<int> mPanelNumbers = new List<int>();

    uint mCurrSequence = 0;

    string dDeathPanel = "DeathPanel";
    public string panelName { get { return dDeathPanel; }
        set { dDeathPanel = value; } }

    bool mHasKilledPlayer = false;
    float mInvincibility = 0.5f;
	
	List<CLight> mLights = new List<CLight>();
	
	PuzzleCEnvironmentController envControllerScript;
	
    enum DIRECTIONS
    {
        T = 0,
        L,
        R,
        B,

        NUM_DIRECTIONS
    };

    public void OnStart()
    {
        for (int i = 0; i <= mRows * mCols + 1; ++i)
        {
            GameObject go = GameObject.GetGameObjectByName(Common.prefix + dDeathPanel + i.ToString(), false);
            if (go != null)
            {
                DeathPanel script = GetScript<DeathPanel>(go, false);
                if (script != null)
                {
                    mDeathPanels.Add(script);
                }
            }
        }
		
		foreach (GameObject obj in GameObject.GetGameObjectsWithTag("endlights"))
        {
            mLights.Add(obj.RequireComponent<CLight>());
        }
        if (mDeathPanels.Count != (mRows * mCols + mAdditional))
        {
            Logger.Log("Wrong init for DeathPanelController, less than 14 panels registered!");
        }
		
		envControllerScript = GetScript<PuzzleCEnvironmentController>(gameObject);
		
		if(envControllerScript == null)
			Logger.Log("Missing dependency script");
		
        RandomizePanels();
    }

    uint GetCol(uint index)
    {
        return (index - 1) % mCols;
    }
    uint GetRow(uint index)
    {
        return (index-1) / mCols;
    }
    uint GetIndex(uint row, uint col)
    {
        return row * mCols + col + 1; // because there is a panel0 infront
    }
	
	bool CheckIndex(uint row, uint col)
	{
		return (row < mRows && col < mCols); 
	}
	
    bool RecursiveSelectPanels(uint currRow, uint currCol, uint currSeq, uint totalSeq,
        ref List<int> panelBoard)
    {
        panelBoard[(int)GetIndex(currRow, currCol)] = (int)currSeq++; // Set the curr tile to this number
        if (currSeq > totalSeq) // Finished getting sequence
        {
            // Check if this last tile is at the last row (sequence must end at last row)
            if (currRow == mRows - 1)
                return true;
            else
                return false;
        }

        // Try all directions, and save those that are possible
        List<uint> availableDirections = new List<uint>();
        for (int i = 0; i < (int)DIRECTIONS.NUM_DIRECTIONS; ++i)
        {
            uint rowIndex = currRow;
            uint colIndex = currCol;

            switch (i)
            {
                case (int)DIRECTIONS.T: --rowIndex; break;
                case (int)DIRECTIONS.L: --colIndex; break;
                case (int)DIRECTIONS.R: ++colIndex; break;
                case (int)DIRECTIONS.B: ++rowIndex; break;
            }
            if (rowIndex >= mRows)
                continue;
            if (colIndex >= mCols)
                continue;
            if (panelBoard[(int)GetIndex(rowIndex, colIndex)] != -1) // If already occupied, try another direction
                continue;

            availableDirections.Add(GetIndex(rowIndex, colIndex));
        }
        if (availableDirections.Count == 0) // No available moves from this tile
            return false;

        // Test all the possible moves
        for (int i = 0; i < availableDirections.Count; ) // Don't need to reduce i
        {
            int randomChoice = MMath.GetRandomInt(0, availableDirections.Count);

            uint positionIndex = availableDirections[randomChoice];
            uint nextRow = GetRow(positionIndex);
            uint nextCol = GetCol(positionIndex);

            // If this returned a good board
            if (RecursiveSelectPanels(nextRow, nextCol, currSeq, totalSeq, ref panelBoard))
                return true;
            // Remove that move from the board
            panelBoard[(int)GetIndex(nextRow, nextCol)] = -1;

            availableDirections.RemoveAt(randomChoice); // Remove that move once tried
        }
        return false;
    }
    void RandomizePanels()
    {
        mPanelNumbers.Clear();
        mPanelNumbers.Add(0); // always have first big panel safe
        for (int i = 0; i < mRows * mCols; ++i)
        {
            mPanelNumbers.Add(-1); // -1 means empty
        }
        mPanelNumbers.Add((int)mSequenceTotal + 1); // always have last big panel hold the last number
        
        // Must start with first row
        uint rowIndex = 0;
        uint colIndex = (uint)MMath.GetRandomInt(0, mCols);

        RecursiveSelectPanels(rowIndex, colIndex, 1, mSequenceTotal, ref mPanelNumbers);
        // Assign the numbers to the panels
        for (int i = 0; i < mDeathPanels.Count; ++i)
        {
            mDeathPanels[i].SetNumber(mPanelNumbers[i], i);
            //Logger.Log("num: " + mPanelNumbers[i].ToString());
        }
    }
    public void ResetUponDeathOfPlayer()
    {
        for (int i = 0; i < mDeathPanels.Count; ++i)
        {
            mDeathPanels[i].isActive = true; // Reset all panels to active again
        }
        mCurrSequence = 0;

        RandomizePanels();
        envControllerScript.ResetState();
    }

    public void OnPanelTriggered(int panelID)
    {
        if (mHasKilledPlayer)
            return;

        int actualSeq = mPanelNumbers[panelID];
        if (actualSeq != mCurrSequence) // If player walked on wrong panel
        {
            Common.GetStealthPlayerCameraScript().Kill();
            mHasKilledPlayer = true;
        }
        else // If player walked on correct panel
        {
            ++mCurrSequence; // Move on to next sequence
            mDeathPanels[panelID].SetGreen();
			
			uint row = GetRow((uint)panelID);
			uint col = GetCol((uint)panelID);
            if (col < mCols && row < mRows)
            {
                envControllerScript.TriggerColumnLights(col);
                envControllerScript.TriggerMonster(SpawnZone(row, col));
            }

            // Reach the end of all panels
            if (mCurrSequence >= mAdditional + mSequenceTotal)
            {
                for (int i = 0; i < mDeathPanels.Count; ++i)
                {
                    mDeathPanels[i].SetDisable(); // Disable all panels, so they don't kill the player
                }
                Common.GetConsoleScreenScript().QueueMessage(
                    "Subject [REDACTED] : Physical resistance test complete", 0.0f, false, true);
            }
        }
    }

    public void OnUpdate()
    {
        if (mHasKilledPlayer) // Hack to prevent player from dying twice in succession
        {
            mInvincibility -= FrameController.DT();
            if (mInvincibility <= 0.0f)
            {
                mInvincibility = 1.0f;
                mHasKilledPlayer = false;
            }
        }
    }
	
	public Vector3 SpawnZone(uint row, uint col)
	{
		if(col >= mCols || row >= mRows )
			return new Vector3();
		else
		{
			List<uint> indices = new List<uint>();
			
			if(CheckIndex(row + 1, col))
				indices.Add(GetIndex(row + 1,col));
			if(CheckIndex(row, col + 1))
				indices.Add(GetIndex(row,col + 1));
			if(CheckIndex(row - 1, col))
				indices.Add(GetIndex(row - 1,col));
			if(CheckIndex(row, col - 1))
				indices.Add(GetIndex(row,col - 1));
				
			if(indices.Count == 0)	
				return new Vector3();
			else
			{
				int index = MMath.GetRandomInt(0, indices.Count - 1);
                int deathPanelIdx = (int)indices[index];
				
			//faceTransform.LookAt(Common.GetStealthPlayer().transform.GetPosition(CTransform.TransformSpace.GLOBAL
				//Vector3 spawnPoint
                return mDeathPanels[deathPanelIdx].gameObject.transform.GetPosition(CTransform.TransformSpace.GLOBAL);
			}
		}
	}
	
	public void OnPuzzleComplete()
	{
		foreach (CLight obj in mLights)
        {
            obj.setEnabled(true);
        }
		for (int i = 0; i < mDeathPanels.Count; ++i)
        {
            mDeathPanels[i].isActive = false; // Reset all panels to active again
        }
	}
}
