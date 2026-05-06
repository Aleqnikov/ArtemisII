#ifndef ARTEMISII_VECTOR_H
#define ARTEMISII_VECTOR_H


/**
 * Класс, описывающий вектор параметров, передающихся в программу численного
 * интегрирования.
 */

struct Vector
{
	double x, y, z;
	double vx, vy, vz;

	double m;

	Vector(double x, double y, double z,double vx, double vy, double vz, double m) :
	x(x), y(y), z(z), vx(vx), vy(vy), vz(vz), m(m) {};
}



#endif //ARTEMISII_VECTOR_H