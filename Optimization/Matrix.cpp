#include "Matrix.h"
#include <iostream>
#include <stdarg.h>
#include <exception>
#include <math.h>
#define DIM_ERR throw std::exception("Matrix Dimension Error", -1);
#define WID_ERR throw std::exception("Matrix Width Error", -1);
#define CON_ERR throw std::exception("Matrix Constraint Error",-1);
#define SQR_ERR throw std::exception("Matrix Not Square",-1);
#define DET_ERR throw std::exception("Matrix No Determinant",-1);
#define NO_INVERSE throw std::exception("No Inverse for this Matrix",-1);
#define IMG_EIGEN throw std::exception("Imagine Eigen Value",-1);
#define _ERR(x) throw std::exception(x,-1);
#define ZERO 0.0000001
#define EIGEN_TOLERANCE 0.0001 
#define _PI 3.1415926
typedef std::pair<int, std::pair<Vector, Vector>> IVV;
typedef std::pair<int, Vector> IV;

Matrix::Matrix():rows(0),cols(0)
{
}

Matrix::Matrix(unsigned int rows, unsigned int cols):rows(rows),cols(cols),Data(rows) {
	for (int i = 0; i < rows; i++) {
		Data[i] = Vector(cols);
	}
}

Matrix::Matrix(unsigned int rows, unsigned int cols, const std::vector<Vector>& Data):
	rows(rows),cols(cols),Data(Data) 
{

}

Matrix::Matrix(const std::vector<Vector>& mat) {
	Data = mat;
	rows = mat.size();
	if (rows)
		cols = mat[0].dim();
}

Matrix::Matrix(const Vector & mat)
	:Data(mat.dim()),cols(1),rows(mat.dim())
{
	for (int i = 0; i < rows; ++i) {
		Data[i].Data.push_back(mat[i]);
	}
}

//copy constructor
Matrix::Matrix(const Matrix& mat) {
	Data = mat.Data;
	rows = mat.rows;
	cols = mat.cols;
	det = mat.det;
	noinv = mat.noinv;
	if (mat.rref != nullptr) {
		rref = new std::vector<Vector>(*(mat.rref));
	}
	if (mat.inv != nullptr) {
		inv = new std::vector<Vector>(*(mat.inv));
	}
}

//move constructor
Matrix::Matrix(Matrix&& mat) {
	if (&mat != this) {
		Data = std::move(mat.Data);
		rows = mat.rows;
		cols = mat.cols;
		det = mat.det;
		noinv = mat.noinv;
		if (mat.rref != nullptr) {
			rref = mat.rref;
			mat.rref = nullptr;
		}
		if (mat.inv != nullptr) {
			inv = mat.inv;
			mat.inv = nullptr;
		}
	}
}

Matrix Matrix::operator=(const Matrix & mat) {
	Data = mat.Data;
	rows = mat.rows;
	cols = mat.cols;
	det = mat.det;
	noinv = mat.noinv;
	if (mat.rref != nullptr) {
		rref = new std::vector<Vector>(*(mat.rref));
	}
	if (mat.inv != nullptr) {
		inv = new std::vector<Vector>(*(mat.inv));
	}
	return *this;
}

Matrix::~Matrix() {
	if (rref != nullptr) {
		delete rref;
	}
	if (inv != nullptr) {
		delete inv;
	}
}

void Matrix::Det() {
	//Deal only square matrices
	if (detcalc)return;
	else if (rows != cols || noinv)
	{
		det = 0;
		detcalc = true;
		return;
	}
	else {
		detcalc = true;
		//If not generated;
		int DIM = cols;
		//Determinant Preparation;
		int swaps = 0;
		double _det = 1;

		//Sort
		std::vector<IV> sorted;
		for (int i = 0; i < DIM; ++i) {
			bool insert = false;
			IV entity; entity.second = Data[i];
			for (int j = 0; j < DIM; ++j) {
				if (fabs(Data[i][j]) > ZERO) {
					entity.first = j;
					for (int k = 0, lk = sorted.size(); k < lk; ++k) {
						if (j < sorted[k].first) {
							sorted.insert(sorted.begin() + k, std::move(entity));
							//Counting swaps;
							swaps += (lk - k) % 2;
							insert = true;
							break;
						}
					}
					if (!insert) {
						sorted.push_back(std::move(entity));
						insert = true;
					}
					break;
				}
				else Data[i][j] = 0;
			}
			if (!insert) {
				det = 0;
				return;
			}
		}

		//Elimination && Determinating
		for (int r = 0; r < DIM; ++r) {

			Vector & rowVec = sorted[r].second;
			//Skipping leading zeros
			int c = sorted[r].first;

			//Inverse Error Condition.
			if (c != r || c == DIM) {
				det = 0;
				return;
			}

			//Normalize
			double divident = rowVec[c];
			_det *= divident; //det.
			for (int i_c = c; i_c < DIM; ++i_c) {
				rowVec[i_c] /= divident;
			}
			//Eliminate
			for (int i_r = r + 1; i_r < DIM; ++i_r) {
				Vector & _rowVec = sorted[i_r].second;

				double multiplier = _rowVec[c];
				if (fabs(multiplier) < ZERO) {
					_rowVec[c] = 0.0;
					continue;
				}
				bool leadZero = true;
				for (int i_c = c; i_c < DIM; ++i_c) {
					_rowVec[i_c] -= multiplier * rowVec[i_c];
					//Deviation Removal
					if (leadZero && fabs(_rowVec[i_c]) < ZERO) {
						sorted[i_r].first += 1;
						_rowVec[i_c] = 0.0;
					}
					else leadZero = false;
				}
			}

			//Sort
			if (r + 1 != DIM) {
				int _swap = 0;
				int front = r, back = DIM, pivot = r + 1;
				bool hasreq = false;
				while (1) {
					while (sorted[++front].first == pivot && (hasreq = true) && front + 1 < DIM);
					while (sorted[--back].first != pivot && back > front);
					if (back <= front) break;
					std::swap(sorted[front], sorted[back]);
					++_swap;
				}
				if (!hasreq) {
					//Error cannot find row with pivot at  r+1;
					det = 0;
					return;
				}
				else {
					swaps += _swap % 2;
				}
			}
		}

		//Save determinant
		det = swaps % 2 ? -(_det) : _det;
		return;
	}
}

