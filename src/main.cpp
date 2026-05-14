#include <iostream>
#include <string>
#include <map>

#include "clp/CLP.h"
#include "clp/Parcer.h"

#include "Telemetry.h"
#include "simulation/Simulation.h"

const std::string ROCKET_CONFIG = "../cfg/rocket.yaml";
const std::string SYSTEM_CONFIG = "../cfg/simulation.yaml";
const std::string MFL_DATA = "../cfg/origakafirsthorizons_results.txt";

/**
 * О функции main
 * В данной функции происходит инициализация основных параметров всей системы
 * работа с cli, парсинг, последующяя инициализация системы, а также запуск самой симуляции.
*/

int main(int argc, char *argv[])
{
    CLP clp(argc, argv);

    double h = clp.h();
    std::string method = clp.method();
    std::string output = clp.output();


    std::vector<std::pair<double, Vector3D>> mfldata = Parcer::parce_mfl(MFL_DATA);

    SimulationConfig sim_cfg = Parcer::parce_cfg(SYSTEM_CONFIG);
    Rocket rocket = Parcer::parce_rocket(ROCKET_CONFIG);

    System system(sim_cfg, rocket, method, mfldata, h);
    Telemetry telemetry(system, output);
    Simulation simulator(system, telemetry);

    simulator.Run();

    return 0;
}
