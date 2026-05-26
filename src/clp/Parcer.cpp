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


// Константа Julian Date старта миссии Artemis II
const double ARTEMIS_2_START_JD = 2461132.44111111;



namespace launch {
    const Vector3D up   ( 0.0233457021,  0.9494782251, -0.3129633815);  // эклиптика J2000
    const Vector3D east (-0.9996462375,  0.0181802840, -0.0194133241);  // эклиптика J2000
}

std::vector<std::pair<double, Vector3D>> Parcer::parce_mfl(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open MFL file: " + path);

    const Vector3D north = launch::up.vecprod(launch::east);

    constexpr double EPS   = 0.409092804222;
    const double cos_e = std::cos(EPS);
    const double sin_e = std::sin(EPS);

    std::vector<std::pair<double, Vector3D>> result;
    std::string line;
    bool data_started = false;

    while (std::getline(file, line)) {
        if (!data_started) {
            if (line.find("$$SOE") != std::string::npos) data_started = true;
            continue;
        }
        if (line.find("$$EOE") != std::string::npos) break;

        // Строка 1: Julian Date
        double jd;
        std::istringstream ss_time(line);
        if (!(ss_time >> jd)) continue;
        double t = (jd - ARTEMIS_2_START_JD) * 86400.0;

        // Строка 2: позиция (пропускаем)
        if (!std::getline(file, line)) break;

        // Строка 3: скорость VX VY VZ (ECI J2000, км/с)
        if (!std::getline(file, line)) break;
        for (char& c : line)
            if (c=='V'||c=='X'||c=='Y'||c=='Z'||c=='O'||c=='=') c = ' ';

        double vx, vy, vz;
        std::istringstream ss_vel(line);
        if (!(ss_vel >> vx >> vy >> vz)) {
            std::getline(file, line);
            continue;
        }

        // ECI → эклиптика J2000, км/с → м/с
        double vx_ecl = vx;
        double vy_ecl =  vy * cos_e + vz * sin_e;
        double vz_ecl = -vy * sin_e + vz * cos_e;
        Vector3D vel(vx * 1000, vy * 1000, vz * 1000);



        result.push_back({ t, vel });

        // Строка 4: LT RG RR (пропускаем)
        if (!std::getline(file, line)) break;
    }

    return result;
}