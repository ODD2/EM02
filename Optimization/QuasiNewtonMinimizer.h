#pragma once
#include "FunctionDef.h"
class QuasiNewtonMinimizer
{
public:
	QuasiNewtonMinimizer(FunctionDef * Function);
	unsigned int numArgs = 0;
	FunctionDef & Function;
	Vector Find();
};

