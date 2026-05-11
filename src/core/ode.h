#ifndef ARTEMISII_ODE_H
#define ARTEMISII_ODE_H

#include "core/Vector.h"
#include "core/physics.h"
#include "core/moon.h"

Vector f(double t, Vector X, void* context) {
    System* system = static_cast<System*>(context);  

    Vector rM = moon_position(t);  
    Vector dr = rM - X.r;

    Vector res;
    res.r = X.v;

    auto [F_total, beta_total] = system->rocket.compute_thrust(t);

    res.v = (-Consts::mu_E) * X.r / std::pow(X.r.mod(), 3)
          + Consts::mu_M * (dr / std::pow(dr.mod(), 3) - rM / std::pow(rM.mod(), 3))
          + (F_total / X.m) * system->n(t); // TODO поправить на MFL
    
    res.m = -beta_total;

    return res;
}



#endif //ARTEMISII_ODE_H