/******************************************************************************/
/*!
File       DelegateList.h
Author     Joshua Chew Shi Yang
Par        email: joshuashiyang.chew@digipen.edu
Date       December 28, 2013
Brief

All content � 2014 DigiPen (USA) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

/**
* File : Delegate_List.h
* Brief : Function delegate declarations.
* Reference: http://www.codeproject.com/KB/cpp/ImpossiblyFastCppDelegate.aspx.
*/

#define MCH_FASTCALL __fastcall
#define MCH_DELEGATE_CALLTYPE MCH_FASTCALL

// 0 params
#define MCH_DELEGATE_PARAM_COUNT 0
#define MCH_DELEGATE_TEMPLATE_PARAMS 
#define MCH_DELEGATE_TEMPLATE_ARGS 
#define MCH_DELEGATE_PARAMS 
#define MCH_DELEGATE_ARGS
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 1 params
#define MCH_DELEGATE_PARAM_COUNT 1
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1
#define MCH_DELEGATE_TEMPLATE_ARGS A1
#define MCH_DELEGATE_PARAMS A1 a1
#define MCH_DELEGATE_ARGS a1
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 2 params
#define MCH_DELEGATE_PARAM_COUNT 2
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2
#define MCH_DELEGATE_ARGS a1,a2
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 3 params
#define MCH_DELEGATE_PARAM_COUNT 3
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3
#define MCH_DELEGATE_ARGS a1,a2,a3
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 4 params
#define MCH_DELEGATE_PARAM_COUNT 4
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3, typename A4
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3, A4
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3, A4 a4
#define MCH_DELEGATE_ARGS a1,a2,a3,a4
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 5 params
#define MCH_DELEGATE_PARAM_COUNT 5
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3, typename A4, typename A5
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3, A4, A5
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3, A4 a4, A5 a5
#define MCH_DELEGATE_ARGS a1,a2,a3,a4,a5
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 6 params
#define MCH_DELEGATE_PARAM_COUNT 6
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3, typename A4, typename A5, typename A6
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3, A4, A5, A6
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6
#define MCH_DELEGATE_ARGS a1,a2,a3,a4,a5,a6
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 7 params
#define MCH_DELEGATE_PARAM_COUNT 7
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3, A4, A5, A6, A7
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7
#define MCH_DELEGATE_ARGS a1,a2,a3,a4,a5,a6,a7
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 8 params
#define MCH_DELEGATE_PARAM_COUNT 8
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3, A4, A5, A6, A7, A8
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8
#define MCH_DELEGATE_ARGS a1,a2,a3,a4,a5,a6,a7,a8
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 9 params
#define MCH_DELEGATE_PARAM_COUNT 9
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3, A4, A5, A6, A7, A8, A9
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9
#define MCH_DELEGATE_ARGS a1,a2,a3,a4,a5,a6,a7,a8,a9
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS

// 10 params
#define MCH_DELEGATE_PARAM_COUNT 10
#define MCH_DELEGATE_TEMPLATE_PARAMS typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10
#define MCH_DELEGATE_TEMPLATE_ARGS A1, A2, A3, A4, A5, A6, A7, A8, A9, A10
#define MCH_DELEGATE_PARAMS A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10
#define MCH_DELEGATE_ARGS a1,a2,a3,a4,a5,a6,a7,a8,a9,a10
#include "DelegateImplementation.h"
#undef MCH_DELEGATE_PARAM_COUNT
#undef MCH_DELEGATE_TEMPLATE_PARAMS
#undef MCH_DELEGATE_TEMPLATE_ARGS
#undef MCH_DELEGATE_PARAMS
#undef MCH_DELEGATE_ARGS
