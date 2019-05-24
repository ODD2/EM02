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
	double lambda,scaler,value,lvalue,step;
	value = Function.CalcFuncWithVar();
	try
	{
		int times = 0;
		do
		{
			basePoint = destPoint;
			step = 1.0;
			lambda = 0;
			/*lambda = dot((-1 * gradk), direction) / dot(direction, multm(Function.Heissen(), direction));
			value = Function(basePoint + lambda * direction);
			while (value!=value)
			{
				lambda *= 0.95;
				value = Function(basePoint + lambda * direction);
			}*/
			
			//lambda = GoldentSectionSearch(Function, direction);
			//Function.setVariables(basePoint + lambda * direction);


			for (int i = 0; i < 7; ++i) {
				do
				{
					lambda += step;
					Function.fsetVariables(basePoint + lambda * direction);
					lvalue = Function.CalcFuncWithVar();
					if (lvalue < value) value = lvalue;
					else {
						lambda -= step;
						break;
					}
				} while (1);
				step *= 0.1;
			}
			value = Function(basePoint + lambda * direction);

			destPoint = Function.GetInitPv();
			gradk_1 = Function.Gradient();
			deltaGrad = gradk_1 - gradk;
			if (length(deltaGrad) < _EPSILON)break;
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
				Recorder::RecordLine("-----------------------");
			}
#endif

			gradk = gradk_1;
			++times;
		} while (length(gradk) > _EPSILON && times < 100 && length(destPoint - basePoint )>_EPSILON);
		return destPoint;
	}
	catch (...) {
		throw;
	}
}
