/******************************************************************************/
/*!
File       Event.h
 
 
Author     Joshua Chew Shi Yang
Par        email: joshuashiyang.chew@digipen.edu
 
 
Date       December 28, 2013
Brief

All content © 2014 DigiPen (USA) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef __MCH_EVENT_H_
#define __MCH_EVENT_H_

namespace Mocha
{
	template <typename TypeSignature> class Event;
}

// Engine Headers //
#include "Delegates.h"
#include "EventDeclaration.h"

// Convenience macro.
// Usage: Event< void() >::Method<MCH_E_METHOD( Class, classMethod )>( classInstance );
#define MCH_EVENT_METHOD( classSignature, classMethod )\
	classSignature,&classSignature::classMethod

#endif // __MCH_EVENT_H_