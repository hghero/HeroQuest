#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <exception>
#include <QtCore/QString>

// ===============================================

template <class T>
T round(T x)
{
	return floor(x + 0.5);
};

// ===============================================

template <class T>
class Vec2
{
public:
	T x;
	T y;

	Vec2()
	:
	x(0),
	y(0)
	{
		// NIX
	};

	Vec2(T new_x, T new_y)
	:
	x(new_x),
	y(new_y)
	{
		// NIX
	};

	Vec2(const Vec2<int>& other)
	:
	x(other.x),
	y(other.y)
	{
		// NIX
	};

	~Vec2()
	{
		// NIX
	};

	QString toString() const
	{
		std::stringstream str_stream;
		str_stream << "(" << x << ", " << y << ")";
		return QString(str_stream.str().c_str());
	};

	Vec2<T> operator+(const Vec2<T>& other) const
	{
		Vec2<T> result(*this);
		result += other;
		return result;
	};

	Vec2<T>& operator+=(const Vec2<T>& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	};

	Vec2<T> operator-(const Vec2<T>& other) const
	{
		Vec2<T> result(*this);
		result -= other;
		return result;
	};

	Vec2<T>& operator-=(const Vec2<T>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	};

	bool operator==(const Vec2<T>& other) const
	{
		return x == other.x && y == other.y;
	};

	bool operator!=(const Vec2<T>& other) const
	{
		return !(*this == other);
	};

	/*!
	 * Scalar multiplication.
	 */
	template <class S>
	Vec2<T>& operator*=(const S& scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	};

	template <class S>
	Vec2<T> operator*(const S& scalar) const
	{
		Vec2<T> result(*this);
		result *= scalar;
		return result;
	};

	/*!
	 * Scalar division.
	 */
	template <class S>
	Vec2<T>& operator/=(const S& scalar)
	{
		x = T(x / double(scalar));
		y = T(y / double(scalar));
		return *this;
	};

	template <class S>
	Vec2<T> operator/(const S& scalar) const
	{
		Vec2<T> result(*this);
		result /= scalar;
		return result;
	};

	double length2() const
	{
		return this->x * this->x + this->y * this->y;
	};

	double length() const
	{
		return sqrt(length2());
	};
};

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

std::ostream& operator<<(std::ostream& stream, const Vec2i& vec);
std::ostream& operator<<(std::ostream& stream, const Vec2f& vec);
std::ostream& operator<<(std::ostream& stream, const Vec2d& vec);

// ===============================================

template <class T>
class StorageMatrix
{
public:
	StorageMatrix()
	{
		resize(1, 1);
	};

	StorageMatrix(int size_x, int size_y)
	{
		resize(size_x, size_y);
	};

	StorageMatrix(int size_x, int size_y, const T& val)
	{
		resize(size_x, size_y);

		for (int y = 0; y < size_y; ++y)
			for (int x = 0; x < size_x; ++x)
				_rows[y][x] = val;
	};

	~StorageMatrix()
	{
		; // nix
	};

	void resize(int size_x, int size_y)
	{
		_rows.resize(size_y);
		for (int i = 0; i < size_y; ++i)
			_rows[i].resize(size_x);
	};

	std::vector<T>& operator[](int index)
	{
		return _rows[index];
	};

	const std::vector<T>& operator[](int index) const
	{
		return _rows[index];
	};

	int getWidth() const
	{
		if (_rows.size() == 0)
			return 0;
			
		return int(_rows[0].size());
	};

	int getHeight() const
	{
		return int(_rows.size());
	};

private:
	std::vector<std::vector<T> > _rows;
};

// ===============================================

class Line
{
public:
	static const double EPSILON_DETERMINANT;

	Line(const Vec2i& p1, const Vec2i& p2)
	:
	_p1(p1),
	_p2(p2)
	{
		if (_p1 == _p2)
		{
			std::cout << "Warning: line not well-defined! p1 = " << p1 << "; p2 = " << p2 << std::endl;
			throw new std::exception("line not well-defined");
		}
	};

	const Vec2i& getP1() const
	{
		return _p1;
	};

	const Vec2i& getP2() const
	{
		return _p2;
	};

	/*!
	 * \return True, if p is contained in the rectangle defined by the line, false otherwise.
	 */
	template <class T>
	bool contains(const Vec2<T>& p) const
	{
		T p_small_x = (_p1.x < _p2.x) ? T(_p1.x) : T(_p2.x);
		T p_great_x = (_p1.x > _p2.x) ? T(_p1.x) : T(_p2.x);

		T p_small_y = (_p1.y < _p2.y) ? T(_p1.y) : T(_p2.y);
		T p_great_y = (_p1.y > _p2.y) ? T(_p1.y) : T(_p2.y);

		return p.x >= p_small_x && p.x <= p_great_x &&
			   p.y >= p_small_y && p.y <= p_great_y;
	};

private:
	Vec2i _p1, _p2;
};

// ===============================================

uint randomNumber(uint limit);

template <class ElementType>
typename ElementType getRandomElementByValue(std::vector<typename ElementType>& container)
{
	if (container.size() == 0)
		cout << "Internal program error: container is empty!" << endl;

	uint r = randomNumber(container.size());

	return container[r];
}

// ===============================================

bool intersect(const Line& l1, const Line& l2, Vec2d* s);

// ===============================================

#endif
