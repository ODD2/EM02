#include "QuasiNewtonMinimizer.h"
#include "Matrix.h"
#include "PowellMinimizer.h"


QuasiNewtonMinimizer::QuasiNewtonMinimizer(FunctionDef * Function)
	:Function(*Function) {
	numArgs = Function->FuncVar->size();
}

Vector QuasiNewtonMinimizer::Find()
{
	
	try
	{
		Vector basePoint, destPoint(Function.GetInitP());
		Vector direction, move, deltaPoint, deltaGradient;
		Vector gradient1, gradient2;
		Matrix QuasiHeiss = Matrix::Identity(numArgs);
		Matrix GradM, PointM,TempM;

		gradient2 = Function.Gradient();
		double value, lambda;
		int times = 0;
		do
		{
			basePoint = destPoint;
			gradient1 = gradient2;

			direction = -1 * multm(QuasiHeiss, gradient1);
			lambda = GoldentSectionSearch(Function, direction);

			Function.setVariables(basePoint + lambda * direction);
			destPoint = Function.GetInitPv();

			gradient2 = Function.Gradient();

			deltaPoint = destPoint - basePoint;
			deltaGradient = gradient2 - gradient1;
			
			GradM = Vector::toMatrix(deltaGradient);
			PointM = Vector::toMatrix(deltaPoint);
			TempM = (QuasiHeiss*GradM);
			double scaler1 = dot(deltaPoint, deltaGradient), 
						scaler2 = dot(deltaGradient,Matrix::getVector(TempM,0)) ;

			QuasiHeiss = QuasiHeiss + (PointM * transpose(PointM)) / scaler1 + (TempM*transpose(TempM)) / scaler2;
 

			move = Matrix::getVector(solve0(Function.Heissen(), -1 * Function.Gradient()), 0);
			value = Function(destPoint + move);
			destPoint = Function.GetInitPv();
#ifdef OD_RECORD
			{
				Recorder::Record(times, "Times");
				Recorder::Record(QuasiHeiss, "QuasiHess");
				Recorder::Record(destPoint, "Dest");
			}
#endif
			++times;
		} while (length(basePoint - destPoint) > _EPSILON && times< 1000);
		return (basePoint + destPoint) / 2;
	}
	catch (...) {
		throw;
	}
}
