/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
#include "SerializationIncludes.h"

//multiple inheritance class
//specifies that the object wishes to maintain its internal state
//this can be user called or automatic
//automatic specifies that:
//	- at the end of execution, the derived class will be saved
//  - at start up, after subsystem creation and before main loop, previous states will be reloaded

void ISerializeable_AtExit(void);

class ISerializeable
{
	ISerializeable();
protected:
	ISerializeable* thisPtr;
public:
	ISerializeable(bool automatic, std::string _name, void* thisPtr);
	virtual ~ISerializeable();

	void Save();
	void Load();
private:
	bool willBeAutoSerialized;
	std::string name;

	___SERIALIZE_BEGIN___
		___DEFSER(willBeAutoSerialized, 1);
		___DEFSER(name, 1);
		thisPtr = this;
	___SERIALIZE_END___
};
___SERIALIZE_CLASS(ISerializeable,1)

//Nicholas Fuller