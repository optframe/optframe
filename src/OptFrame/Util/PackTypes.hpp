// PackTypes class
// Author: Igor M. Coelho
// Date:   May 2016
// Idea:   To store different types in a single variable (union) in order to
//         reduce storage and also be able to perform numeric operations.
// Applic: It can be applied to very special MO problems with specific needs
//         for each objective space.

#ifndef __PACKTYPES__
#define __PACKTYPES__

#include <iostream>

using namespace std;

struct PackTypes
{
	union { int i; double d; long long ll; } value;
	int type;

	// ===============

	// default constructor (double)
	PackTypes() {
		type = 1;
		value.d = 0;
	}

	template<class T>
	PackTypes(const T& val) {
		type = 1;
		value.d = val;
	}

	// PackTypes constructor
	PackTypes(int _type, const PackTypes& val) : type(_type) {
		switch (type) {
			case 0:  value.i = val.value.i; break;
			case 1:  value.d = val.value.d; break;
			default: value.ll = val.value.ll;
		}
	}

	// generic constructor
	template<class T> PackTypes(int _type, T val) : type(_type) {
		switch (type) {
			case 0:  value.i = val; break;
			case 1:  value.d = val; break;
			default: value.ll = val;
		}
	}

	// ===============

	// conversion operator (for typecasts like: int(PackTypes), double(PackTypes), ...)
	template<class T> operator T(void) const {
		switch (type) {
			case 0:  return value.i;
			case 1:  return value.d;
			default: return value.ll;
		}
	}

	// ===============

	// assignment operator
	template<class T>
	PackTypes& operator=(T val) {
		switch (type) {
			case 0:  value.i = val;  return *this;
			case 1:  value.d = val;  return *this;
			default: value.ll = val; return *this;
		}
	}

	// ===============

	// += generic
	template<class T>
	PackTypes& operator+=(const T rhs) {
		switch (type) {
			case 0:  value.i  += rhs; return *this;
			case 1:  value.d  += rhs; return *this;
			default: value.ll += rhs; return *this;
		}
	}

	// += PackTypes
	PackTypes& operator+=(const PackTypes& rhs) {
		switch (type) {
			case 0:  value.i  += rhs.value.i;  return *this;
			case 1:  value.d  += rhs.value.d;  return *this;
			default: value.ll += rhs.value.ll; return *this;
		}
	}

	// ===============

	template<class T>
	const PackTypes operator+(T lhs) const {
	    return PackTypes(*this) += lhs;
	}

	template<class T> friend PackTypes operator+(T lhs, const PackTypes& t) {
		return PackTypes(t) += lhs;
	}

	// ===============

	// -= generic
	template<class T>
	PackTypes& operator-=(const T rhs) {
		switch (type) {
			case 0:  value.i  -= rhs; return *this;
			case 1:  value.d  -= rhs; return *this;
			default: value.ll -= rhs; return *this;
		}
	}

	// -= PackTypes
	PackTypes& operator-=(const PackTypes& rhs) {
		switch (type) {
			case 0:  value.i  -= rhs.value.i;  return *this;
			case 1:  value.d  -= rhs.value.d;  return *this;
			default: value.ll -= rhs.value.ll; return *this;
		}
	}

	// ===============

	template<class T>
	const PackTypes operator-(T lhs) const {
	    return PackTypes(*this) -= lhs;
	}

	template<class T> friend PackTypes operator-(T lhs, const PackTypes& t) {
		//return PackTypes(t) -= lhs; // NOT DOING THIS... OPPOSITE DIRECTION!
		switch (t.type) {
			case 0:  return PackTypes(t.type, lhs - t.value.i);
			case 1:  return PackTypes(t.type, lhs - t.value.d);
			default: return PackTypes(t.type, lhs - t.value.ll);
		}
	}

	// unary
	PackTypes operator-() {
	    return 0 - PackTypes(*this);
	}

	// ===============

	// *= generic
	template<class T>
	PackTypes& operator*=(const T rhs) {
		switch (type) {
			case 0:  value.i  *= rhs; return *this;
			case 1:  value.d  *= rhs; return *this;
			default: value.ll *= rhs; return *this;
		}
	}

	// -= PackTypes
	PackTypes& operator*=(const PackTypes& rhs) {
		switch (type) {
			case 0:  value.i  *= rhs.value.i;  return *this;
			case 1:  value.d  *= rhs.value.d;  return *this;
			default: value.ll *= rhs.value.ll; return *this;
		}
	}

	// ===============

	template<class T>
	const PackTypes operator*(T lhs) const {
	    return PackTypes(*this) *= lhs;
	}

	template<class T> friend PackTypes operator*(T lhs, const PackTypes& t) {
		return PackTypes(t) *= lhs;
	}

	// ===============

	// /= generic
	template<class T>
	PackTypes& operator/=(const T rhs) {
		switch (type) {
			case 0:  value.i  /= rhs; return *this;
			case 1:  value.d  /= rhs; return *this;
			default: value.ll /= rhs; return *this;
		}
	}

