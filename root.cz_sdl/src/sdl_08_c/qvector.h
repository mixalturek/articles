/***************************************************************************
 *   libQuark - Archive of useful C++ classes                              *
 *   Copyright (C) 2004 by Michal Turek - Woq                              *
 *   WOQ (at) seznam.cz, http://woq.nipax.cz/                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef LIBQUARK_QVECTOR_H
#define LIBQUARK_QVECTOR_H

#include <iostream>
#include "libquark.h"

// Only private use
#define X m_data[0]
#define Y m_data[1]
#define Z m_data[2]

using namespace std;

namespace libQuark
{

template <class TYPE>
class QVector
{
public:
	QVector();
	QVector(TYPE x, TYPE y, TYPE z);
	QVector(const QVector<TYPE>& v);

	// Vector from two points
	QVector(const QVector<TYPE>& start_point,
		const QVector<TYPE>& end_point);

	~QVector();


	void Set(TYPE x = 0, TYPE y = 0, TYPE z = 0);
	void SetX(TYPE x) { X = x; }
	void SetY(TYPE y) { Y = y; }
	void SetZ(TYPE z) { Z = z; }

	TYPE GetX() const { return X; }
	TYPE GetY() const { return Y; }
	TYPE GetZ() const { return Z; }


	// Return pointer to data (for glVertex3*v())
	TYPE* GetDataPtr() { return m_data; }
	operator TYPE*() { return m_data; }

	bool IsZero() const { return (X == 0 && Y == 0 && Z == 0); }


	const QVector<TYPE>& operator=(const QVector<TYPE>& v);

	void operator+=(const QVector<TYPE>& v);
	void operator-=(const QVector<TYPE>& v);

	QVector<TYPE> operator+(const QVector<TYPE>& v) const;
	QVector<TYPE> operator-(const QVector<TYPE>& v) const;

	// Inverse vector
	QVector<TYPE> operator-() const { X = -X; Y = -Y; Z = -Z; }


	// Vector scaling
	void operator*=(TYPE n);
	void operator/=(TYPE n);
	QVector<TYPE> operator*(TYPE n) const;
	QVector<TYPE> operator/(TYPE n) const;

	// Vectors can be MATHEMATICAL equal (use Normalize() before)
	bool operator==(const QVector<TYPE>& v) const;
	bool operator!=(const QVector<TYPE>& v) const;


	// Dot product
	TYPE operator*(const QVector<TYPE>& v) const;
	TYPE Dot(const QVector<TYPE>& v) const;


	// Cross product from two vectors
	QVector<TYPE> Cross(const QVector<TYPE>& v) const;

	// Cross product from three points
	QVector<TYPE> Cross(const QVector<TYPE>& p1,
			const QVector<TYPE>& p2) const;


	double Magnitude() const;
	void Normalize();

	double AngleRad(const QVector<TYPE>& v) const;
	double AngleDeg(const QVector<TYPE>& v) const;

	// Distance of two points
	double Distance(const QVector<TYPE>& v) const;

private:
	TYPE m_data[3];
};

///////////////////////////////////////////////////////////////////////////////

template <class TYPE>
QVector<TYPE>::QVector()
{
	X = (TYPE)0;
	Y = (TYPE)0;
	Z = (TYPE)0;
}

template <class TYPE>
QVector<TYPE>::QVector(const QVector<TYPE>& v)
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
}

template <class TYPE>
QVector<TYPE>::QVector(const QVector<TYPE>& start_point,
			const QVector<TYPE>& end_point)
{
	*this = end_point - start_point;
}

template <class TYPE>
QVector<TYPE>::QVector(TYPE x, TYPE y, TYPE z)
{
	X = x;
	Y = y;
	Z = z;
}

template <class TYPE>
QVector<TYPE>::~QVector()
{

}

template <class TYPE>
inline void QVector<TYPE>::Set(TYPE x, TYPE y, TYPE z)
{
	X = x;
	Y = y;
	Z = z;
}

template <class TYPE>
inline void QVector<TYPE>::operator+=(const QVector<TYPE>& v)
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;
}

template <class TYPE>
inline void QVector<TYPE>::operator-=(const QVector<TYPE>& v)
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;
}

template <class TYPE>
inline void QVector<TYPE>::operator*=(TYPE n)
{
	X *= n;
	Y *= n;
	Z *= n;
}

template <class TYPE>
inline void QVector<TYPE>::operator/=(TYPE n)
{
	if(n != 0)
	{
		X /= n;
		Y /= n;
		Z /= n;
	}
}

template <class TYPE>
inline const QVector<TYPE> & QVector<TYPE>::operator=(const QVector<TYPE>& v)
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	return *this;
}

template <class TYPE>
inline QVector<TYPE> QVector<TYPE>::operator+(const QVector<TYPE>& v) const
{
	return QVector(X + v.X, Y + v.Y, Z + v.Z);
}

template <class TYPE>
inline QVector<TYPE> QVector<TYPE>::operator-(const QVector<TYPE>& v) const
{
	return QVector(X - v.X, Y - v.Y, Z - v.Z);
}

template <class TYPE>
inline QVector<TYPE> QVector<TYPE>::operator*(TYPE n) const
{
	return QVector(X * n, Y * n, Z * n);
}

template <class TYPE>
inline QVector<TYPE> QVector<TYPE>::operator/(TYPE n) const
{
	if(n != 0)
	{
		return QVector(X / n, Y / n, Z / n);
	}

	return *this;
}

// Dot product
template <class TYPE>
inline TYPE QVector<TYPE>::operator*(const QVector<TYPE>& v) const
{
	return X*v.X + Y*v.Y + Z*v.Z;
}

template <class TYPE>
inline bool QVector<TYPE>::operator==(const QVector<TYPE>& v) const
{
	// Vectors can be MATHEMATICAL equal!!! (use Normalize() before)
	 return (X == v.X && Y == v.Y && Z == v.Z);
}

template <class TYPE>
inline bool QVector<TYPE>::operator!=(const QVector<TYPE>& v) const
{
	return !(*this == v);
}

template <class TYPE>
inline double QVector<TYPE>::Magnitude() const
{
	return sqrt(X*X + Y*Y + Z*Z);
}

template <class TYPE>
inline void QVector<TYPE>::Normalize()
{
	*this /= this->Magnitude();
}

template <class TYPE>
inline TYPE QVector<TYPE>::Dot(const QVector<TYPE>& v) const
{
	return X*v.X + Y*v.Y + Z*v.Z;
}

// Cross product from two vectors
template <class TYPE>
QVector<TYPE> QVector<TYPE>::Cross(const QVector<TYPE>& v) const
{
	QVector ret;

	ret.X = Y * v.Z - Z * v.Y;
	ret.Y = Z * v.X - X * v.Z;
	ret.Z = X * v.Y - Y * v.X;

	return ret;
}

// Cross product from three points
template <class TYPE>
QVector<TYPE> QVector<TYPE>::Cross(const QVector<TYPE>& p1,
				const QVector<TYPE>& p2) const
{
	QVector<TYPE> v1(*this, p1);
	QVector<TYPE> v2(*this, p2);

	return v1.Cross(v2);
}

template <class TYPE>
inline double QVector<TYPE>::AngleRad(const QVector<TYPE>& v) const
{
	TYPE magnitudes = Magnitude() * v.Magnitude();

	if(magnitudes == 0)
		return 0;

	return acos((*this * v) / magnitudes);
}

template <class TYPE>
inline double QVector<TYPE>::AngleDeg(const QVector<TYPE>& v) const
{
	return RADTODEG(AngleRad(v));
}

// Distance of two points
template <class TYPE>
inline double QVector<TYPE>::Distance(const QVector<TYPE>& v) const
{
	return sqrt((X-v.X)*(X-v.X) + (Y-v.Y)*(Y-v.Y) + (Z-v.Z)*(Z-v.Z));
}

///////////////////////////////////////////////////////////////////////////////

template <class TYPE>
istream& operator>>(istream& is, QVector<TYPE>& v)
{
	TYPE tmp;

	is >> tmp;
	v.SetX(tmp);
	is >> tmp;
	v.SetY(tmp);
	is >> tmp;
	v.SetZ(tmp);

	return is;
}

template <class TYPE>
ostream& operator<<(ostream& os, QVector<TYPE>& v)
{
	return (os << v.GetX() << " " << v.GetY() << " " << v.GetZ());
}

};

#endif
