#pragma once
#include "FunctionDef.h"
class NewtonMinimizer
{
public:
	NewtonMinimizer(FunctionDef * Function);
	unsigned int numArgs = 0;
	FunctionDef & Function;
	Vector Find();
};

