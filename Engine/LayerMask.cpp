#include "stdafx.h"
#include "LayerMask.h"

LayerMask::LayerMask()
{
	mCurrentLayers = 1;
}

void LayerMask::AddLayer(Layers layer)
{
	mCurrentLayers |= layer;
}

void LayerMask::RemoveLayer(Layers layer)
{
	mCurrentLayers &= ~layer;
}

LayerMask::Layers LayerMask::GetLayerMasks()
{
	return (Layers)mCurrentLayers;
}

bool LayerMask::IsOnLayer(Layers layer)
{
	return (mCurrentLayers & layer) != 0;
}

void LayerMask::SetAllLayers()
{
  for (int i = 1; i < LAYER_TOTAL; i <<= 1)
    mCurrentLayers |= i;
}

void LayerMask::RemoveAllLayers()
{
  mCurrentLayers = 1;
}
