#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "VarBox.h"
#include "FormTok.h"
#include "HelperFunctions.h"
#include "odflags.h"
#include "Vector.h"
#include "Matrix.h"
using namespace std;
#define _EPSILON 0.000000001
#define H 0.001
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
	std::vector<double> GetInitP() {
		std::vector<double> ret;
		for (auto it = FuncVar->begin(), ed = FuncVar->end();
			it != ed;
			++it) {
			ret.push_back(it->second.value);
		}
		return ret;
	}
	Vector GetInitPv() {
		Vector ret;
		for (auto it = FuncVar->begin(), ed = FuncVar->end();
			it != ed;
			++it) {
			ret.Data.push_back(it->second.value);
		}
		return ret;
	}

	double setVariables(vector<double>& args) {
		int index = 0;
		for (auto it = FuncVar->begin(); it != FuncVar->end(); ++it) {
#ifdef OD_EXCEP_ON
			if (args[index] > it->second.l_high )
			{
				it->second.value = it->second.l_high;
				//throw exception("Variable Value Out of Limit!");
			}
			if (args[index] < it->second.l_low) {
				it->second.value = it->second.l_low;
			}
			else 
#endif //ERROR
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
	double operator()(vector<double>& args) {
		setVariables(args);
		return CalcFuncWithVar();
	}
	double operator()(Vector& args) {
		return operator()(args.Data);
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

#ifdef OD_PRINT_PROCESS
		while(Cur) {
			if (Cur->tt == TokenType::real) {
				cout << Cur->container.value;
			}
			else cout << *(Cur->container.content);
			Cur = Cur->Next;
		}
		cout << '\n';
#endif
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
#ifdef OD_PRINT_PROCESS
			Cur = HEAD;
			while (Cur) {
				if (Cur->tt == TokenType::real) {
					cout << Cur->container.value;
				}
				else cout << *(Cur->container.content);
				Cur = Cur->Next;
			}
			cout << '\n';
#endif
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
#ifdef OD_PRINT_PROCESS
			cout << '\n';
#endif
		}

		return Cur->container.value;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public: 
	Vector Gradient(Vector args)
	{
		//Set Base Point
		try {
			setVariables(args.Data);
			return Gradient();
		}
		catch (...) {
			throw;
		}
	}

//	Vector Gradient()
//	{
//		try {
//			Vector ret(FuncVar->size());
//
//			//Calculate Base Point Value
//			double base, diff, origin;
//			int index = 0;
//
//			for (auto it = FuncVar->begin(), ed = FuncVar->end(); it != ed; ++it) {
//				//Calculate Differs.
//				origin = it->second.value;
//				it->second.value -= FLT_EPSILON;
//				if (it->second.value < it->second.l_low) {
//					it->second.value = it->second.l_low;
//				}
//				base = CalcFuncWithVar();
//				it->second.value = origin;
//
//				it->second.value += FLT_EPSILON;
//				if (it->second.value > it->second.l_high) {
//					it->second.value = it->second.l_high;
//				}
//				
//				diff = CalcFuncWithVar() - base;
//				//TODO: Deviation Removal;
//				ret[index] = diff /(2* FLT_EPSILON);
//#ifdef OD_PRINT_PROCESS
//				cout << diff / FLT_EPSILON << ' ';
//#endif // OD_PRINT_PROCESS
//				it->second.value = origin;
//				++index;
//			}
//
//			return ret;
//		}
//		catch (...) {
//			throw;
//		}
//	}
	
	Vector Gradient()
	{
		try {
			Vector ret(FuncVar->size());

			//Calculate Base Point Value
			double base, diff, origin;
			int index = 0;

			for (auto it = FuncVar->begin(), ed = FuncVar->end(); it != ed; ++it) {
				//Calculate Differs.
				origin = it->second.value;
				bool inlow = true, inhigh = true;
				if (origin - H < it->second.l_low) {
					it->second.value = it->second.l_low;
					inlow = false;
				}
				else if (origin + H > it->second.l_high) {
					it->second.value = it->second.l_high;
					inhigh = false;
				}
				if (inlow&&inhigh) {
					it->second.value = origin - H;
					base = CalcFuncWithVar();
					it->second.value = origin + H;
					diff = CalcFuncWithVar() - base;
					//TODO: Deviation Removal;
					ret[index] = diff / (2 * H);
#ifdef OD_PRINT_PROCESS
					cout << diff / _EPSILON << ' ';
#endif // OD_PRINT_PROCESS
				}
				else if (!inlow) {
					base = CalcFuncWithVar();
					it->second.value = origin + H;
					diff = CalcFuncWithVar() - base;
					//TODO: Deviation Removal;
					ret[index] = diff / (H);
				}
				else if (!inhigh) {
					base = CalcFuncWithVar();
					it->second.value = origin - H;
					diff = base - CalcFuncWithVar();
					//TODO: Deviation Removal;
					ret[index] = diff / (H);
				}
				++index;
				it->second.value = origin;
			}

			return ret;
		}
		catch (...) {
			throw;
		}
	}

	Matrix Heissen(Vector args) {
		//Set Base Point
		try {
			setVariables(args.Data);
			return Heissen();
		}
		catch (...) {
			throw;
		}
	}

	Matrix Heissen() {
		try {
			Matrix ret(FuncVar->size(), FuncVar->size());
			
			//Calculate Base Point Value
			double base,diffx1, diffx2, diffx1x2;
			double origx1, origx2;
			double lx1, hx1, lx2, hx2;
			double dx1, dx2;
			int index1 = 0,index2 = 0;
			bool inhx1, inlx1, inhx2, inlx2;


			for (auto itx1 = FuncVar->begin(), ed = FuncVar->end(); itx1 != ed; ++itx1) {
				inhx1 = true; inlx1 = true; dx1 = 2 * H;
				origx1 = itx1->second.value;
				lx1 = origx1 - H;
				hx1 = origx1 + H;
				if (hx1> itx1->second.l_high) {
					hx1 = origx1;
					cout << "CAUTION!!! itx1 Value Out of High Bound  Adjusting" << endl;
					inhx1 = false;
					dx1 = H;
				}
				else if (lx1 < itx1->second.l_low) {
					lx1 = origx1;
					cout << "CAUTION!!! itx1 Value Out of Low Bound  Adjusting" << endl;
					inlx1 = false;
					dx1 = H;
				}

				index2 = 0;
				for (auto itx2 = FuncVar->begin(); itx2 != ed; ++itx2) {
					inhx2 = true; inlx2 = true; dx2 = 2 * H;
					origx2 = itx2->second.value;
					if (itx1 == itx2) {
						double midx2 = origx1;
						lx2 = origx1 - 2*H;
						hx2 = origx1 + 2*H;
						if (hx2 > itx2->second.l_high) {
							hx2 = origx1;
							midx2 = hx2 - 2 * H;
							lx2 = hx2 - 4 * H;
							cout << "CAUTION!!! itx2 Value Out of High Bound Adjusting" << endl;
						}
						else if (lx2 < itx2->second.l_low) {
							lx2 = origx1;
							midx2 = lx2 + 2 * H;
							hx2 = lx2 + 4 * H;
							cout << "CAUTION!!! itx2 Value Out of Low Bound  Adjusting" << endl;
						}
						/////////////////////////////////////////////////////////
						itx2->second.value = lx2;
						base = CalcFuncWithVar();
						itx2->second.value = midx2;
						diffx1 = CalcFuncWithVar();
						/////////////////////////////////////////////////////////
						diffx2 = diffx1;
						itx2->second.value = hx2;
						diffx1x2 = CalcFuncWithVar();
						/////////////////////////////////////////////////////////

											//TODO: Deviation Removal;
						double v1 = (diffx1x2 - diffx2);
						double v2 = (diffx1 - base);
						ret[index1][index2] = (v1 - v2) / (dx2*dx2);
#ifdef OD_PRINT_PROCESS
						cout << ret[index1][index2] << ' ';
#endif // OD_PRINT_PROCESS
						++index2;
						itx2->second.value = origx2;
					}
					else {
						lx2 = origx2 - H;
						hx2 = origx2 + H;
						if (hx2 > itx2->second.l_high) {
							hx2 = origx2;
							cout << "CAUTION!!! itx2 Value Out of High Bound Adjusting" << endl;
							inhx2 = false;
							hx2 = itx2->second.l_high;
							dx2 = H;
						}
						else if (lx2 < itx2->second.l_low) {
							lx2 = origx2;
							cout << "CAUTION!!! itx2 Value Out of Low Bound  Adjusting" << endl;
							inlx2 = false;
							dx2 = H;
						}
						/////////////////////////////////////////////////////////
						itx2->second.value = lx2;
						itx1->second.value = lx1;
						base = CalcFuncWithVar();
						itx1->second.value = hx1;
						diffx1 = CalcFuncWithVar();
						/////////////////////////////////////////////////////////
						itx2->second.value = hx2;
						itx1->second.value = lx1;
						diffx2 = CalcFuncWithVar();
						itx1->second.value = hx1;
						diffx1x2 = CalcFuncWithVar();
						/////////////////////////////////////////////////////////

						//TODO: Deviation Removal;
						double v1 = (diffx1x2 - diffx2);
						double v2 = (diffx1 - base);
						ret[index1][index2] = (v1 - v2) / (dx1*dx2);
#ifdef OD_PRINT_PROCESS
						cout << ret[index1][index2] << ' ';
#endif // OD_PRINT_PROCESS
						++index2;
						itx2->second.value = origx2;
					}
				}
#ifdef OD_PRINT_PROCESS
				cout << '\n';
#endif // OD_PRINT_PROCESS
				++index1;
				itx1->second.value = origx1;
			}
			return ret;
		}
		catch (...) {
			throw;
		}

	}
};

