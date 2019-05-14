#pragma once
#include <string>
using namespace std;
enum TokenType {
	op,
	var,
	real,
	unknown,
	total
};
struct FormTok {
	string content;
	enum TokenType tt;
};


//Transfer from FormTok to FormTokLink Help Caculate Formula
union FTLU{
	string * content;
	double value;
};
struct FormTokLink {
	FTLU container;
	enum TokenType tt;
	FormTokLink * Prev;
	FormTokLink * Next;
};