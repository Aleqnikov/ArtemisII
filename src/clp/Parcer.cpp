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


// Константа старта миссии Artemis II в Julian Date
const double ARTEMIS_2_START_JD = 2461132.44111111;

std::vector<std::pair<double, Vector3D>> Parcer::parce_mfl(std::string path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open MFL file: " + path);

    std::vector<std::pair<double, Vector3D>> result;
    std::string line;
    bool data_started = false;

    while (std::getline(file, line)) {
        // 1. Ищем начало данных
        if (!data_started) {
            if (line.find("$$SOE") != std::string::npos) data_started = true;
            continue;
        }
        // 2. Ищем конец данных
        if (line.find("$$EOE") != std::string::npos) break;

        // --- Читаем блок из 4-х строк для каждой точки времени ---

        // Строка 1: Julian Date (например, 2461132.582318345 = ...)
        double jd;
        std::istringstream ss_time(line);
        if (!(ss_time >> jd)) continue;

        // Переводим время: (Текущий JD - JD старта) * секунд в сутках
        double t_relative = (jd - ARTEMIS_2_START_JD) * 86400.0;

        // Строка 2: Позиция (X, Y, Z) - пропускаем
        if (!std::getline(file, line)) break;

        // Строка 3: Скорость (VX, VY, VZ) - это то, что нам нужно
        if (!std::getline(file, line)) break;

        double vx, vy, vz;
        // Заменяем символы 'V', 'X', 'Y', 'Z', '=' на пробелы для простого парсинга
        for (char &c : line) if (c == 'V' || c == 'X' || c == 'Y' || c == 'Z' || c == 'O' || c == '=') c = ' ';

        std::istringstream ss_vel(line);
        if (ss_vel >> vx >> vy >> vz) {
            result.push_back({t_relative, Vector3D(vx, vy, vz)});
        }

        // Строка 4: Доп. данные (LT, RG, RR) - пропускаем
        if (!std::getline(file, line)) break;
    }

    return result;
}
