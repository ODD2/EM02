#pragma once
#include "Vector.h"
struct Matrix
{
	
	Matrix();
	Matrix(unsigned int rows, unsigned int cols);
	Matrix(unsigned int rows, unsigned int cols,const std::vector<Vector>& Data);
	Matrix(const std::vector<Vector>& mat);
	Matrix(const Vector& mat);
	Matrix(const Matrix& mat);
	Matrix(Matrix&& mat);
	Matrix operator=(const Matrix & mat);
	~Matrix();
	//ptrs
	std::vector<Vector> * rref = nullptr;
	std::vector<Vector> * inv = nullptr;
	static Vector getVector(const Matrix&  mat,int index) {
		if (index >= mat.cols)throw  std::exception("Out of Range!!");
		else
		{
			Vector ret(mat.rows);
			for (int i = 0; i < mat.rows; ++i) {
				ret[i] = mat[i][index];
			}
			return ret;
		}
	}
	//values
	unsigned int rows;
	unsigned int cols;

	double det = 0;

	bool noinv = false;
	bool detcalc = false;

	std::vector<Vector> Data;

	//access functions.
	Vector& operator [](std::size_t index) {
		return Data[index];
	}
	Vector operator [](std::size_t index) const {
		return Data[index];
	}
	
	//tool functions.
	static Matrix Identity(int n,double v = 1.0) {
		Matrix ret(n, n);
		for (int i = 0; i < n; ++i) {
			ret[i][i] = v;
		}
		return ret;
	}

	//single functions.
	void Det();
	//void Inv();
};

union MatFunc {
	void * fp;
	Matrix(*one)(const Matrix&);
	Matrix(*two)(const Matrix&, const  Matrix&);
	Matrix(*three)(const Matrix&, const  Matrix&, const Matrix&);
};

Vector getEigenVector(Matrix &l);
std::vector<std::vector<std::string>> solve(const Matrix &l, const Matrix &r);
Matrix solve0(const Matrix &l, const Matrix &r);
std::vector<Vector> eigenValue(Matrix &l);
Matrix addm(const Matrix & l, const Matrix & r);
Matrix subm(const Matrix & l, const Matrix & r);
Matrix multm(const Matrix & l, const Matrix & r);
Vector multm(const Matrix & l, const Vector & r);
Matrix transpose(const Matrix &m);
Matrix powerMethod(const Matrix &l);
Matrix leastsquare(Matrix &l, Matrix &r);
Matrix guass(Matrix &l);
Matrix inverse(Matrix &l);
Matrix adjoint(Matrix &l);
double determ(Matrix &l);
bool independent(Matrix &m);
int rank(Matrix &m);

Matrix operator *(const Matrix &l, const Matrix&r);
Matrix operator *(const Matrix &l, const Vector&r);
Matrix operator*(const Matrix & l, double r);
Matrix operator*(double l, const Matrix & r);
Matrix operator +(const Matrix &l, const Matrix&r);
Matrix operator -(const Matrix &l, const Matrix&r);
Matrix operator /(const Matrix &l, double r);



extern std::map<std::string, MatFunc> MatrixOps;