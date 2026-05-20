#ifndef ARTEMISII_SYSTEM_H
#define ARTEMISII_SYSTEM_H

#include "rocket/Rocket.h"
#include "../core/Vector.h"
#include "MFL.h"
#include "../clp/Parcer.h"

// Тип указателя на численный метод
using MethodFn = Vector(*)(double t, const Vector& y, double h,
                           Vector(*f)(double t, const Vector y, void* ctx),
                           void* ctx);

// Класс, что описывает текущее состояние системы, состоит в основном из вектора
// значений, и текущей ракеты.
class System
{
public:
    double h;
    double t_curr;
    double t_end;
    Vector X;
    Rocket rocket;
    MethodFn method;
    MFL mfl;

	Vector3D n(const Vector& current_X, double t) {
		return mfl.get_n(*this, current_X, t);
	}

    std::string log();

    System (SimulationConfig sim_cfg,
        Rocket rocket,
        std::string method,
        std::vector<std::pair<double, Vector3D>> thottle_control,
        double h);
};


#endif //ARTEMISII_SYSTEM_H