Matrix addm(const Matrix & l, const Matrix & r) {
	if (l.rows != r.rows) DIM_ERR
	else if ( l.cols != r.cols) WID_ERR
	else {
		Matrix ret = l;
		for (int i = 0, j = ret.rows; i < j; ++i) {
			for (int k = 0, l = ret.cols; k < l; ++k) {
				ret[i][k] += r[i][k];
			}
		}
		return ret;
	}
}

Matrix subm(const Matrix & l, const Matrix & r) {
	if (l.rows != r.rows) DIM_ERR
	else if (l.cols != r.cols) WID_ERR
	else {
		Matrix ret = l;
		for (int i = 0, j = ret.rows; i < j; ++i) {
			for (int k = 0, l = ret.cols; k < l; ++k) {
				ret[i][k] -= r[i][k];
			}
		}
		return ret;
	}
}

Matrix multm(const Matrix & l, const Matrix & r) {
	if (l.cols != r.rows) CON_ERR
	else {
		Matrix ret(l.rows, r.cols);
		for (int retc = 0; retc < ret.cols; ++retc) {
			for (int retr = 0; retr < ret.rows; ++retr) {
				ret[retr][retc] = 0;
				for (int lc = 0; lc < l.cols; ++lc) {
					ret[retr][retc] += l[retr][lc] * r[lc][retc];
				}
			}
		}
		return ret;
	}
}

Vector multm(const Matrix & l, const Vector & r)
{
	if (l.cols != r.dim()) CON_ERR
	else {
		Vector ret(l.rows);
		for (int retr = 0; retr < l.rows; ++retr) {
			ret[retr] = 0;
			for (int lc = 0; lc < l.cols; ++lc) {
				ret[retr] += l[retr][lc] * r[lc];
			}
		}
		return ret;
	}
}

Matrix transpose(const Matrix &m) {
	Matrix ret(m.cols, m.rows);
	for (int r = 0; r < m.rows; ++r) {
		for (int c = 0; c < m.cols; ++c) {
			ret[c][r] = m[r][c];
		}
	}
	return ret;
}

bool independent( Matrix &m) {
	std::vector<Vector> * rref;
	if (m.cols > m.rows) return false;
	else if (m.rref != nullptr) {
		rref = m.rref;
	}
	else {
		guass(m);
		rref = m.rref;
	}
	int COLS = m.cols;
	for (int i = 0; i < COLS; ++i) {
		if ((*rref)[i][i] != 1)return false;
	}
	return true;
}

int rank(Matrix &m) {
	int rk = 0;
	if (m.rref == nullptr) {
		guass(m);
	}
	std::vector<Vector> * rref = m.rref;
	for (int r = 0; r < m.rows; ++r) {
		Vector & rowVec = (*rref)[r];
		for (int c = 0; c < m.cols; ++c) {
			if (rowVec[c] != 0) {
				rk += 1;
				break;
			}
		}
	}
	return rk;
}

