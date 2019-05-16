#pragma once
#include "FunctionDef.h"
#define STEEPEST_STEP 0.001
 class SteepestMinimizer
{
public:
	 SteepestMinimizer(FunctionDef * Function):
		 Function(*Function) {
		 numArgs = this->Function.FuncVar->size();
	}

	unsigned int numArgs = 0;

	FunctionDef & Function;

	Vector Find();
};

