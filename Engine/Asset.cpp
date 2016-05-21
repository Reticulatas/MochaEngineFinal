/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "Asset.h"

std::string Asset::GetName()
{
	return this->mName;
}

void Asset::SetName(const std::string& name)
{
	this->mName = name;
}


unsigned int Asset::GetUseCounter()
{
	return this->mUseCounter;
}

void Asset::AddUseCounter()
{
	this->mUseCounter++;
}

void Asset::SubtractUseConter()
{
	this->mUseCounter--;
}

void Asset::SetAutoUnload(bool unload)
{
	mAutoUnload = unload;
}

bool Asset::IsAutoUnloading()
{
	return this->mAutoUnload;
}

Asset::~Asset()
{
	/* virtual destructor */
}

void Asset::SetId( unsigned id )
{
	this->mid = id;
}

unsigned Asset::GetId() const
{
	return this->mid;
}

void Asset::SetUseCounter( unsigned useCounter )
{
	mUseCounter = useCounter;
}

void Asset::SetType(AssetType val)
{
	mType = val;
}

void Asset::IsVirtual(bool val)
{
	mIsVirtual = val;
}

bool Asset::IsVirtual() const
{
	return mIsVirtual;
}

Asset::Asset() : mName(AssetHandleFee), mAutoUnload(true), mUseCounter(0), mid(0), mIsVirtual(false)
{

}

meta_define(Asset);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change