#include "MFL.h"
#include "System.h"
#include <algorithm>
#include <cmath>

namespace launch {
    constexpr double UP_X   =  0.0233457021;
    constexpr double UP_Y   =  0.9494782251;
    constexpr double UP_Z   = -0.3129633815;

    constexpr double EAST_X = -0.9996462375;
    constexpr double EAST_Y =  0.0181802840;
    constexpr double EAST_Z = -0.0194133241;
}

// Целевая точка от профессора (JPL Horizons).
// ВАЖНО: переведено из км и км/с в метры и м/с!
const Vector3D TARGET_POS(-24442520.46, -14322896.36, -1284178.29);
const Vector3D TARGET_VEL(-1844090.0, -3873750.0, -336390.0);

MFL::MFL(const std::vector<std::pair<double, Control>>& mfldata) : data(mfldata) {
    // Высчитываем нормаль к плоскости орбиты Луны через векторное произведение (h = r x v)
    Vector3D h = TARGET_POS.vecprod(TARGET_VEL);
    moon_plane_normal = h.normalize();
}

Vector3D MFL::angles_to_n(double theta, double psi,
                          const Vector3D& up,
                          const Vector3D& east,
                          const Vector3D& north)
{
    return up    * std::sin(theta)
         + east  * (std::cos(theta) * std::sin(psi))
         + north * (std::cos(theta) * std::cos(psi));
}

Control MFL::velocity_to_control(const Vector3D& vel,
                                 const Vector3D& up,
                                 const Vector3D& east,
                                 const Vector3D& north)
{
    double len = vel.mod();
    if (len < 1e-12)
        return Control{ M_PI / 2.0, 0.0 };

    Vector3D nv = vel / len;
    double theta = std::asin(std::clamp(nv.dot(up), -1.0, 1.0));
    double psi   = std::atan2(nv.dot(east), nv.dot(north));
    return Control{ theta, psi };
}

Control MFL::get_control(const System& system, const Vector& X, double t)
{
    // Оставляем заглушку. Теперь ракетой управляет напрямую get_n
    return Control{ 0 , 0 };
}


int mod = 0;
Vector3D MFL::get_n(const System& system, const Vector& X, double t)
{

	if (t < 150) {

		return system.X.r / system.X.r.mod();
	}


	double A = -636638.01822281;
	double B = -21603260.16260338;
	double C = 252281532.75012207;
	double D = -1007827223239.6875;

	double num = A * system.X.r.data[0] + B * system.X.r.data[1] + C * system.X.r.data[2] + D;
	double Den = A*A + B*B + C*C;

	Vector3D pre_res(-num/Den * A, -num/Den * B, -num/Den * C);
	pre_res = (pre_res.normalize() + X.v.normalize() + X.r.normalize()).normalize();


	return (pre_res - pre_res.vecprod(Vector3D(A, B, C)).normalize()).normalize();







}