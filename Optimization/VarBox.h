#pragma once
struct VarBox{
	//limits
	double l_low = -FLT_MAX;
	double l_high = FLT_MAX;
	//current value
	double value = 0;
};