Matrix operator*(const Matrix & l, const Matrix & r)
{
	return multm(l, r);
}
Matrix operator*(const Matrix & l, double r)
{
	Matrix ret = l;
	for (int i = 0; i < l.rows; ++i) {
		for (int j = 0; j < l.cols; ++j) {
			ret[i][j] *= r;
		}
	}
	return ret;
}
Matrix operator*(double l, const Matrix & r)
{
	Matrix ret = r;
	for (int i = 0; i < r.rows; ++i) {
		for (int j = 0; j < r.cols; ++j) {
			ret[i][j] *= l;
		}
	}
	return ret;
}
Matrix operator*(const Matrix & l, const Vector & r)
{
	return multm(l, r);
}
Matrix operator+(const Matrix & l, const Matrix & r)
{
	return addm(l, r);
}
Matrix operator-(const Matrix & l, const Matrix & r)
{
	return subm(l, r);
}
Matrix operator/(const Matrix & l, double r)
{
	Matrix ret = l;
	for (int i = 0; i < l.rows; ++i) {
		for (int j = 0; j < l.cols; ++j) {
			ret[i][j] /= r;
		}
	}
	return ret;
}

int comp_1(const void * a,const  void * b) {
	return ((std::pair<int, Vector>*)a)->first  -  ((std::pair<int, Vector>*)b)->first;
}

Matrix guass(Matrix &l) {
	int COLS = l.cols;
	int ROWS = l.rows;
	Matrix sortedM; sortedM.rows = l.rows;  sortedM.cols = l.cols;
	if (l.rref!=nullptr) {
		sortedM.Data = *l.rref;
		return sortedM;
	}
	//Sort
	std::vector<std::pair<int, Vector>> sorted;
	for (int i = 0; i < l.rows; ++i) {
		bool insert = false;
		for (int j = 0; j < l.cols; ++j) {
			if (fabs(l[i][j]) > ZERO) {
				std::pair<int, Vector> entity(j, l[i]);
				for (int k = 0, lk = sorted.size(); k < lk; ++k) {
					if (j < sorted[k].first) {
						sorted.insert(sorted.begin() + k, std::move(entity));
						insert = true;
						break;
					}
				}
				if (!insert) {
					sorted.push_back(std::move(entity));
					insert = true;
				}
				break;
			}
			else l[i][j] = 0;
		}
		if (!insert) {
			sorted.push_back(std::pair<int, Vector>(l.cols, l[i]));
		}
	}

	
	//Elimination
	int limit = l.rows < l.cols ? l.rows : l.cols;
	for (int r = 0; r < limit; ++r) {
		
		//Skipping leading zeros
		Vector & rowVec = sorted[r].second;
		int c = sorted[r].first;
		if (c == COLS)break;
	
		//Eliminate
		for (int i_r = r + 1; i_r < ROWS; ++i_r) {
			Vector & _rowVec = sorted[i_r].second;
			double multiplier = _rowVec[c]/rowVec[c];
			if (multiplier == 0) {
				//Afterward vectors  has larger equal leading zeros.
				break;
			}
			bool leadZero = true;
			for (int i_c = c; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0.0;
					if(leadZero) sorted[i_r].first += 1;
				}
				else leadZero = false;
			}
		}

		//Normalize
		double divident = rowVec[c];
		for (int i_c = c; i_c < COLS; ++i_c) {
			rowVec[i_c] /= divident;
		}
		//Sort
		if(r + 1 != ROWS )	std::qsort(&sorted[r + 1], ROWS - 1 - r, sizeof(std::pair<int, Vector>), comp_1);
	}

	//CleanUp
	for (int i_r = ROWS - 1; i_r >= 0; --i_r) {
		if (sorted[i_r].first == COLS) continue;
		Vector & base = sorted[i_r].second;
		int leading0 = sorted[i_r].first;
		for (int i_rr = i_r - 1; i_rr >= 0; --i_rr) {
			Vector & target = sorted[i_rr].second;
			double multiplier = target[leading0];
			//Deviation Removal & Multiplier Zero
			if (fabs(multiplier) < ZERO) {
				target[leading0] = 0;
				continue;
			}
			for (int i_c = leading0; i_c < COLS; ++i_c) {
				target[i_c] -= multiplier * base[i_c];
				//Deviation Removal
				if (fabs(target[i_c]) < ZERO) {
					target[i_c] = 0;
				}
			}
		}
	}


	//Create Result
	for (int i = 0; i < sorted.size(); ++i) {
		sortedM.Data.push_back(std::move(sorted[i].second));
	}
	//Save RREF for further use.
	l.rref = new std::vector<Vector>(sortedM.Data);
	return sortedM;
}

