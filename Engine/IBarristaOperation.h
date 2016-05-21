#pragma once
#include "meta.h"
#include "BarristaToGo.h"

class IBarristaOperation : public IMeta
{
public:
    //returns whether or not the operation has more to do
	virtual bool Tick() = 0;

	template <typename... Args>
	BarristaToGo::BarristaToGoDescriptor* Invoke(void* cl, meta::metafunction_method* fnc, IBarristaOperation* clashingop, unsigned msDelay = 0, bool repeat = false, Args&&... args)
	{
		return barrista().togo->Invoke(cl, fnc, clashingop, msDelay, repeat, args...);
	}
};