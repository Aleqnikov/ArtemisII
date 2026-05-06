#ifndef ARTEMISII_STAGE_H
#define ARTEMISII_STAGE_H

#include "engine.h"

struct Stage
{
    double weight;
    double fuel_w;

    Engine engine;

    Stage::Stage(double weight, double fuel_w, std::string thottle)
    : weight(weight), fuel_w(fuel_w), engine(0, 0, thottle) {};
};

#endif //ARTEMISII_STAGE_H