std::vector<std::vector<std::string>> solve(const Matrix &l, const Matrix &r) {
	int COLS = l.cols;
	int ROWS = l.rows;
	int ACOLS = r.cols;
	int AROWS = r.rows;
	if(ROWS!=AROWS)CON_ERR

	//Sort
	std::vector<IVV> sorted;
	for (int i = 0; i < ROWS; ++i) {
		bool insert = false;
		IVV entity; entity.second.first = l[i]; entity.second.second = r[i];
		for (int j = 0; j < COLS; ++j) {
			if (fabs(l[i][j]) > ZERO) {
				entity.first = j;
				for (int k = 0, lk = sorted.size(); k < lk; ++k) {
					if (j < sorted[k].first) {
						sorted.insert(sorted.begin() + k, std::move(entity));
						insert = true;
						break;
					}
				}
				if (!insert) {
					sorted.push_back(std::move(entity));
					insert = true;
				}
				break;
			}
			else l[i][j] = 0;
		}
		if (!insert) {
			entity.first = COLS;
			sorted.push_back(std::move(entity));
		}
	}

	//Elimination
	int limit = ROWS < COLS ? ROWS : COLS;
	for (int r = 0; r < limit; ++r) {
		
		//Skipping leading zeros
		Vector & rowVec = sorted[r].second.first;
		Vector & ansVec = sorted[r].second.second;
		int c = sorted[r].first;
		if (c == COLS)break;
	
		//Eliminate
		for (int i_r = r + 1; i_r < ROWS; ++i_r) {
			Vector & _rowVec = sorted[i_r].second.first;
			Vector & _ansVec = sorted[i_r].second.second;
			
			double multiplier = _rowVec[c]/ rowVec[c];
			if (multiplier == 0) {
				//Afterward vectors  has larger equal leading zeros.
				break;
			}
			bool leadZero = true;
			for (int i_c = c; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0.0;
					if (leadZero) sorted[i_r].first += 1;
				}
				else leadZero = false;
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0.0;
				}
			}
		}

		//Normalize
		double divident = rowVec[c];
		for (int i_c = c; i_c < COLS; ++i_c) {
			rowVec[i_c] /= divident;
		}
		for (int i_c = 0; i_c < ACOLS; ++i_c) {
			ansVec[i_c] /= divident;
		}


		//Sort
		if (r + 1 != ROWS)	std::qsort(&sorted[r + 1], ROWS - 1 - r, sizeof(IVV), comp_1);
	}

	//CleanUp
	for (int i_r = ROWS - 1; i_r >= 0; --i_r) {
		if (sorted[i_r].first == COLS) continue;

		Vector & rowVec = sorted[i_r].second.first;
		Vector & ansVec = sorted[i_r].second.second;
		int leading0 = sorted[i_r].first;

		for (int i_rr = i_r - 1; i_rr >= 0; --i_rr) {
			Vector & _rowVec = sorted[i_rr].second.first;
			Vector & _ansVec = sorted[i_rr].second.second;
			double multiplier = _rowVec[leading0];
			//Deviation Removal & Multiplier Zero
			if (fabs(multiplier) < ZERO) {
				_rowVec[leading0] = 0;
				continue;
			}
			for (int i_c = leading0; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0;
				}
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0;
				}
			}
		}
	}

	//Create Result
	std::vector<std::vector<std::string>> result;
	for (int c = 0; c < ACOLS; ++c) {
		std::vector<std::string> ans;
		for (int r = 0; r < AROWS; ++r) {
			std::string formula;
			Vector & reqVec = sorted[r].second.first;
			for (int ac = 0; ac < COLS; ++ac) {
				if (reqVec[ac] != 0) {
					if (formula.size() != 0&&reqVec[ac] > 0) {
							formula += "+";
					}
					if (reqVec[ac] != 1)
						formula += std::to_string(reqVec[ac]) + "*";

					formula += "(x" + std::to_string(ac) + ")";
				}
			}
			double value = sorted[r].second.second[c];
			if (formula.size() == 0) {
				if (value != 0) {
					ans.clear();
					ans.push_back("undefined");
					break;
				}
			}
			else  {
				ans.push_back(formula + " = " + std::to_string(value));
			}
		}
		result.push_back(std::move(ans));
	}
	return result;
}

