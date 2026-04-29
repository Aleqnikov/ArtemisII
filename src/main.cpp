#include <iostream>
#include <string>
#include <map>

#include "clp/CLP.h"
#include "clp/Parcer.h"

enum class
{
    BASE_CONFIGURATION = ".../",
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

    std::map<std::string, std::string> system_cnfg = Parcer.parce_cfg(BASE_CONFIGURATION);
    std::map<std::string, std::string> thrust_cnfg = Parcer.parce_thrust(path_to_thrust);

    // Парсим данные из JPL для получения вектора скорости. Сразу его читаем в файл.
    // как бы (t, (vx, vy))
    // TODO (vx vy vz)
    std::vector<std::pair<double, std::pair<double, double>> thottle = Parcer.parce_thottle(path_to_thottle);

    // МОжет thrust cfg переименовать в конфиг ракеты.
    System system(system_cnfg, thrust_cnfg, method, thottle, h);

    Telemetry telemetry(output);

    Simulator simulator(system, telemetry);

    simulator.Run();

    return 0;
}