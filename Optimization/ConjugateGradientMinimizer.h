#pragma once
#include "FunctionDef.h"
 class ConjugateGradientMinimizer
{
public:
	ConjugateGradientMinimizer(FunctionDef * Function);
	unsigned int numArgs = 0;
	FunctionDef & Function;
	Vector Find();
 };