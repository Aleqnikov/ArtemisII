#include <iostream>
#include <string>
#include <map>

#include "clp/CLP.h"
#include "clp/Parcer.h"

const std::string ROCKET_CONFIG = "../cfg/rocket.yaml";
const std::string SYSTEM_CONFIG = "../cfg/simulation.yaml";

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

    std::string path_to_thrust = clp.thrust();
    std::string path_to_thottle = clp.thottle();



    // Парсим данные из JPL для получения вектора скорости. Сразу его читаем в файл.
    // как бы (t, (vx, vy))
    // TODO (vx vy vz)
    std::vector<std::pair<double, std::pair<double, double>> thottle = Parcer.parce_thottle(path_to_thottle);

    SimulationConfig sim_cfg = Parcer::parce_cfg(SYSTEM_CONFIG);
    Rocket rocket = Parcer::parce_cfg(ROCKET_CONFIG); // Убедись, что метод перегружен

    System system(sim_cfg, rocket, method, {}, h);
    Telemetry telemetry(system, output);
    Simulation simulator(system, telemetry);

    simulator.Run();
    return 0;
}