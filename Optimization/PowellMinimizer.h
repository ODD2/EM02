#pragma once
#include "FunctionDef.h"
#include "Vector.h"


class PowellMinimizer
{
public:
	PowellMinimizer(FunctionDef * Function);
	unsigned int numArgs = 0;
	FunctionDef & Function;
	Vector Find();
private:
	double GoldenSectionSearch(Vector & direction, Vector & basePoint);
};

vector<double> operator*(double scale, const vector<double>& r);

vector<double> operator+(const vector<double>& l, const vector<double>& r);

vector<double> operator-(const vector<double>& l, const vector<double>& r);
