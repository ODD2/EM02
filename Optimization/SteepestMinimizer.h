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

	Vector Find() {
		Vector basePoint;
		Vector destPoint(Function.GetInitP());
		Vector dirSteepest;
		Matrix Heissen;
		double baseValue, destValue= Function.CalcFuncWithVar(), stepDistance;
		int times = 0;
		do {
			basePoint = destPoint;
			baseValue = destValue;
			dirSteepest = -1 * Function.Gradient();
			Heissen = Function.Heissen();
			stepDistance = dot(dirSteepest,dirSteepest) / dot(dirSteepest, multm(Heissen, dirSteepest));
			destValue = Function(basePoint + stepDistance * dirSteepest);
			destPoint = Function.GetInitPv();
			++times;
		} while (length(destPoint-basePoint)> _EPSILON && times < 1000);
		return  (basePoint + destPoint )/ 2;
	}
};

