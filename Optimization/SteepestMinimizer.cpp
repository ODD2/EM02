#include "SteepestMinimizer.h"
#include "Recorder.h"

Vector SteepestMinimizer::Find()
{
	Vector basePoint;
	Vector destPoint(Function.GetInitP());
	Vector dirSteepest;
	Matrix Heissen;
	double baseValue, destValue = Function.CalcFuncWithVar(), stepDistance;
	int times = 0;
	do {
		basePoint = destPoint;
		baseValue = destValue;
		dirSteepest = -1 * Function.Gradient();
		Heissen = Function.Heissen();
		stepDistance = dot(dirSteepest, dirSteepest) / dot(dirSteepest, multm(Heissen, dirSteepest));
		destValue = Function(basePoint + stepDistance * dirSteepest);
		destPoint = Function.GetInitPv();
#ifdef OD_RECORD
		{
			Recorder::Record(times, "Times");
			Recorder::Record(basePoint, "Base");
			Recorder::Record(dirSteepest,"Dir");
			Recorder::Record(stepDistance, "lambda");
			Recorder::Record(destPoint, "Dest");
		}
#endif
		++times;
	} while (length(destPoint - basePoint) > _EPSILON && times < 1000);
	return  (basePoint + destPoint) / 2;
}
