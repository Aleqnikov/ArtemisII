#ifndef ARTEMISII_STAGE_H
#define ARTEMISII_STAGE_H

#include "engine.h"

struct Stage
{
    double weight;
    double fuel_w;

    Engine engine;

    Stage(double weight, double fuel_w, std::string thottle);
};

#endif //ARTEMISII_STAGE_H