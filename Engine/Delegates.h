/******************************************************************************/
/*!
File       Delegates.h
Author     Joshua Chew Shi Yang
Par        email: joshuashiyang.chew@digipen.edu
Date       December 28, 2013
Brief

All content © 2014 DigiPen (USA) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

/**
* File : Delegate.h
* Brief : The template for delegates
* Reference: http://www.codeproject.com/KB/cpp/ImpossiblyFastCppDelegate.aspx.
*/

#ifndef __MCH_DELEGATE_H_
#define __MCH_DELEGATE_H_

// Engine Headers //

namespace Mocha
{
	template <typename FunctionSignature> class Delegate;
}

#include "DelegateDeclaration.h"

#endif // __CH_DELEGATE_H_
