#include "ConjugateGradientMinimizer.h"
#include "PowellMinimizer.h"





ConjugateGradientMinimizer::ConjugateGradientMinimizer(FunctionDef * Function):
	Function(*Function)
{
	numArgs = Function->FuncVar->size();
}

Vector ConjugateGradientMinimizer::Find()
{
	Vector basePoint, destPoint(Function.GetInitP());
	
	Vector gradk= Function.Gradient(),gradk_1,deltaGrad;
	Vector direction = -1 * gradk;
	double lambda,scaler;
	try
	{
		int times = 0;
		do
		{
			basePoint = destPoint;
			lambda = GoldentSectionSearch(Function, direction);
			Function.setVariables(basePoint + lambda * direction);
			destPoint = Function.GetInitPv();
			gradk_1 = Function.Gradient();
			deltaGrad = gradk_1 - gradk;
			scaler = dot(gradk_1, deltaGrad) / dot(direction, deltaGrad);
			direction = -1 * gradk_1 + scaler * direction;
#ifdef OD_RECORD
			{
				Recorder::Record(times, "Times");
				//Recorder::Record(basePoint, "Base");
				Recorder::Record(lambda, "Alpha");
				Recorder::Record(scaler, "Beta");
				Recorder::Record(direction, "Dir");
				Recorder::Record(destPoint, "Dest");
			}
#endif

			gradk = gradk_1;
			++times;
		} while (length(destPoint-basePoint ) > _EPSILON && times < 1000);
		return (basePoint + destPoint) / 2;
	}
	catch (...) {
		throw;
	}
}
