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
    double t;
    Vector X;
    Rocket rocket;
    auto method;
    MFl mfl;

    System (std::map<std::string, std::string>,
        std::map<std::string, std::string>,
        std::string method,
        std::vector<std::pair<double, std::pair<double, double>>,
        double);
};


#endif //ARTEMISII_SYSTEM_H