#pragma once
#include<vector>
#include<string>
#include <map>
class Matrix;
//定義向量資料結構
struct Vector
{
	std::vector<double> Data;

	Vector() {

	}

	explicit Vector(int dimension):Data(dimension) {
		
	}

	Vector(std::vector<double> v) :Data(v) {
	}

	static Matrix toMatrix(const Vector & v);


	const unsigned int dim() const {
		return Data.size();
	}

	double& operator [](std::size_t index) {
		return Data[index];
	}

	double operator [](std::size_t index) const{
		return Data[index];
	}
	
	void scale(double magnitude) {
		for (int i = 0; i < Data.size(); ++i) {
			Data[i] *= magnitude;
		}
	}
	
};

union VecFunc {
	void * fp;
	Vector(*one)(const Vector&);
	Vector(*two)(const Vector&, const  Vector&);
	Vector(*three)(const Vector&, const  Vector&, const Vector&);
};

extern std::map<std::string, VecFunc> VectorOps;


Vector add(const Vector& l, const Vector& r);
Vector sub(const Vector & l, const Vector & r) throw(std::exception);
Vector mult(const Vector& l, const Vector& r);
//Vector dot(const Vector& l, const Vector& r);
double dot(const Vector& l, const Vector& r);
Vector cross(const Vector&l, const Vector &r);
Vector comp(const Vector&l, const Vector&r);
Vector proj(const Vector&l, const Vector&r);
Vector triangle(const Vector &l, const Vector &r);
Vector angle(const Vector &l, const Vector &r);
Vector planeNormal(const Vector &l, const Vector &r);
Vector lengthv(const Vector& target);
double length(const Vector& target);
Vector nrmlz(const Vector &r);
bool parallel(const Vector &l,const Vector &r);
bool orthogonal(const Vector &l, const Vector &r);
bool independent(const Vector &l, const Vector &r);


Vector operator+(const Vector&l, const Vector&r);
Vector operator-(const Vector&l, const Vector&r);
Vector operator*(double l, const Vector&r);
Vector operator/(const Vector&l, double r);


std::vector<Vector> gram_schmidt(std::vector<Vector> ret);
std::vector<Vector> gram_schmidt(Vector l, ...);



