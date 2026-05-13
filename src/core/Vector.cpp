#include "Vector.h"

Vector::Vector(Vector3D r, Vector3D v, double m)
    : r(r), v(v), m(m) {};

Vector Vector::operator*(double num) const {
	Vector result = *this;
	
	result.r = result.r * num;
	result.v = result.v * num;
	result.m = result.m * num;

	return result;
}

Vector Vector::operator/(double num) const {

	Vector result = *this;
	result.r = result.r / num;
	result.v = result.v / num;
	result.m = result.m / num;
	return result;
}

Vector Vector::operator+(const Vector& other) const {
	
	Vector result = *this;

	result.r = result.r + other.r;
	result.v = result.v + other.v;
	result.m = result.m + other.m;
	
	return result;
}

Vector Vector::operator-(const Vector& other) const {

	Vector result = *this + (other * (-1));

	return result;
}