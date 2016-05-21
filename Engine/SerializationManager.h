/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once
 

#define ___SHOW(a) std::cout << "SHOW(a): " << #a << ": " << ___VALUE(a) << std::endl
#define ___RETURNNAME(a, retval) retval = #a
//like concat, tostring requires two levels of indirection for macro expansion
#define ______GETNAME2(a) #a
#define ___GETNAME(a) ______GETNAME2(a)
#define ___VALUE(a) (a)
#define ___RETURNVALUE(a, retval) retval = (a)
#define ___CALL(obj, fnc) obj.fnc()
#define ___CALLPTR(obj, fnc) obj->(fnc)()
#define ___PACK(fnc) int fnc = 0
#define ___UNPACK(packfnc) typeid(packfnc).name()
//concat requires two levels of indirection for macro expansions
#define ______CONCAT2(a,b) a ## b
#define ___CONCAT(a,b) ______CONCAT2(a,b)

/* Do not remove this comment, it's actually mandatory */
/////////////////////////////////////
//Original Author: Nicholas Fuller
//Modifications:
//	Date	-	Change