Matrix solve0(const Matrix &l, const Matrix &r) {
	int COLS = l.cols;
	int ROWS = l.rows;
	int ACOLS = r.cols;
	int AROWS = r.rows;
	if (ROWS != AROWS)CON_ERR

		//Sort
		std::vector<IVV> sorted;
	for (int i = 0; i < ROWS; ++i) {
		bool insert = false;
		IVV entity; entity.second.first = l[i]; entity.second.second = r[i];
		for (int j = 0; j < COLS; ++j) {
			if (fabs(l[i][j]) > ZERO) {
				entity.first = j;
				for (int k = 0, lk = sorted.size(); k < lk; ++k) {
					if (j < sorted[k].first) {
						sorted.insert(sorted.begin() + k, std::move(entity));
						insert = true;
						break;
					}
				}
				if (!insert) {
					sorted.push_back(std::move(entity));
					insert = true;
				}
				break;
			}
			else l[i][j] = 0;
		}
		if (!insert) {
			entity.first = COLS;
			sorted.push_back(std::move(entity));
		}
	}

	//Elimination
	int limit = ROWS < COLS ? ROWS : COLS;
	for (int r = 0; r < limit; ++r) {

		//Skipping leading zeros
		Vector & rowVec = sorted[r].second.first;
		Vector & ansVec = sorted[r].second.second;
		int c = sorted[r].first;
		if (c == COLS)break;

		//Eliminate
		for (int i_r = r + 1; i_r < ROWS; ++i_r) {
			Vector & _rowVec = sorted[i_r].second.first;
			Vector & _ansVec = sorted[i_r].second.second;

			double multiplier = _rowVec[c] / rowVec[c];
			if (multiplier == 0) {
				//Afterward vectors  has larger equal leading zeros.
				break;
			}
			bool leadZero = true;
			for (int i_c = c; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0.0;
					if (leadZero) sorted[i_r].first += 1;
				}
				else leadZero = false;
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0.0;
				}
			}
		}

		//Normalize
		double divident = rowVec[c];
		for (int i_c = c; i_c < COLS; ++i_c) {
			rowVec[i_c] /= divident;
		}
		for (int i_c = 0; i_c < ACOLS; ++i_c) {
			ansVec[i_c] /= divident;
		}


		//Sort
		if (r + 1 != ROWS)	std::qsort(&sorted[r + 1], ROWS - 1 - r, sizeof(IVV), comp_1);
	}

	//CleanUp
	for (int i_r = ROWS - 1; i_r >= 0; --i_r) {
		if (sorted[i_r].first == COLS) continue;

		Vector & rowVec = sorted[i_r].second.first;
		Vector & ansVec = sorted[i_r].second.second;
		int leading0 = sorted[i_r].first;

		for (int i_rr = i_r - 1; i_rr >= 0; --i_rr) {
			Vector & _rowVec = sorted[i_rr].second.first;
			Vector & _ansVec = sorted[i_rr].second.second;
			double multiplier = _rowVec[leading0];
			//Deviation Removal & Multiplier Zero
			if (fabs(multiplier) < ZERO) {
				_rowVec[leading0] = 0;
				continue;
			}
			for (int i_c = leading0; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0;
				}
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0;
				}
			}
		}
	}

	//Create Result
	Matrix ret(l.cols, r.rows);
	for (int c = 0; c < ACOLS; ++c) {
		for (int r = 0; r < AROWS; ++r) {
			Vector & reqVec = sorted[r].second.first;
			double value = sorted[r].second.second[c];
			if (sorted[r].first == COLS ) {
				if ( value != 0) {
					throw std::exception("Error Equation Cannot Solve");
				}
				else continue;
			}
			else {
				ret[sorted[r].first][c] = value;
			}
		}
	}
	return ret;
}


