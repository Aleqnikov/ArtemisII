#ifndef VECTOR3D_H
#define VECTOR3D_H


#include <array>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>


class Vector3D
{
public:
    std::array<double, 3> data;

    Vector3D(std::array<double, 3> pars) : data(pars) {};
    Vector3D(double x, double y, double z) : data({x, y, z}) {};
	Vector3D() : data({0.0, 0.0, 0.0}) {}

    Vector3D operator+(const Vector3D &other) const
    {
        Vector3D res(data);

        for (int i = 0; i  < 3 ; i++)
            res.data[i] += other.data[i];

        return res;
    }

    Vector3D operator-(const Vector3D& other) const
    {
        Vector3D res(data);
        for (int i = 0; i  < 3 ; i++)
            res.data[i] -= other.data[i];

        return res;
    }

    Vector3D operator*(const double scalar) const
    {
       Vector3D res(data);
        for (int i = 0; i  < 3 ; i++)
            res.data[i] *= scalar;

        return res; 
    }

    Vector3D operator/(const double scalar) const
    {
       Vector3D res(data);
        for (int i = 0; i  < 3 ; i++)
            res.data[i] /= scalar;

        return res; 
    }

    Vector3D vecprod(const Vector3D &other) const
	{
	    return Vector3D(
            data[1] * other.data[2] - data[2] * other.data[1],
            data[2] * other.data[0] - data[0] * other.data[2],
            data[0] * other.data[1] - data[1] * other.data[0]
        );
	}

    double mod() const
    {
        return std::pow(data[0]*data[0] + data[1]*data[1] + data[2]*data[2], 0.5);
    }

	std::string to_string()
	{
    	std::ostringstream res;
    	res << data[0] << " " << data[1] << " " << data[2] << " ";
    	return res.str();
	}

    Vector3D normalize() const
	{
	    return Vector3D(data) / Vector3D(data).mod();
	}

    double dot(const Vector3D& other) const
	{
	    return data[0] * other.data[0]
             + data[1] * other.data[1]
             + data[2] * other.data[2];
	}

};


#endif