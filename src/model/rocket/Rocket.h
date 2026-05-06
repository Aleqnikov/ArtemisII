#ifndef ARTEMISII_ROCKET_H
#define ARTEMISII_ROCKET_H

#include "stage.h"

struct Rocket
{
    std::vector<Stage> stages;

	std::vector<Stage> work_stages;
    std::map<int, std::vector<int>> flight_plan;

    Rocket::Rocket(std::map<std::string, std::string> Rocket_cfg);
};


#endif //ARTEMISII_ROCKET_H