//CAUTION!!! This Function Deals ONLY SQUARE Matrices.
Matrix inverse( Matrix &l) {
	//Deal only square matrices
	if (l.rows != l.cols)
	{
		l.det = 0;
		l.detcalc = true;
		SQR_ERR
	}
	//If singular
	else if(l.noinv) NO_INVERSE
	//If generated Inverse.
	else if (l.inv != nullptr) {
		Matrix ret; 
		ret.rows = l.rows; 
		ret.cols = l.cols;
		ret.Data = *l.inv;
		return ret;
	}

	
    //If not generated;
	int COLS = l.cols;
	int ROWS = l.rows;
	int ACOLS = ROWS;
	int AROWS = ROWS;
	const Matrix r = Matrix::Identity(ROWS);
	//Determinant Preparation;
	l.detcalc = true; //executing this function must result a determinant.
	int swaps = 0;
	double det = 1;



	//Sort
	std::vector<IVV> sorted;
	for (int i = 0; i < ROWS; ++i) {
		bool insert = false;
		IVV entity; entity.second.first = l[i]; entity.second.second = r[i];
		for (int j = 0; j < COLS; ++j) {
			if (fabs(l[i][j]) > ZERO) {
				entity.first = j;
				for (int k = 0, lk = sorted.size(); k < lk; ++k) {
					if (j < sorted[k].first) {
						sorted.insert(sorted.begin() + k, std::move(entity));
						//Counting swaps;
						swaps += (lk - k) % 2;
						insert = true;
						break;
					}
				}
				if (!insert) {
					sorted.push_back(std::move(entity));
					insert = true;
				}
				break;
			}
			else l[i][j] = 0;
		}
		if (!insert) {
			l.noinv = true;
			l.det = 0;
			NO_INVERSE
		}
	}

    //Elimination && Determinating
	int limit = ROWS;
	for (int r = 0; r < limit; ++r) {

		Vector & rowVec = sorted[r].second.first;
		Vector & ansVec = sorted[r].second.second;
		//Skipping leading zeros
		int c = sorted[r].first;

		//Inverse Error Condition.
		if (c != r || c == COLS) {
			l.noinv = true;
			l.det = 0;
			NO_INVERSE
		}


		//Eliminate
		for (int i_r = r + 1; i_r < ROWS; ++i_r) {
			Vector & _rowVec = sorted[i_r].second.first;
			Vector & _ansVec = sorted[i_r].second.second;

			double multiplier = _rowVec[c]/rowVec[c];
			if (fabs(multiplier) < ZERO) {
				_rowVec[c] = 0.0;
				continue;
			}
			bool leadZero = true;
			for (int i_c = c; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (leadZero && fabs(_rowVec[i_c]) < ZERO) {
					sorted[i_r].first += 1;
					_rowVec[i_c] = 0.0;
				}
				else leadZero = false;
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0.0;
				}
			}
		}

		//Normalize
		double divident = rowVec[c];
		det *= divident; //det.
		for (int i_c = c; i_c < COLS; ++i_c) {
			rowVec[i_c] /= divident;
		}
		for (int i_c = 0; i_c < ACOLS; ++i_c) {
			ansVec[i_c] /= divident;
		}

		//Sort
		//OLD Method
		//if (r + 1 != ROWS)	std::qsort(&sorted[r + 1], ROWS - 1 - r, sizeof(IVV), comp_1);

		//Base on the Sqaure and Inverse Criterion Condition.
		//Sorting can be easy and straight forward.
		//NEW Method
		if (r + 1 != ROWS) {
			int _swap = 0;
			int front = r, back = ROWS, pivot = r + 1;
			bool hasreq = false;
			while (1) {
				while (sorted[++front].first == pivot && (hasreq = true) && front + 1 < ROWS);
				while (sorted[--back].first != pivot && back > front);
				if (back <= front) break;
				std::swap(sorted[front], sorted[back]);
				++_swap;
			}
			if (!hasreq) {
				//Error cannot find row with pivot at  r+1;
				l.noinv = true;
				l.det = 0;
				NO_INVERSE
			}
			else {
				swaps += _swap % 2;
			}
		}
	}

	//CleanUp
	for (int i_r = ROWS - 1; i_r >= 0; --i_r) {
		if (sorted[i_r].first == COLS) continue;

		Vector & rowVec = sorted[i_r].second.first;
		Vector & ansVec = sorted[i_r].second.second;
		int leading0 = sorted[i_r].first;

		for (int i_rr = i_r - 1; i_rr >= 0; --i_rr) {
			Vector & _rowVec = sorted[i_rr].second.first;
			Vector & _ansVec = sorted[i_rr].second.second;
			double multiplier = _rowVec[leading0];
			if (fabs(multiplier) < ZERO) {
				_rowVec[leading0] = 0;
				continue;
			}
			for (int i_c = leading0; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0;
				}
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0;
				}
			}
		}
	}

	
	//Create Result
	Matrix result;
	result.cols = COLS;
	result.rows = ROWS;
	//Save inverse
	l.inv = new std::vector<Vector>;
	for (int i = 0; i < ROWS; ++i)
	{
		Vector & trg = sorted[i].second.second;
		result.Data.push_back(trg);
		l.inv->push_back(std::move(trg));
	}

	//Save determinant
	l.det = swaps % 2 ? -det : det;

	//Save rref if not exist
	if (l.rref == nullptr) {
		l.rref = new std::vector<Vector>(Matrix::Identity(ROWS).Data);
	}
	
	return result;
}

double determ(Matrix &l) {
	if (l.rows != l.cols) {
		l.detcalc = true;
		CON_ERR
	}
	else if (l.detcalc) return l.det;
	else {
		try
		{
			//Not yet generated det and inverse.
			inverse(l);
		}
		catch (const std::exception& e) {}
		return l.det;
	}
}

Matrix adjoint(Matrix &l) {
	if (l.cols != l.rows)CON_ERR
	else{
		int DIM = l.rows;
		double det = determ(l);
		if (det == 0) {
			Matrix ret(DIM, DIM);
			Matrix tmp(DIM - 1, DIM - 1);
			//Create Cofactor
			for (int r = 0; r < DIM; ++r) {
				for (int c = 0; c < DIM; ++c) {
					int tilt_r = 0;
					//Init tmp mat;
					tmp.detcalc = false;
					for (int i_r = 0; i_r < DIM; ++i_r) {
						if (i_r == r) {
							tilt_r = 1;
							continue;
						}
						int tilt_c = 0;
						for (int i_c = 0; i_c < DIM; ++i_c) {
							if (i_c == c)
							{
								tilt_c = 1;
								continue;
							}
							tmp[i_r - tilt_r][i_c - tilt_c] = l[i_r][i_c];
						}
					}
					tmp.Det();
					ret[r][c] = (r + c) % 2 ? -tmp.det : tmp.det;
				}
			}
			return ret;
		}
		else {
			Matrix id = Matrix::Identity(DIM,det);
			Matrix inv; inv.rows = DIM; inv.cols = DIM;
			inv.Data = *l.inv;
			return multm(inv, id);
		}
	}
}

