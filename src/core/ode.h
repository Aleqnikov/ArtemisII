#ifndef ARTEMISII_ODE_H
#define ARTEMISII_ODE_H

#include "core/Vector.h"
#include "core/physics.h"
#include "core/moon.h"

inline Vector f(double t, Vector X, void* context) {
    System* system = static_cast<System*>(context);  

    Vector3D rM = moon_position(t);
    Vector3D dr = rM - X.r;

    Vector res(X.r, X.v, X.m);
    res.r = X.v;

    auto [F_total, beta_total] = system->rocket.compute_thrust(t);

    res.v = X.r * (-Consts::mu_E / std::pow(X.r.mod(), 3))
          + (dr / std::pow(dr.mod(), 3) - rM / std::pow(rM.mod(), 3)) * Consts::mu_M
          + system->n(t) * (F_total / X.m);

    res.m = -beta_total;

    return res;
}



#endif //ARTEMISII_ODE_H