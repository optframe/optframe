// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

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

class MatrixBadIndex
{
private:
   string message;
   unsigned row, col;
   unsigned maxRow, maxCol;

public:
   MatrixBadIndex(string m, unsigned _row, unsigned _col, unsigned _maxRow, unsigned _maxCol) :
      message(m), row(_row), col(_col), maxRow(_maxRow), maxCol(_maxCol)
   {
   }

   unsigned getMaxRow() const
   {
	   return maxRow;
   }

   unsigned getMaxCol() const
   {
	   return maxCol;
   }

   unsigned getRow() const
   {
	   return row;
   }

   unsigned getCol() const
   {
	   return col;
   }
};

template<class T>
class Matrix
{
public:

   Matrix(unsigned _quadratic = 1)
   {
      rows = cols = _quadratic;

      if (rows == 0 || cols == 0)
         throw MatrixBadIndex("Matrix constructor has 0 size", 0, 0, 0, 0);

      data = new T[rows * cols];
   }

   Matrix(unsigned _rows, unsigned _cols)
   {
      rows = _rows;
      cols = _cols;

      if (rows == 0 || cols == 0)
         throw MatrixBadIndex("Matrix constructor has 0 size", 0, 0, 0, 0);

      data = new T[rows * cols];
   }

   T& operator()(unsigned row, unsigned col)
   {
      if (row >= rows || col >= cols)
         throw MatrixBadIndex("Matrix Out of Bounds Exception", row, col, rows, cols);

      return data[cols * row + col];
   }

   T operator()(unsigned row, unsigned col) const
   {
      if (row >= rows || col >= cols)
         throw MatrixBadIndex("Matrix Out of Bounds Exception", row, col, rows, cols);

      return data[cols * row + col];
   }

   virtual ~Matrix() //Destructor
   {
      delete[] data;
   }

   // Retorna true se a matriz for quadrada
   bool square() const
   {
      return (rows == cols);
   }

   // Metodos util! fill() preenche a matriz com o valor desejado
   void fill(T v)
   {
      unsigned int total = rows * cols;

      for (unsigned int i = 0; i < (total); i++)
         data[i] = v;
   }

   Matrix(const Matrix& m) // Copy constructor
   {
      rows = m.rows;
      cols = m.cols;

      unsigned int total = rows * cols;
      data = new T[total];

      for (unsigned int i = 0; i < (total); i++)
         data[i] = m.data[i];
   }

   Matrix& operator=(const Matrix& m) // Assignment operator
   {
      //Verificando auto-referencia (Importante!)
      if (&m == this)
         return *this;

      delete[] data;

      rows = m.rows;
      cols = m.cols;

      int total = rows * cols;

      data = new T[total];

      for (int i = 0; i < (total); i++)
         data[i] = m.data[i];

      return *this;
   }

   unsigned getNumRows() const
   {
      return rows;
   }

   unsigned getNumCols() const
   {
      return cols;
   }

   vector<T> getRow(int _row) const
   {
      vector<T> row(cols);

      for (int i = 0; i < cols; i++)
      {
         row[i] = operator()(_row, i);
      }

      return row;
   }

   void setRow(int p, vector<T>& _row)
   {
      int numCol = (_row.size() < cols) ? _row.size() : cols;

      for (int i = 0; i < numCol; i++)
      {
         operator()(p, i) = _row[i];
      }
   }

   vector<T> getCol(int _col) const
   {
      vector<T> col(rows);

      for (int i = 0; i < rows; i++)
      {
         col[i] = operator()(i, _col);
      }

      return col;
   }

   void setCol(int p, vector<T>& _col)
   {
      int numRow = (_col.size() < rows) ? _col.size() : rows;

      for (int i = 0; i < numRow; i++)
      {
         operator()(i, p) = _col[i];
      }
   }

   friend ostream& operator<<(ostream &os, const Matrix<T> &obj)
   {
      //const Matrix<T> &obj = *this;
      //os << "Matrix(" << obj.getRows() << "," << obj.getCols() << ")" << endl;

      os << endl;

      for (unsigned int i = 0; i < obj.getNumRows(); i++)
      {
         for (unsigned int j = 0; j < obj.getNumCols(); j++)
            os << obj(i, j) << "\t";
         os << endl;
      }
      return os;
   }

private:
   unsigned rows, cols;
   T* data;
};

/*
template<class T>
ostream& operator<<(ostream &os, const Matrix<T> &obj)
{
   //os << "Matrix(" << obj.getRows() << "," << obj.getCols() << ")" << endl;

   os << endl;

   for (unsigned int i = 0; i < obj.getNumRows(); i++)
   {
      for (unsigned int j = 0; j < obj.getNumCols(); j++)
         os << obj(i, j) << " ";
      os << endl;
   }
   return os;
}

ostream& operator<<(ostream &os, const Matrix<string> &obj)
{
   os << "Matrix(" << obj.getNumRows() << "," << obj.getNumCols() << ")" << endl;

   for (unsigned int i = 0; i < obj.getNumRows(); i++)
   {
      for (unsigned int j = 0; j < obj.getNumCols(); j++)
         os << "\"" << obj(i, j) << "\"" << "\t";
      os << endl;
   }
   return os;
}
*/

#endif /*MATRIX_H_*/
