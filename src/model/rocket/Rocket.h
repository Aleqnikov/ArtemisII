#ifndef ARTEMISII_ROCKET_H
#define ARTEMISII_ROCKET_H

#include "stage.h"

struct Rocket
{
    std::vector<Stage> stages;
    std::vector<Stage> work_stages;

    Rocket(std::map<std::string, std::string> Rocket_cfg);
};


#endif //ARTEMISII_ROCKET_H