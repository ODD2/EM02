#include "PowellMinimizer.h"
#include "Vector.h"
#include <math.h>
using namespace std;
#define GPROP 0.61803398874989484
#define invGPROP 0.38196601125010515

PowellMinimizer::PowellMinimizer(FunctionDef * Function) :
	Function(*Function)
{
	numArgs =  this->Function.FuncVar->size();
}

Vector PowellMinimizer::Find()
{
	//Initializations
	vector<Vector> Directions(numArgs);
	Vector Magnitudes(numArgs);
	for (int i = 0; i < numArgs; ++i) {
		Directions[i] = Vector(numArgs);
		Directions[i][numArgs - 1 - i] = 1;
		Magnitudes[i] = -FLT_MAX;
	}

	Vector Ps;
	Vector Pe(Function.GetInitP());
	int times = 0;
	do{
		Ps = Pe;
#ifdef OD_RECORD
		{
			Recorder::Record(times, "Times");
		}
#endif
		for (int i = numArgs -1 ; i >=0 ; --i) {
#ifdef OD_RECORD
			{
				Recorder::Record(Pe, "X");
			}
#endif
			Magnitudes[i] = GoldenSectionSearch(Directions[i], Pe);
			Pe = Pe + (Magnitudes[i] * Directions[i]);
#ifdef OD_RECORD
			{
				Recorder::Record(Magnitudes[i], "lambda");
				Recorder::Record(Directions[i], "Dir");
			}
#endif
		}
		Directions.pop_back();//Remove Direction TODO: Remove the Direction that moves the longest distance
		Directions.insert(Directions.begin(), Pe - Ps);//Add New Direction.
		double tmpMag = GoldenSectionSearch(Directions[0], Pe);//Push deeper through the direction
		Pe = Pe + tmpMag * Directions[0];
		++times;
	} while ( length(Ps-Pe) > _EPSILON && times < 1000 );

	return (Ps + Pe) / 2;
}

double PowellMinimizer::GoldenSectionSearch( Vector & direction,  Vector & basePoint) {
	//Find Boundaries.
	//TODO:Error Detection , Dimension Unmach.
	double high_bound = FLT_MAX, low_bound = -FLT_MAX;
	auto it = Function.FuncVar->begin();
	double high_scale = 0, low_scale = 0;
	for (int i = 0; i < direction.dim(); ++i,++it) {
		if (direction[i] == 0) {
			continue;
		}
		else if (direction[i] > 0) {
			low_scale = (it->second.l_low - basePoint[i]) / direction[i];
			high_scale = (it->second.l_high - basePoint[i]) / direction[i];
		}
		else {
			high_scale = (it->second.l_low - basePoint[i]) / direction[i];
			low_scale = (it->second.l_high - basePoint[i]) / direction[i];
		}
		//
		if (high_scale < high_bound) {
			high_bound = high_scale;
		}
		if (low_scale > low_bound) {
			low_bound = low_scale;
		}
	}

	//Do Section Search Algorithm
	double a = low_bound, b = high_bound;
	double width = b - a;
	double c = a + width* invGPROP, d = a+ width*GPROP;
	Vector  loc_c = basePoint + (c * direction), loc_d = basePoint + (d * direction);

	int n = int(ceil(log(_EPSILON / width) / log(GPROP)))+5;
	
	int times = 0;
	while (times < n) {
		width *= GPROP;
		if (Function(loc_c.Data) > Function(loc_d.Data) ){
			a = c;
			c = d;
			d = a + GPROP * width;
			loc_c = loc_d;
			loc_d = basePoint + (d * direction);
		}
		else {
			b = d;
			d = c;
			c = a + invGPROP * width;
			loc_d = loc_c;
			loc_c = basePoint + (c * direction);
		}
		++times;
	}

	return (a + b) / 2;
}

double GoldentSectionSearch(FunctionDef& Function,Vector & direction) {
	//Find Boundaries.
	//TODO:Error Detection , Dimension Unmach.
	Vector originPoint = Function.GetInitPv();
	Vector  basePoint = originPoint;
	double high_bound = FLT_MAX, low_bound = -FLT_MAX;
	auto it = Function.FuncVar->begin();
	double high_scale = 0, low_scale = 0;
	for (int i = 0; i < direction.dim(); ++i, ++it) {
		if (direction[i] == 0) {
			continue;
		}
		else if (direction[i] > 0) {
			low_scale = (it->second.l_low - basePoint[i]) / direction[i];
			high_scale = (it->second.l_high - basePoint[i]) / direction[i];
		}
		else {
			high_scale = (it->second.l_low - basePoint[i]) / direction[i];
			low_scale = (it->second.l_high - basePoint[i]) / direction[i];
		}
		//
		if (high_scale < high_bound) {
			high_bound = high_scale;
		}
		if (low_scale > low_bound) {
			low_bound = low_scale;
		}
	}

	//Do Section Search Algorithm
	double a = low_bound, b = high_bound;
	double width = b - a;
	double c = a + width * invGPROP, d = a + width * GPROP;
	Vector  loc_c = basePoint + (c * direction), loc_d = basePoint + (d * direction);

	int n = int(ceil(log(_EPSILON / width) / log(GPROP)))+5;

	int times = 0;
	while (times < n) {
		width *= GPROP;
		if (Function(loc_c.Data) > Function(loc_d.Data)) {
			a = c;
			c = d;
			d = a + GPROP * width;
			loc_c = loc_d;
			loc_d = basePoint + (d * direction);
		}
		else {
			b = d;
			d = c;
			c = a + invGPROP * width;
			loc_d = loc_c;
			loc_c = basePoint + (c * direction);
		}
		++times;
	}
	//Resume Origin Point.
	Function.setVariables(originPoint.Data);
	return (a + b) / 2;
}

vector<double> operator*(double scale, const vector<double>& r) {
	vector<double> ret = r;
	for (int i = 0; i < ret.size(); ++i) {
		ret[i] *= scale;
	}
	return ret;
}

vector<double> operator+(const vector<double>& l, const vector<double>& r) {
	vector<double> ret = l;
	for (int i = 0; i < ret.size(); ++i) {
		ret[i] += r[i];
	}
	return ret;
}

vector<double> operator-(const vector<double>& l, const vector<double>& r) {
	vector<double> ret = l;
	for (int i = 0; i < ret.size(); ++i) {
		ret[i] -= r[i];
	}
	return ret;
}