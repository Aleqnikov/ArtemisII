#include "System.h"
#include "../core/numerical_methods/rk45.h"

#include <sstream>

std::string System::log() {
    std::ostringstream res;
    res << t_curr << " ";
    res << X.r.to_string();
    res << X.v.to_string();
    res << X.m << " ";
    return res.str();
}


System::System(SimulationConfig sim_cfg, Rocket rocket, std::string method_i,
                double h)
    : h(h),
      t_curr(0),
      t_end(sim_cfg.t_end),
      rocket(rocket),
      X({sim_cfg.r_x, sim_cfg.r_y, sim_cfg.r_z} , {sim_cfg.v_x, sim_cfg.v_y, sim_cfg.v_z}, rocket.getMass()),
      mfl()
{
    method = RK4;

}
