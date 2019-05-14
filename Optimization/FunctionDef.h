#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "VarBox.h"
#include "FormTok.h"
#include "HelperFunctions.h"
using namespace std;
class FunctionDef
{
public:
	FunctionDef() {
		FuncVar = new std::map<string, VarBox>;
		FuncForm = new std::vector<FormTok>;
		TokLinkHead = NULL;
	}
	//Variable Table , Limits and Current Value.
	std::map<string, VarBox> * FuncVar;
	string OriginForm="";
	//Formula of Function.
	std::vector<FormTok>  * FuncForm;
	//Token Link Of Function.
	FormTokLink * TokLinkHead;

	//getter/setter
	std::vector<FormTok> * GetFmu() {
		return  FuncForm;
	}
	std::map<string, VarBox> * GetVar() {
		return  FuncVar;
	}
	double setVariables(vector<double>& args) {
		int index = 0;
		for (auto it = FuncVar->begin(); it != FuncVar->end(); ++it) {
			it->second.value = args[index];
			++index;
		}
		return 0;
	}

	double setLimits(map<string,pair<double,double>>& limits) {
		for (auto it = limits.begin(); it != limits.end(); ++it) {
			if (FuncVar->count(it->first) == 0) {
				//TODO Exceptions;
			}
			else {
				VarBox & v =(*FuncVar)[it->first];
				v.l_low = it->second.first;
				v.l_high = it->second.second;
			}
		}
		return 0;
	}

	double operator()(vector<double> args) {
		
		return 0;
	}

	string toString() {
		string result = "f(X) = " + OriginForm + "\r\n";
		for (auto it = FuncVar->begin(); it != FuncVar->end(); ++it) {
			result += it->first +" = " + to_string(it->second.value) +  ", [" + to_string(it->second.l_low) + "," + to_string(it->second.l_high) + "]" + "\r\n";
		}
		return result;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private: void FitContent(FormTokLink * FTL, FormTok& FT) {
		FTL->tt = FT.tt;
		if (FT.tt == TokenType::op || FT.tt == TokenType::var) {
			FTL->container.content = new string(FT.content);
			
		}
		else if (FT.tt == TokenType::real) {
			FTL->container.value = stold(FT.content);
		}
	}
public: void CreateTokenLink() {
		
		TokLinkHead = new FormTokLink();
		FitContent(TokLinkHead,(*FuncForm)[0]);

		FormTokLink* Prev = TokLinkHead;
		for (int i = 1; i < FuncForm->size(); ++i) {
			FormTokLink * Cur = new FormTokLink();
			Prev->Next = Cur; Cur->Prev = Prev; //Bidirection Linkage
			FitContent(Cur, (*FuncForm)[i]);
			Prev = Cur;
		}
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private: FormTokLink * CopyTokenLink() {
		FormTokLink *NewHead = new FormTokLink();
		NewHead->container = TokLinkHead->container;
		NewHead->tt = TokLinkHead->tt;
		FormTokLink * OldCur = TokLinkHead->Next;
		FormTokLink * Prev = NewHead;
		while (OldCur) {
			FormTokLink * Cur = new FormTokLink();
			Cur->Prev = Prev; Prev->Next = Cur;
			Cur->container = OldCur->container;
			Cur->tt = OldCur->tt;
			OldCur= OldCur->Next;
			Prev = Cur;
		}
		return NewHead;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private: void AdjustNeighbor(FormTokLink * Cur, FormTokLink * HEAD, int reqArgs = 0) {
		FormTokLink * NewNeighbor = Cur;//Find the New Neighbor
		vector<FormTokLink*> trashcan; //Collect Unused Resources

		for (int i = 0; i < reqArgs; ++i) {
			NewNeighbor = NewNeighbor->Prev;
			if (NewNeighbor == NULL) {
				throw exception("Error,Cannot Iterate to the desire position");
			}
			trashcan.push_back(NewNeighbor);	
		}

		//Adjust Connection
		Cur->Prev = NewNeighbor->Prev;
		if (Cur->Prev == NULL) {//Head Condition
			HEAD = Cur;
		}
		else {
			Cur->Prev->Next = Cur;
		}

		//CleanUp
		for (int i = 0; i < trashcan.size(); ++i) {
			delete trashcan[i];
		}
	}
public: double CalcFuncWithVar() {
		FormTokLink * HEAD = CopyTokenLink();
		FormTokLink * Cur=HEAD;

		//Debug
		while(Cur) {
			if (Cur->tt == TokenType::real) {
				cout << Cur->container.value;
			}
			else cout << *(Cur->container.content);
			Cur = Cur->Next;
		}
		cout << '\n';
		Cur = HEAD;
		//Replace Variables With Values inside the VarTable
		while (Cur) {
			if (Cur->tt == TokenType::var) {
				Cur->tt = TokenType::real;
				string& varName = *(Cur->container.content);
				Cur->container.value = (FuncVar->operator[](varName)).value;
			}
			Cur = Cur->Next;
		}

		int length = FuncForm->size();

		while (length > 1) {
			Cur = HEAD;
			while (Cur) {
				if (Cur->tt == TokenType::real) {
					cout << Cur->container.value;
				}
				else cout << *(Cur->container.content);
				Cur = Cur->Next;
			}
			cout << '\n';
			Cur = HEAD;
			while (Cur && Cur->tt != TokenType::op) {
				Cur = Cur->Next;
			}
			//TODO:Error Detection;
			if (Cur == nullptr)throw std::exception("Error, Incorrect Formula", -1);

			const string& op =*(Cur->container.content);
			double result;
			if (OPinfo[op].reqArgs == 1 && OPfunc_1.count(op) == 1) {
				 Cur->container.value = OPfunc_1[op](Cur->Prev->container.value);
				 Cur->tt = TokenType::real;
				 if (Cur->Prev == HEAD) HEAD = Cur;
				 AdjustNeighbor(Cur, HEAD,1);
				 length -= 1;
			}
			else if (OPinfo[op].reqArgs == 2 && OPfunc_2.count(op) == 1) {
				//Change Cur into result
				Cur->container.value = OPfunc_2[op](Cur->Prev->Prev->container.value, Cur->Prev->container.value); 
				Cur->tt = TokenType::real;
				if (Cur->Prev->Prev == HEAD) HEAD = Cur;
				AdjustNeighbor(Cur, HEAD,2);
				length -= 2;
			}
			else {
				//TODO:Error Detection;
			}
			cout << '\n';
		}

		return Cur->container.value;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};

