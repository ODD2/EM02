#pragma once
#include <string>
#include <vector>
#include "FormTok.h"
using namespace std;
struct Info {
	unsigned int reqArgs;
	int opPrior;
};



extern map<string, Info> OPinfo;
extern map<string, double(*)(double) > OPfunc_1;
extern map<string, double(*)(double, double) > OPfunc_2;


void Infix2Postfix(std::vector<std::string>& fx);
void Infix2Postfix(std::vector<FormTok>& fx);