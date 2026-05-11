#ifndef ARTEMISII_VECTOR_H
#define ARTEMISII_VECTOR_H

#include "core/Vector3d.h"
/**
 * Класс, описывающий вектор параметров, передающихся в программу численного
 * интегрирования.
 */


struct Vector {
	Vector3D r;
	Vector3D v;
	double m;

	Vector(std::vector<double> data);
	Vector(Vector3D r, Vector3D v, double m);
	Vector operator*(double num) const;
	Vector operator/(double num) const;
	Vector operator+(const Vector& other) const;
	Vector operator-(const Vector& other) const;
};




#endif //ARTEMISII_VECTOR_H