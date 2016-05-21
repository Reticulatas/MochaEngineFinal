/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
 
#include "IRegisterable.h"
 

/*
extern Serializer::KeyToDefs defaultStrMap[];
extern unsigned defaultStrMapLen;
*/
int Reg::numOfRegs = 0;
char Reg::reg_names[MAXREGS][REGNAMELENGTH];
Reg::GenFncPtr Reg::reg_gens[MAXREGS];

//define generic object reference counting for all objects
int IRegisterable::AddRef() 
{
	static bool firstAccess = true;
	if (firstAccess)
		firstAccess = false; as_ref_count = 0;
	return ++as_ref_count; 
}
int IRegisterable::Release() 
{
	//as_ref_count--;
	return 1;
}

void Serializer::_Assign(std::string name, Reg::GenFncPtr gen)
{
	if (Reg::numOfRegs >= MAXREGS) {	Log("Too many components, go complain to Fuller");	return;	}
	if (name.length() >= REGNAMELENGTH)
		name = "NAME_TOO_LONG";
	strcpy_s(Reg::reg_names[Reg::numOfRegs], REGNAMELENGTH, name.c_str());
	Reg::reg_gens[Reg::numOfRegs] = gen;
	Reg::numOfRegs++;
}

IRegisterable* Serializer::MakeInstanceFromString(std::string name)
{
	for (int i = 0; i < Reg::numOfRegs; i++)
	{
		if (!name.compare(Reg::reg_names[i]))
		{
			//find top level functor for making object of component type T, then cast it to a function that produces an ISerializable
			IRegisterable* rptr = dynamic_cast<IRegisterable*>((Reg::reg_gens[i])());
			return rptr;
		}
	}
	return 0;
}

std::vector<std::string> Serializer::GenerateRegisteredNameList(void)
{
	std::vector<std::string> names;
	for (int i = 0; i < Reg::numOfRegs; i++)
		names.push_back(Reg::reg_names[i]);
	return names;
}

meta_define(IRegisterable);

/////////////////////////////////////
//Original Author: Nicholas Fuller 
//Modifications:
//	Date	-	Change