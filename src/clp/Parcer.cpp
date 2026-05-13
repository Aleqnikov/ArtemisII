#include "Parcer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

Rocket Parcer::parce_rocket(std::string path) {
    YAML::Node config = YAML::LoadFile(path);

    Rocket rocket;

    for (auto const& node : config["rocket"]["stages"]) {
        Stage stage(node["dry_mass"].as<double>(), node["fuel_mass"].as<double>(), "");
        stage.engine.I_sp  = node["engines"]["i_sp"].as<double>();
        stage.engine.T_max = node["engines"]["t_max"].as<double>();

        auto times = node["engines"]["thottle_prog"]["time"].as<std::vector<double>>();
        auto throt = node["engines"]["thottle_prog"]["throttle"].as<std::vector<double>>();
        for (size_t i = 0; i < times.size(); ++i)
            stage.engine.throttle.push_back({times[i], throt[i] / 100.0});

        rocket.stages.push_back(stage);
    }

    // flight_plan — только для построения дерева, дальше не нужен
    std::map<int, std::vector<int>> flight_plan;
    auto worked_logic = config["rocket"]["global_parametrs"]["stage_worked"];
    for (YAML::const_iterator it = worked_logic.begin(); it != worked_logic.end(); ++it) {
        int phase = it->first.as<int>();
        for (int id : it->second.as<std::vector<int>>())
            flight_plan[phase].push_back(id - 1); // 1-based → 0-based
    }

    rocket.build_graph(flight_plan); // строим дерево и забываем про flight_plan
    return rocket;
}


SimulationConfig Parcer::parce_cfg(std::string path)
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


std::vector<std::pair<double, Vector3D>> Parcer::parce_mfl(std::string path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open MFL file: " + path);

    std::vector<std::pair<double, Vector3D>> result;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        double t, vx, vy, vz;
        if (!(ss >> t >> vx >> vy >> vz)) continue;

        result.push_back({t, Vector3D(vx, vy, vz)});
    }

    return result;
}
