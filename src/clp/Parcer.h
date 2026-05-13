#ifndef ARTEMISII_PARCER_H
#define ARTEMISII_PARCER_H

#include "model/rocket/Rocket.h"

/**
 * Утильный класс.
 */

struct SimulationConfig {
    std::string name;
    double t_end;

    double r_x, r_y, r_z;
    double v_x, v_y, v_z;

    double earth_angle;
    double earth_mass;
    double earth_radius;
    double earth_mu;

    double moon_x, moon_y, moon_z;
    double moon_mass;
    double moon_radius;
};

class Parcer
{
public:
    static Rocket parce_rocket(std::string path);
    static SimulationConfig parce_cfg(std::string path);
    static std::vector<std::pair<double, Vector3D>> parce_mfl(std::string path);
};


#endif //ARTEMISII_PARCER_H
