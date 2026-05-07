#ifndef ARTEMISII_ROCKET_H
#define ARTEMISII_ROCKET_H

#include "stage.h"
#include "core/Vector3d.h"

struct ThrustResult { double F_total; double beta_total; };

struct Rocket
{
    Vector3D r;
    Vector3D v;

    std::vector<Stage> stages;

	std::vector<Stage> work_stages;
    std::map<int, std::vector<int>> flight_plan;

	double getMass();

	void init_start(Vector X);
    std::vector<int> Rocket::active_stage_ids(double t) const;
    ThrustResult Rocket::compute_thrust(double t) const;

    Rocket::Rocket(std::map<std::string, std::string> Rocket_cfg);
};


#endif //ARTEMISII_ROCKET_H