#include "SteepestMinimizer.h"
#include "Recorder.h"

Vector SteepestMinimizer::Find()
{
	Vector basePoint;
	Vector destPoint(Function.GetInitP());
	Vector dirSteepest;
	Vector gradient = Function.Gradient();
	Matrix Heissen;
	double baseValue, destValue = Function.CalcFuncWithVar(), stepDistance;
	int times = 0;
	do {
		basePoint = destPoint;
		baseValue = destValue;
		dirSteepest = -1 * gradient;
		Heissen = Function.Heissen();
		stepDistance = dot(dirSteepest, dirSteepest) / dot(dirSteepest, multm(Heissen, dirSteepest));
		while (1) {
			destValue = Function(basePoint + stepDistance * dirSteepest);
			if (destValue != destValue) stepDistance *= 0.9;
			else break;
		}
		
		destPoint = Function.GetInitPv();
		gradient = Function.Gradient();
#ifdef OD_RECORD
		{
			Recorder::Record(times, "Times");
			Recorder::Record(basePoint, "Base");
			Recorder::Record(dirSteepest,"Dir");
			Recorder::Record(stepDistance, "lambda");
			Recorder::Record(destPoint, "Dest");
			Recorder::RecordLine("-----------------------");
		}
#endif
		++times;
	} while (length(gradient) > _EPSILON && times < 1000);
	return  destPoint;
}
