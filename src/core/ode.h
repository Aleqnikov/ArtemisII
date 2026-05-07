#ifndef ARTEMISII_ODE_H
#define ARTEMISII_ODE_H

#include "Vector.h"
#include "physics.h"

Vector f (double t, Vector X, void *context)
{
    System* system = std::static_cast<System*>(context);

    Vector res(X);

    res.r = X.v;

    res.v =  (-Consts::mu_E) * X.r / std::pow(X.r.mod() , 3) +
            + Consts::mu_M * ((moon_position(t) - X.r ) / std::pow((moon_position(t) - X.r ).mod(), 3) - 
            moon_position(t) / std::pow(moon_position(t).mod(), 3)) + F(t)/m * n(t);

    res.m = -mu(t);

    return res;


}




#endif //ARTEMISII_ODE_H