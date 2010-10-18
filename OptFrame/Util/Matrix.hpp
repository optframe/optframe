// TODO
// Implement ITERATORS - Usage: to sort matrix based on a line or a column.
// Implement sortMatrix method based on 'sort' of STL.

// Ver este link
//http://www.parashift.com/c++-faq-lite/operator-overloading.html

// boas dicas em 13.9

// Analisar preferencia de m(1,2) do que m[1][2]

// http://www.parashift.com/c++-faq-lite/operator-overloading.html#faq-13.11

// (...)
// Put it this way: the operator() approach is never worse than, and sometimes
// better than, the [][] approach. 


#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

class BadIndex
{
private:
	string message;
public:
	BadIndex(string m)
	{
		message = m;
	};	
};


template<class T>
class Matrix 
{
public:
	Matrix(unsigned _quadratic)
	{
		rows = cols = _quadratic;

		if (rows == 0 || cols == 0)
			throw BadIndex("Matrix constructor has 0 size");

		data = new T[rows * cols];
	};

	Matrix(unsigned _rows, unsigned _cols)
	{
		rows = _rows;
		cols = _cols;

		if (rows == 0 || cols == 0)
			throw BadIndex("Matrix constructor has 0 size");

		data = new T[rows * cols];
	};

	T& operator() (unsigned row, unsigned col)  // subscript operators often come in pairs
	{
		if (row >= rows || col >= cols)
		{
			cout << "[set Matrix] Pedido de ("<<row<<","<<col<<")" << " de ("<<rows<<","<<cols<<")" << endl;

			int jkl;
			cin >> jkl;

			throw BadIndex("Matrix subscript out of bounds");
		}

		return data[cols*row + col];
	};

	T  operator() (unsigned row, unsigned col) const  // subscript operators often come in pairs
	{
		if (row >= rows || col >= cols)
		{
			cout << "[get Matrix] Pedido de ("<<row<<","<<col<<")" << " de ("<<rows<<","<<cols<<")" << endl;

			int jkl;
			cin >> jkl;

			throw BadIndex("const Matrix subscript out of bounds");
		}

		return data[cols*row + col];
	};

	virtual ~Matrix()  //Destructor
	{
		delete[] data;
	};

	// Metodos util! fill() preenche a matriz com o valor desejado
	void fill(T v)
	{
		unsigned int total = rows * cols;

		for(unsigned int i = 0; i < (total) ; i++)
			data[i] = v;
	};

	Matrix(const Matrix& m)  // Copy constructor
	{
		rows = m.rows;
		cols = m.cols;

		unsigned int total = rows * cols;
		data = new T[total];

		for(unsigned int i = 0; i < (total) ; i++)
			data[i] = m.data[i];
	};

	Matrix& operator= (const Matrix& m) // Assignment operator
	{
		//Verificando auto-referencia (Importante!)
		if(&m == this)
			return *this;

		delete [] data;

		rows = m.rows;
		cols = m.cols;

		int total = rows * cols;

		if(total < 0)
		{
			cerr << "Valor maior do que o suportado pela Matrix! (" << total << ")" << endl;
			exit(1);
		}

		data = new T[total];

		for(int i = 0; i < (total); i++)
			data[i] = m.data[i];

		return *this;
	};

	unsigned getRows() const
	{
		return rows;
	}

	unsigned getCols() const
	{
		return cols;
	}

	vector<T> getRow(int _row) const
	{
	   vector<T> row(cols);
	   
	   for(int i=0;i<cols;i++)
	   {
	      row[i] = operator()(_row,i);
	   }
	   
		return row;
	}

	void setRow(int p, vector<T>& _row)
	{
      int numCol = (_row.size() < cols)?_row.size():cols;
	   
	   for(int i=0;i<numCol;i++)
	   {
	      operator()(p,i) = _row[i];
	   }
	}

   vector<T> getCol(int _col) const
	{
	   vector<T> col(rows);
	   
	   for(int i=0;i<rows;i++)
	   {
	      col[i] = operator()(i,_col);
	   }
	   
		return col;
	}

	void setCol(int p, vector<T>& _col)
	{
	   int numRow = (_col.size() < rows)?_col.size():rows;	   
	   
	   for(int i=0;i<numRow;i++)
	   {
	      operator()(i,p) = _col[i];
	   }
	}


private:
	unsigned rows, cols;
	T* data;
};



template<class T>
ostream& operator<< (ostream &os, const Matrix<T> &obj)
{
	//os << "Matrix(" << obj.getRows() << "," << obj.getCols() << ")" << endl;
	
	os << endl;

	for(unsigned int i = 0; i < obj.getRows(); i++)
	{
		for(unsigned int j=0; j < obj.getCols(); j++)
			os << obj(i,j) << " ";
		os << endl;
	}
	return os;
}

ostream& operator<< (ostream &os, const Matrix<string> &obj)
{
	os << "Matrix(" << obj.getRows() << "," << obj.getCols() << ")" << endl;
	
	for(unsigned int i = 0; i < obj.getRows(); i++)
	{
		for(unsigned int j=0; j < obj.getCols(); j++)
			os << "\"" << obj(i,j) << "\"" << "\t";
		os << endl;
	}
	return os;
}

#endif /*MATRIX_H_*/
