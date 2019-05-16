#pragma once
#include <string>
#include "Vector.h"
#include "Matrix.h"
#define NEWLINE "\r\n"

using namespace std;
class Recorder
{
public:
	static string  content;
	Recorder();
	static const char * Record() {
		return content.c_str();
	}

	static void Clear() {
		content.clear();
	}

	static void Record(const Matrix &l, string name)
	{
		content += name + "[" + to_string(l.rows) + "][" + to_string(l.cols)+"]:" + NEWLINE;
		for (int i = 0; i < l.rows; ++i) {
			for (int j = 0; j < l.cols; ++j) {
				content += " " + to_string(l[i][j]);
			}
			content += NEWLINE;
		}
	}

	static void Record(const Vector &l, string name)
	{
		content += name + " = [";
		for (int i = 0; i < l.dim(); ++i) {
			content += " " + to_string(l[i]);
		}
		content += (string)" ]" + NEWLINE;
	}

	static void Record(int v, string name)
	{
		content += name + " = " + to_string(v) + NEWLINE;
	}

	static void Record(double v, string name)
	{
		content += name + " = " + to_string(v) + NEWLINE;
	}

	static void Record(const string & msg)
	{
		content += msg + " ";
	}

	static void RecordLine(const string & msg)
	{
		content += msg + NEWLINE;
	}
};


