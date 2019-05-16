#include "Recorder.h"
string Recorder::content = "";


Recorder::Recorder()
{
}

 /*void Recorder::Record(const Matrix &l , string name)
{
	content += name + " =" + NEWLINE;
	for (int i = 0; i < l.rows; ++i) {
		for (int j = 0; j < l.cols; ++j) {
			content +=  " " + to_string(l[i][j]);
		}
		content += NEWLINE;
	}
}

void Recorder::Record(const Vector &l, string name)
{
	content += name + " = [";
	for (int i = 0; i < l.dim(); ++i) {
		content += " " + to_string(l[i]);
	}
	content +=  (string)" ]" + NEWLINE;
}

void Recorder::Record(int v, string name)
{
	content += name + " = " + to_string(v) + NEWLINE;
}

void Recorder::Record(double v, string name)
{
	content += name + " = " + to_string(v) + NEWLINE;
}

void Recorder::Record(const string & content)
{
	Recorder::content += content + " ";
}

void Recorder::RecordLine(const string & content)
{
	Recorder::content +=  content + NEWLINE;
}*/


