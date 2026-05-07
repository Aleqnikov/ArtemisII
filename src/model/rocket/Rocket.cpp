#include "Rocket.h"


void Rocket::init_start(Vector X)
{
    r = Vector3D({X.r})
    v = Vector3D({X.v})
}

double getMass()
{
    double mass = 0;
    for (auto stage : stages)
    {
        mass += stage.weight + stage.fuel_w;
    }

    return mass;
}