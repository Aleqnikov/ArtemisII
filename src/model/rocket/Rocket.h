#ifndef ARTEMISII_ROCKET_H
#define ARTEMISII_ROCKET_H

#include "stage.h"
#include "core/Vector3d.h"

struct Rocket
{
    Vector3D r;
    Vector3D v;

    std::vector<Stage> stages;

	std::vector<Stage> work_stages;
    std::map<int, std::vector<int>> flight_plan;

	double getMass();

	void init_start(Vector X);

    Rocket::Rocket(std::map<std::string, std::string> Rocket_cfg);
};


#endif //ARTEMISII_ROCKET_H