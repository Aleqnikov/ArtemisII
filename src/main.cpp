#include <iostream>
#include <string>
#include <map>

#include "clp/CLP.h"
#include "clp/Parcer.h"

enum class
{
    ROCKET_CONFIG: "/cfg/rocket.yaml",
	SYSTEM_CONFIG: "/cfg/system.yaml"
    ..
};

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

    Rocket rocket = Parcer.parce_rocket(ROCKET_CONFIG);
    SimulationConfig sim_cfg = Parcer.parce_cfg(SYSTEM_CONFIG);

    // Парсим данные из JPL для получения вектора скорости. Сразу его читаем в файл.
    // как бы (t, (vx, vy))
    // TODO (vx vy vz)
    std::vector<std::pair<double, std::pair<double, double>> thottle = Parcer.parce_thottle(path_to_thottle);

    // МОжет thrust cfg переименовать в конфиг ракеты.
    System system(sim_cfg, rocket, method, thottle, h);

    Telemetry telemetry(system, output);

    Simulation simulator(system, telemetry);

    simulator.Run();

    return 0;
}