Matrix leastsquare(Matrix &l, Matrix &r) {
	//Copy
	Matrix n_l = l;
	//n_l.cols += 1;
	////Insert Constant 
	//for (auto i = 0; i < n_l.rows; ++i) {
	//	n_l.Data[i].Data.push_back(1);
	//}
	//Calculate
	Matrix trans = transpose(n_l);
	//Vector res;
	Matrix result = multm(inverse(multm(trans, n_l)), multm(trans, r));
	/*for (int i = 0; i < result.rows; ++i) {
		res.Data.push_back(result[i][0]);
	}*/
	return result;
}

Matrix powerMethod(const Matrix &l) {
	if (l.cols != l.rows)  CON_ERR
	else {
		int DIM = l.cols;
		Matrix initVec(l.rows,1);
		Matrix A = l.Data;
		for (int i = 0; i < DIM; ++i) {
			initVec[i][0] = 1;
		}
		Matrix newVec = multm(A, initVec);

		for (int times = 1000; times > 0	; --times) {
			newVec = multm(A, newVec);
			//normalize
			double length = 0;
			for (int i = 0; i < DIM; ++i) {
				double &v = newVec[i][0];
				if (abs(v) < ZERO) {
					v = 0;
				}
				else {
					length += pow(v, 2);
				}
			}
			length = pow(length, 0.5);
			for (int i = 0; i < DIM; ++i) {
				newVec[i][0] /= length;
			}
		}

		//Normalize
		double length = 0;
		for (int i = 0; i < DIM; ++i) {
			double &v = newVec[i][0];
			if (abs(v) < ZERO) {
				v = 0;
			}
			else {
				length += pow(v, 2);
			}
		}
		length = pow(length, 0.5);
		for (int i = 0; i < DIM; ++i) {
			newVec[i][0] /= length;
		}
		//Constructing result
		double eigen = 0;
		Matrix eigenVec = newVec;
		newVec = multm(A, newVec);
		//find eigenvalue
		for (int i = 0, base = 0; i < DIM; ++i)
			 if(abs(newVec[i][0]) > ZERO)
				 eigen = newVec[i][0]/eigenVec[i][0];

		eigenVec = transpose(eigenVec);
		eigenVec.Data.push_back(Vector(1));
		eigenVec[1][0] = eigen;
		return eigenVec;
	}
}

std::vector<Vector> eigenValue(Matrix &l) {
	if (l.cols != l.rows) CON_ERR
	else if (l.rows > 3) DIM_ERR
	else {
		std::vector<double> eigens;
		if (l.cols == 2) {
			double b = -(l[0][0] + l[1][1]); 
			double c = determ(l); 
			double cond = pow(b, 2) - 4 * c; 

			//get eigen value
			if (cond < 0) IMG_EIGEN
			else if(cond == 0) {
				eigens.push_back((-b / 2));
			}
			else {
				eigens.push_back((-b + pow(cond, 0.5)) / 2);
				eigens.push_back((-b - pow(cond, 0.5)) / 2);
			}
		}
		else { //3x3 matrix
			double a, b, c, j,Q,R,theta;
			j = l[0][0] + l[1][1];
			a = -(j + l[2][2]);
			b = -(l[2][0] * l[0][2] + l[2][1] * l[1][2] + l[1][0] * l[0][1] - l[0][0] * l[1][1] - j * l[2][2]);
			c = -determ(l);
			Q = (pow(a, 2) - 3 * b) / 9;
			R = (2 * pow(a, 3) - 9 * a*b + 27 * c) / 54;
			if (Q < 0)CON_ERR
			else if (Q == 0) {
				eigens.push_back(-a / 3);
			}
			else {
				theta = acos(R / pow(pow(Q, 3), 0.5));
				double _cos = cos(theta / 3);
				double _sin = pow( 1 - _cos * _cos,0.5);
				double _sin60 = pow(3, 0.5) / 2;

				double _Q = pow(Q, 0.5);
				eigens.push_back(-2 *_Q*cos(theta / 3) - a / 3);
				eigens.push_back(-2 * _Q*( _cos* -0.5 - _sin60 *_sin ) - a / 3);
				eigens.push_back(-2 * _Q*( _cos* -0.5 + _sin60 * _sin )- a / 3);
			}
		}

		//removing deviations
	/*	for (int i = 0; i < eigens.size(); ++i) {
			if (ceil(eigens[i]) - eigens[i] < ZERO) eigens[i] = ceil(eigens[i]);
			else  if (eigens[i] - floor(eigens[i]) < ZERO) eigens[i] = floor(eigens[i]);
		}*/
		Vector eigenVec;
		eigenVec.Data = eigens;
		std::vector<Vector> results;
		results.push_back(eigenVec);
		for (int i = 0; i < eigens.size(); ++i) {
			Matrix eigenMatrix = l;
			for (int j = 0; j < eigenMatrix.cols; ++j)
				eigenMatrix[j][j] -= eigens[i];
			results.push_back(getEigenVector(eigenMatrix));
			//results.push_back(solve(eigenMatrix,zeros)[0]);	
			//std::string tmp = "Eigen: " + std::to_string(eigens[i]);
			//results[i].push_back(tmp);
			/*std::cout << eigens[i] << "\n";
			for (int j = 0, k = results[i].size(); j < k; ++j) {
				std::cout << results[i][j] << "\n";
			}*/
		}
		return results;
	}
}

