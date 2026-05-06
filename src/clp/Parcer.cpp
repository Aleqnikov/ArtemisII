#include "Parcer.h"
#include <yaml-cpp/yaml.h>

static Rocket Parcer::parce_cfg(std::string path) {
    YAML::Node config = YAML::LoadFile(path);

    std::map<std::string, std::string> meta;
    meta["name"] = config["rocket"]["metadata"]["name"].as<std::string>();

    Rocket rocket(meta);

    for (auto const& node : config["rocket"]["stages"]) {
        double dry = node["dry_mass"].as<double>();
        double fuel = node["fuel_mass"].as<double>();

        Stage stage(dry, fuel, "");

        stage.engine.I_sp = node["engines"]["i_sp"].as<double>();
        stage.engine.T_max = node["engines"]["t_max"].as<double>();

        auto times = node["engines"]["thottle_prog"]["time"].as<std::vector<double>>();
        auto throt = node["engines"]["thottle_prog"]["throttle"].as<std::vector<double>>();

        for(size_t i = 0; i < times.size(); ++i) {
            stage.engine.thottle.push_back({times[i], throt[i]});
        }

        rocket.stages.push_back(stage);
    }


    auto worked_logic = config["rocket"]["global_parametrs"]["stage_worked"];

    for (YAML::const_iterator it = worked_logic.begin(); it != worked_logic.end(); ++it) {
        int phase_number = it->first.as<int>();
        std::vector<int> active_stages = it->second.as<std::vector<int>>();

        for(int &id : active_stages) {
            rocket.flight_plan[phase_number].push_back(id - 1);
        }
    }

    return rocket;
}




static SimulationConfig Parcer::parce_cfg(std::string path)
{
    YAML::Node config = YAML::LoadFile(path);
    YAML::Node sim_node = config["simulation"];

    SimulationConfig sc;

    sc.name = sim_node["metadata"]["name"].as<std::string>();
    sc.t_end = sim_node["parametrs"]["t_end"].as<double>();

    auto start = sim_node["start_parametrs"];
    sc.r_x = start["r_x"].as<double>();
    sc.r_y = start["r_y"].as<double>();
    sc.r_z = start["r_z"].as<double>();
    sc.v_x = start["v_x"].as<double>();
    sc.v_y = start["v_y"].as<double>();
    sc.v_z = start["v_z"].as<double>();

    auto e = sim_node["Earth"];
    sc.earth_angle  = e["angle"].as<double>();
    sc.earth_mass   = e["mass"].as<double>();
    sc.earth_radius = e["radius"].as<double>();
    sc.earth_mu     = e["mu"].as<double>();

    auto m = sim_node["moon"];
    sc.moon_x      = m["x"].as<double>();
    sc.moon_y      = m["y"].as<double>();
    sc.moon_z      = m["z"].as<double>();
    sc.moon_mass   = m["mass"].as<double>();
    sc.moon_radius = m["radius"].as<double>();

    return sc;
}