	// /= PackTypes
	PackTypes& operator/=(const PackTypes& rhs) {
		switch (type) {
			case 0:  value.i  /= rhs.value.i;  return *this;
			case 1:  value.d  /= rhs.value.d;  return *this;
			default: value.ll /= rhs.value.ll; return *this;
		}
	}

	// ===============

	template<class T>
	const PackTypes operator/(T lhs) const {
	    return PackTypes(*this) /= lhs;
	}

	template<class T> friend PackTypes operator/(T lhs, const PackTypes& t) {
		return PackTypes(t) /= lhs;
	}

	// ===============

	// PackTypes operator=
	bool operator==(const PackTypes& rhs) const {
		if(type != rhs.type)
			return false;
		switch (type) {
			case 0:  return value.i  == rhs.value.i;
			case 1:  return value.d  == rhs.value.d;
			default: return value.ll == rhs.value.ll;
		}
	}

	// numeric operator=
	template<class T>
	bool operator==(const T& rhs) const {
		switch (type) {
			case 0:  return value.i  == rhs;
			case 1:  return value.d  == rhs;
			default: return value.ll == rhs;
		}
	}

	template<class T>
	bool operator!=(const T& rhs) const { return !(*this == rhs); }

	// ===============

	// PackTypes operator<
	bool operator<(const PackTypes& rhs) const {
		if(type != rhs.type)
			return false;
		switch (type) {
			case 0:  return value.i  < rhs.value.i;
			case 1:  return value.d  < rhs.value.d;
			default: return value.ll < rhs.value.ll;
		}
	}

	// numeric operator<
	template<class T>
	bool operator<(const T& rhs) const {
		switch (type) {
			case 0:  return value.i  < rhs;
			case 1:  return value.d  < rhs;
			default: return value.ll < rhs;
		}
	}

	template<class T>
	bool operator> (const T& rhs) const { return rhs < *this; }

	template<class T>
	bool operator<=(const T& rhs) const { return !(*this > rhs); }

	template<class T>
	bool operator>=(const T& rhs) const { return !(*this < rhs); }

	// ===============
	// ========================
	// ========================
	// ========================

	/*
	template<class T> friend PackTypes operator+(T lhs, const PackTypes& t) {
		  		switch (t.type) {
		  			case 0:  return PackTypes(t.type, t.value.i + lhs);
		  			case 1:  return PackTypes(t.type, t.value.d + lhs);
		  			default: return PackTypes(t.type, t.value.ll + lhs);
		  		}
		  	}
*/

/*
	  const MyClass MyClass::operator+(const MyClass &other) const {
	    MyClass result = *this;     // Make a copy of myself.  Same as MyClass result(*this);
	    result += other;            // Use += to add other to the copy.
	    return result;              // All done!
	  }

	  template<class T> PackTypes operator+(T val) {
	  		switch (type) {
	  			case 0:  value.i  += val; return *this;
	  			case 1:  value.d  += val; return *this;
	  			default: value.ll += val; return *this;
	  		}
	  	}

	  	template<class T> friend PackTypes operator+(T lhs, const PackTypes& t) {
	  		switch (t.type) {
	  			case 0:  return PackTypes(t.type, t.value.i + lhs);
	  			case 1:  return PackTypes(t.type, t.value.d + lhs);
	  			default: return PackTypes(t.type, t.value.ll + lhs);
	  		}
	  	}
*/

	friend ostream& operator<<(ostream& os, const PackTypes& mt) {
		os << "(type:" << mt.type << ")";
		switch (mt.type) {
			case 0: os << mt.value.i;   return os;
			case 1: os << mt.value.d;   return os;
			default: os << mt.value.ll; return os;
		}
	}


	static void unit_test()
	{
		PackTypes t(0, 25);

		t += 50;
		cout << t << endl;
		t = ((int)INT_MAX);
		t = t + 1;
		cout << t << endl;
		cout << unsigned(t) << endl;
		t = 3.15;
		cout << t << endl;
		t.type = 1;
		t = 3.15;
		cout << t << endl;
		cout << int(t) << endl;

		t += 50;
		cout << t << endl;

		t.type = 2;
		t = ((long long)INT_MAX)*20;
		cout << t << endl;

		cout << double(t) << endl;
		t = double(t);
		cout << t << endl;
		t = int(t);
		cout << t << endl;

		t.type = 0;
		cout << t << endl;
		t = t - 10;
		cout << t << endl;

		t.type = 1;
		t = 74.5;
		t = t - 4.5;
		cout << t << endl;
		t = -t;
		cout << t << endl;
		t = 10.5 - t;
		cout << t << endl;

		t *= -1;
		cout << t << endl;

		t *= 20;
		cout << t << endl;

		t /= 10;
		cout << t << endl;

		t = 6.0/7.0;
		cout << t << endl;

		if(t == 10)
			cout << "equals to 10" << endl;
		else
			cout << "different from 10" << endl;

		if(t < 10)
			cout << "less than 10" << endl;
		else
			cout << "greater than 10" << endl;
	}
};


#endif
