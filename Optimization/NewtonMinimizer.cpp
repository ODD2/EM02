#include "NewtonMinimizer.h"



NewtonMinimizer::NewtonMinimizer(FunctionDef * Function)
:Function(*Function)
{
}

Vector NewtonMinimizer::Find()
{
	Vector basePoint, destPoint(Function.GetInitP());
	Vector move;
	Vector gradient;
	Matrix Hessein;
	double value;
	try
	{
		int times = 0;
		do
		{
			basePoint = destPoint;
			Hessein = Function.Heissen();
			//if(lengthm(Hessein))
			move = 0.95*Matrix::getVector(solve0(Hessein, -1 * Function.Gradient()), 0);
			value = Function(basePoint + move);
			destPoint = Function.GetInitPv();
#ifdef OD_RECORD
			{
				Recorder::Record(times, "Times");
				Recorder::Record(Hessein, "Hessian");
				Recorder::Record(move, "Delta");
				Recorder::Record(destPoint, "Dest");
			}
#endif
			++times;
		} while (length(basePoint - destPoint) > _EPSILON && times < 1000);
		return (basePoint+destPoint)/2;
	}
	catch (...) {
		throw;
	}
	
}
