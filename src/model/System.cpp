#include "System.h"

System::System (SimulationConfig sim_cfg,
        Rocket rocket,
        std::string method,
        std::vector<std::pair<double, std::pair<double, double>> thottle_control,
        double h) : h(h), t_curr(0), t_end(sim_cfg.t), rocket(rocket)
{
	if (method == "rk4") method = rk4;
	if (method == "AD") method  = ad;

	X = Vector(sim_cfg.r_x, sim_cfg.r_y, sim_cfg.r_z, sim_cfg.v_x, sim_cfg.v_y, sim_cfg.v_z,rocket.getMass());

	MFL(thottle_contorl);

}