Vector getEigenVector(Matrix &l) {
	Matrix r(l.rows, 1);
	int COLS = l.cols;
	int ROWS = l.rows;
	int ACOLS = r.cols;
	int AROWS = r.rows;
	if (ROWS != AROWS)CON_ERR

	//Sort
	std::vector<IVV> sorted;
	for (int i = 0; i < ROWS; ++i) {
		bool insert = false;
		IVV entity; entity.second.first = l[i]; entity.second.second = r[i];
		for (int j = 0; j < COLS; ++j) {
			if (fabs(l[i][j]) > ZERO) {
				entity.first = j;
				for (int k = 0, lk = sorted.size(); k < lk; ++k) {
					if (j < sorted[k].first) {
						sorted.insert(sorted.begin() + k, std::move(entity));
						insert = true;
						break;
					}
				}
				if (!insert) {
					sorted.push_back(std::move(entity));
					insert = true;
				}
				break;
			}
			else l[i][j] = 0;
		}
		if (!insert) {
			entity.first = COLS;
			sorted.push_back(std::move(entity));
		}
	}

	//Elimination
	int limit = ROWS < COLS ? ROWS : COLS;
	for (int r = 0; r < limit; ++r) {

		//Skipping leading zeros
		Vector & rowVec = sorted[r].second.first;
		Vector & ansVec = sorted[r].second.second;
		int c = sorted[r].first;
		if (c == COLS)break;

		//Eliminate
		for (int i_r = r + 1; i_r < ROWS; ++i_r) {
			Vector & _rowVec = sorted[i_r].second.first;
			Vector & _ansVec = sorted[i_r].second.second;

			double multiplier = _rowVec[c] / rowVec[c];
			if (multiplier == 0) {
				//Afterward vectors  has larger equal leading zeros.
				break;
			}
			bool leadZero = true;
			for (int i_c = c; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0.0;
					if (leadZero) sorted[i_r].first += 1;
				}
				else leadZero = false;
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0.0;
				}
			}
		}

		//Normalize
		double divident = rowVec[c];
		for (int i_c = c; i_c < COLS; ++i_c) {
			rowVec[i_c] /= divident;
		}
		for (int i_c = 0; i_c < ACOLS; ++i_c) {
			ansVec[i_c] /= divident;
		}


		//Sort
		if (r + 1 != ROWS)	std::qsort(&sorted[r + 1], ROWS - 1 - r, sizeof(IVV), comp_1);
	}

	//CleanUp
	for (int i_r = ROWS - 1; i_r >= 0; --i_r) {
		if (sorted[i_r].first == COLS) continue;

		Vector & rowVec = sorted[i_r].second.first;
		Vector & ansVec = sorted[i_r].second.second;
		int leading0 = sorted[i_r].first;

		for (int i_rr = i_r - 1; i_rr >= 0; --i_rr) {
			Vector & _rowVec = sorted[i_rr].second.first;
			Vector & _ansVec = sorted[i_rr].second.second;
			double multiplier = _rowVec[leading0];
			//Deviation Removal & Multiplier Zero
			if (fabs(multiplier) < ZERO) {
				_rowVec[leading0] = 0;
				continue;
			}
			for (int i_c = leading0; i_c < COLS; ++i_c) {
				_rowVec[i_c] -= multiplier * rowVec[i_c];
				//Deviation Removal
				if (fabs(_rowVec[i_c]) < ZERO) {
					_rowVec[i_c] = 0;
				}
			}
			for (int i_c = 0; i_c < ACOLS; ++i_c) {
				_ansVec[i_c] -= multiplier * ansVec[i_c];
				//Deviation Removal
				if (fabs(_ansVec[i_c]) < ZERO) {
					_ansVec[i_c] = 0;
				}
			}
		}
	}

	//Create Result
	Vector result(l.cols);
	for (int c = 0; c < ACOLS; ++c) {
		std::vector<std::string> ans;
		for (int r = AROWS -1; r  >=0 ; --r) {
			std::string formula;
			Vector & reqVec = sorted[r].second.first;
			if (sorted[r].first == COLS)result[r] = 1;
			else {
				for (int ac = sorted[r].first+1; ac < COLS; ++ ac) {
					if (reqVec[ac] != 0) {
						result[r] -= (reqVec[ac] * result[ac]);
					}
				}
			}
		}
	}
	return nrmlz(result);
}