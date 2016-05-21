#pragma once
#include "SerializationManager.h"
#include "metaremqual.h"

//late expansions helper macro
#define ___EARLY_EXPAND(x) ___EARLY_EXPAND_I(x)
#define ___EARLY_EXPAND_I(x) x

//splitter
#define __Split2_1(a,b) a
#define __Split2_2(a,b) b
#define __Split3_1(a,b,c) a
#define __Split3_2(a,b,c) b
#define __Split3_3(a,b,c) c