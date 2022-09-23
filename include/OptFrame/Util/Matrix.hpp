// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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

using namespace std; // TODO: remove!

namespace optframe {

/*
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
*/

template<class T>
class Matrix
{
public:
   Matrix(unsigned _quadratic = 1) noexcept
   {
      if (!_quadratic)
         _quadratic = 1;
      rows = cols = _quadratic;

      //if (rows == 0 || cols == 0)
      //   throw MatrixBadIndex("Matrix constructor has 0 size", 0, 0, 0, 0);

      data = new T[rows * cols];
   }

   Matrix(unsigned _rows, unsigned _cols) noexcept
   {
      // rows = _rows ?: 1;
      rows = _rows;
      if (!rows)
         rows = 1;
      //cols = _cols ?: 1;
      cols = _cols;
      if (!cols)
         cols = 1;

      //if (rows == 0 || cols == 0)
      //   throw MatrixBadIndex("Matrix constructor has 0 size", 0, 0, 0, 0);

      data = new T[rows * cols];
   }

   T& operator()(unsigned row, unsigned col) noexcept
   {
      row = row >= rows ? 0 : row;
      col = col >= cols ? 0 : col;
      //if (row >= rows || col >= cols)
      //   throw MatrixBadIndex("Matrix Out of Bounds Exception", row, col, rows, cols);

      return data[cols * row + col];
   }

   T operator()(unsigned row, unsigned col) const noexcept
   {
      row = row >= rows ? 0 : row;
      col = col >= cols ? 0 : col;
      //if (row >= rows || col >= cols)
      //   throw MatrixBadIndex("Matrix Out of Bounds Exception", row, col, rows, cols);

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

      for (int i = 0; i < cols; i++) {
         row[i] = operator()(_row, i);
      }

      return row;
   }

   void setRow(int p, vector<T>& _row)
   {
      int numCol = (_row.size() < cols) ? _row.size() : cols;

      for (int i = 0; i < numCol; i++) {
         operator()(p, i) = _row[i];
      }
   }

   vector<T> getCol(int _col) const
   {
      vector<T> col(rows);

      for (int i = 0; i < rows; i++) {
         col[i] = operator()(i, _col);
      }

      return col;
   }

   void setCol(int p, vector<T>& _col)
   {
      int numRow = (_col.size() < rows) ? _col.size() : rows;

      for (int i = 0; i < numRow; i++) {
         operator()(i, p) = _col[i];
      }
   }

   friend ostream& operator<<(ostream& os, const Matrix<T>& obj)
   {
      //const Matrix<T> &obj = *this;
      //os << "Matrix(" << obj.getRows() << "," << obj.getCols() << ")" << endl;

      os << endl;

      for (unsigned int i = 0; i < obj.getNumRows(); i++) {
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

} // namespace optframe

#endif /*MATRIX_H_*/
