#include <string>
#include <vector>
#include <map>
#include "HelperFunctions.h"
using namespace std;

map<string, Info> OPinfo = {
	
	{"+",{2,1}},
	{"-",{2,1}},
	{"*",{2,2}},
	{"/",{2,2}},
	{"%",{2,2}},
	{"ln",{1,3}},
	{"sin",{1,3}},
	{"cos",{1,3}},
	{"tan",{1,3}},
	{"cot",{1,3}},
	{"sec",{1,3}},
	{"csc",{1,3}},
	{"^",{2,4}},
	{"(",{0,-10}},
	{")",{0,-10}},

};

double cot(double i) { return(1 / tan(i)); }
double sec(double i) { return(1 / cos(i)); }
double csc(double i) { return(1 / sin(i)); }
double plus_od(double i, double j) { return i + j; }
double minus_od(double i, double j) { return i - j; }
double product_od(double i, double j) { return i * j; }
double devide_od(double i, double j) { return i / j; }
map<string, double(*)(double) > OPfunc_1 = {
	{"sin",sin},
	{"cos",cos},
	{"tan",tan},
	{"cot",cot},
	{"sec",sec},
	{"csc",csc},
	{"ln",log}
};

map<string, double(*)(double ,double) > OPfunc_2 = {
	{"+",plus_od},
	{"-",minus_od},
	{"*",product_od},
	{"/",devide_od},
	{"%",fmod},
	{"^",pow},
};

void Infix2Postfix(std::vector<std::string>& fx) {
	try {
		std::vector<std::string> stack;
		std::vector<std::string> postfix;
		for (int i = 0, j = 0; i < fx.size(); i++)
		{
			std::string cur = fx[i];
			if (cur == "(") {
				stack.push_back(fx[i]);
			}
			else if (cur == ")") {
				while (stack.back() != "(")
				{
					postfix.push_back(stack.back());
					stack.pop_back();
				}
				stack.pop_back();
			}
			else if (OPinfo.count(cur)){
				while (stack.size() > 0 && OPinfo[stack.back()].opPrior >= OPinfo[cur].opPrior) {
					postfix.push_back(stack.back());
					stack.pop_back();
				}
				stack.push_back(cur);
			}
			else {
				postfix.push_back(cur);
			}
		}
		while (stack.size()) {
			if (stack.back() == "(") throw std::exception("Lexical Error", -1);
			//TODO: Error Detection.
			postfix.push_back(stack.back());
			stack.pop_back();
		}
		fx = postfix;
	}
	catch (std::exception& e) {
		throw e;
	}
}


void Infix2Postfix(std::vector<FormTok>& fx) {
	try {
		std::vector<FormTok> stack;
		std::vector<FormTok> postfix;
		for (int i = 0, j = 0; i < fx.size(); i++)
		{
			std::string cur = fx[i].content;
			if (cur == "(") {
				stack.push_back(fx[i]);
			}
			else if (cur == ")") {
				while (stack.back().content != "(")
				{
					postfix.push_back(stack.back());
					stack.pop_back();
				}
				stack.pop_back();
			}
			else if (OPinfo.count(cur)) {
				while (stack.size() > 0 && OPinfo[stack.back().content].opPrior >= OPinfo[cur].opPrior) {
					postfix.push_back(stack.back());
					stack.pop_back();
				}
				stack.push_back(fx[i]);
			}
			else {
				postfix.push_back(fx[i]);
			}
		}
		while (stack.size()) {
			if (stack.back().content == "(") throw std::exception("Lexical Error", -1);
			//TODO: Error Detection.
			postfix.push_back(stack.back());
			stack.pop_back();
		}
		fx = postfix;
	}
	catch (std::exception& e) {
		throw e;
	}
}