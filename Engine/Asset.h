/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#define AssetHandleFee std::string("[fee]")

#include "UniqueFileID.h" 
#include "IRegisterable.h"
#include "AvailableAssets.h"
#include "meta.h" 

class Asset : public IMeta
{
protected:
	unsigned int mid;				// Asset ID
	std::string mName;				// Asset Name
	AssetType mType;					// Asset Type
	UniqueFileID fileID;
	unsigned int mUseCounter;		// Number of objects using this Asset
	bool mAutoUnload;				// If false, don't unload asset when count reaches zero;
	bool mIsVirtual;

	friend class AssetManager;
public:
  Asset();

	UniqueFileID FileID() const { return fileID; }

	std::string GetName();
	void SetName(const std::string& name);
	AssetType GetType() const { return mType; }
	void SetType(AssetType val);

	unsigned int GetUseCounter();						// Return how many times a mesh is being used
	void AddUseCounter();								// Register one more component using the asset
	void SubtractUseConter();
	void SetUseCounter(unsigned useCounter);

	void SetAutoUnload(bool unload);
	bool IsAutoUnloading();

	bool IsVirtual() const;
	void IsVirtual(bool val);
	void SetId(unsigned id);
	unsigned GetId() const;

  virtual void OnAssetFileChanged() { }

	virtual Asset* Clone(){return new Asset(*this);}

	
	virtual ~Asset();

  ___SERIALIZE_BEGIN___
    ___DEFSER(mName, 1)
    ___DEFSER(mType, 1)
    ___DEFSER(mUseCounter, 1)
    ___DEFSER(mAutoUnload, 1)
    ___SERIALIZE_END___

    metadef(Asset)
    m_addfunc(OnAssetFileChanged);
    endmetadef;
};
___SERIALIZE_CLASS(Asset, 1);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change