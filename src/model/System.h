#ifndef ARTEMISII_SYSTEM_H
#define ARTEMISII_SYSTEM_H

#include "rocket/Rocket.h"
#include "../core/Vector.h"

// Класс, что описыват текущее состояние системы,  состоит в основном из вектора
// значений, и текущей ракеты.
class System
{
public:
    double h;
    double t_curr;
	double t_end;
    Vector X;
    Rocket rocket;
    auto method;
    MFl mfl;

    System (SimulationConfig sim_cfg,
        Rocket rocket,
        std::string method,
        std::vector<std::pair<double, std::pair<double, double>> thottle_control,
        double h);
};


#endif //ARTEMISII_SYSTEM_H