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
			move = Matrix::getVector(solve0(Hessein, -1 * Function.Gradient()), 0);
			value = Function(basePoint + move);
			while (value != value) {
				move = 0.95 * move;
				value = Function(basePoint + move);
			}
			destPoint = Function.GetInitPv();
			gradient = Function.Gradient();
#ifdef OD_RECORD
			{
				Recorder::Record(times, "Times");
				Recorder::Record(Hessein, "Hessian");
				Recorder::Record(move, "Delta");
				Recorder::Record(destPoint, "Dest");
				Recorder::RecordLine("-----------------------");
			}
#endif
			++times;
		} while (length(gradient)> _EPSILON && times < 1000);
		return destPoint;
	}
	catch (...) {
		throw;
	}
	
}
