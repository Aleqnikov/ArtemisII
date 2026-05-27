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


	bool apogee_target_reached = false;
	double target_apogee = 0.0; // Заполняется из MFL когда нужно
	int mod = 0;

	bool perigee_target_reached = false;  // ← добавить
	double target_perigee = 0.0;          // ← добавить

	bool prm_started = false;             // ← добавить (для фазы 0)

	Vector3D n(const Vector& current_X, double t) {
		return mfl.get_n(*this, current_X, t);
	}

    std::string log();

    System (SimulationConfig sim_cfg,
        Rocket rocket,
        std::string method,

        double h);
};


#endif //ARTEMISII_SYSTEM_H
