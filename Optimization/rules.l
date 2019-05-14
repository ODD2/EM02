%{
int mylineno = 0;
#include <map>
#include <string>
#include "VarBox.h"
#include "FormTok.h"
using namespace std;

std::map<string,VarBox>* VarTable;
std::vector<FormTok> * WorkVec;
%}
%option c++


alpha   [A-Za-z]
dig     [0-9]
num1    {dig}+\.?([eE][-+]?{dig}+)?
num2    {dig}*\.{dig}+([eE][-+]?{dig}+)?
variable   [xyzijk][0-9]*
number  {num1}|{num2}
ws      [ \t]+

%%
sin|cos|tan|cot|sec|csc|ln|"("|")"|"+"|"-"|"*"|"/"|=|"^"|% {
		 WorkVec->push_back({YYText(),TokenType::op});
}

e {
		 WorkVec->push_back({"2.71828182",TokenType::real});
}

{number}  {
    WorkVec->push_back({YYText(),TokenType::real});
}

{variable} {
	WorkVec->push_back({YYText(),TokenType::var});
	if(VarTable->count(YYText())==0){
		(*VarTable)[YYText()];
	}
}

\n    mylineno++;

{ws}    /* skip blanks and tabs */

. {
    cout << "Unknown:" << mylineno << YYText() <<'\n';
}

%%