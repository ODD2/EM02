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
		Matrix GradM, PointM,TempM,Hassien;

		gradient2 = Function.Gradient();
		double value, lambda;
		int times = 0;
		do
		{
			basePoint = destPoint;
			gradient1 = gradient2;
			direction = -1 * multm(QuasiHeiss, gradient1);
			//lambda = GoldentSectionSearch(Function, direction);
			
			lambda = dot(-1*gradient1, direction) / dot(direction, multm(Function.Heissen(), direction));




			value = Function(basePoint + lambda * direction);

			while (value != value) {
				Recorder::RecordLine("Function Reach NaN! limiting lambda.");
				lambda *= 0.9;
				value = Function(basePoint + lambda * direction);
			}



			destPoint = Function.GetInitPv();

			gradient2 = Function.Gradient();

			deltaPoint = destPoint - basePoint;
			deltaGradient = gradient2 - gradient1;
			
			GradM = Vector::toMatrix(deltaGradient);
			PointM = Vector::toMatrix(deltaPoint);
			TempM = (QuasiHeiss*GradM);
			double scaler1 = dot(deltaPoint, deltaGradient), 
						scaler2 = dot(deltaGradient,Matrix::getVector(TempM,0)) ;

			QuasiHeiss = QuasiHeiss + (PointM * transpose(PointM)) / scaler1 - (TempM*transpose(TempM)) / scaler2;
 

#ifdef OD_RECORD
			{
				Recorder::Record(times, "Times");
				Recorder::Record(QuasiHeiss, "QuasiHess");
				Recorder::Record(Function.Heissen(), "Hassien");
				Recorder::Record(destPoint, "Dest");
				Recorder::RecordLine("-----------------------");
			}
#endif
			++times;
		} while (length(gradient2)> _EPSILON && times < 1000); //while (length(basePoint - destPoint) > _EPSILON && times< 1000);
		return destPoint;
	}
	catch (...) {
		throw;
	}
}
