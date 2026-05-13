#ifndef ARTEMISII_MFL_H
#define ARTEMISII_MFL_H

#include <tuple>
#include <vector>
#include "../core/Vector3d.h"


class System;
/**
 * Missile Flight Control
 * Управление полетом ракеты. В данном файле находися программный коплекс
 * для управления полетом ракеты.
 */

// TODO опять же, у нас 3d
class MFL
{
private:
    std::vector<std::pair<double, Vector3D>> mfl;
public:
    Vector3D get_n(System system, double t);



    MFL(std::vector<std::pair<double, Vector3D>> mfl_data): mfl(mfl_data) {};
};


#endif //ARTEMISII_MFL_H