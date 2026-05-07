#include <array>
#include <cmath>

class Vector3D
{
public:
    std::array<double, 3> data;

    Vector3D(std::array<double, 3> pars) : data(pars) {};

    Vector3D operator+(const Vector3D &other) 
    {
        Vector3D res(data);

        for (int i = 0; i  < 3 ; i++)
            res[i] += other.data[i];

        return res;
    }

    Vector3D operator-(const Vector3D& other)
    {
        Vector3D res(data);
        for (int i = 0; i  < 3 ; i++)
            res[i] -= other.data[i];

        return res;
    }

    Vector3D operator*(const double scalar)
    {
       Vector3D res(data);
        for (int i = 0; i  < 3 ; i++)
            res[i] *= scalar;

        return res; 
    }

    Vector3D operator/(const double scalar)
    {
       Vector3D res(data);
        for (int i = 0; i  < 3 ; i++)
            res[i] /= scalar;

        return res; 
    }

    double mod()
    {
        return std::pow(data[0]*data[0] + data[1]*data[1] + data[2]*data[2], 0.5)
    }


};
