#include "Rocket.h"


void Rocket::init_start(Vector X) {
    r = X.r;
    v = X.v;
}

double Rocket::getMass() const {
    double m = 0.0;
    for (const auto& s : stages)
        m += s.weight + s.fuel_w;
    return m;
}


std::vector<int> Rocket::active_stage_ids(double t) const {

    int current_phase = flight_plan.begin()->first;
    for (auto const& [phase, ids] : flight_plan) {

        bool any_burning = false;
        for (int id : ids)
            if (stages[id].engine.is_burning(t)) { any_burning = true; break; }
        if (any_burning) { current_phase = phase; break; }
    }
    return flight_plan.at(current_phase);
}


ThrustResult Rocket::compute_thrust(double t) const {
    double F = 0.0, beta = 0.0;
    for (int id : active_stage_ids(t)) {
        const auto& s = stages[id];
        if (s.fuel_w <= 0.0) continue;
        F    += s.engine.get_thrust(t);
        beta += s.engine.get_beta(t);
    }
    return {F, beta};
}