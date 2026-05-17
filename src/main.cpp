#include <iostream>
#include <string>

#include "clp/CLP.h"
#include "clp/Parcer.h"

#include "Telemetry.h"
#include "simulation/Simulation.h"

const std::string ROCKET_CONFIG = "../cfg/rocket.yaml";
const std::string SYSTEM_CONFIG = "../cfg/simulation.yaml";
const std::string MFL_DATA      = "../cfg/origakafirsthorizons_results.txt";

// Тайминг MECO — локальная константа, не тянем зависимость от Shooting.cpp
static constexpr double T_MECO = 486.0;

int main(int argc, char *argv[]) {
    CLP clp(argc, argv);

    double      h      = clp.h();
    std::string method = clp.method();
    std::string output = clp.output();

    std::vector<std::pair<double, Control>> mfldata = Parcer::parce_mfl(MFL_DATA);
    SimulationConfig sim_cfg = Parcer::parce_cfg(SYSTEM_CONFIG);
    Rocket rocket = Parcer::parce_rocket(ROCKET_CONFIG);

    System system(sim_cfg, rocket, method, mfldata, h);

    // ── Целевая точка эфемерид (JPL Horizons, ICRF J2000 = ECI J2000) ─────────
    //
    // $$SOE
    // 2461132.582318345 = A.D. 2026-Apr-02 01:58:32.3050 TDB
    //   X =-2.444252045558597E+04  Y =-1.432289635529278E+04  Z =-1.284178287937772E+03 [км]
    //  VX =-1.844086721526654E+00  VY =-3.873750473840291E+00  VZ =-3.363918800418908E-01 [км/с]

    constexpr double KM  = 1.0e3;
    constexpr double EPS = 0.409092804222;
    const double cos_e = std::cos(EPS);
    const double sin_e = std::sin(EPS);

    const double r_eci_x = -2.444252045558597e4 * KM;
    const double r_eci_y = -1.432289635529278e4 * KM;
    const double r_eci_z = -1.284178287937772e3 * KM;

    const double v_eci_x = -1.844086721526654e0 * KM;
    const double v_eci_y = -3.873750473840291e0 * KM;
    const double v_eci_z = -3.363918800418908e-1 * KM;

    // ECI → эклиптика J2000
    Vector3D r_target(
        r_eci_x,
        r_eci_y * cos_e + r_eci_z * sin_e,
       -r_eci_y * sin_e + r_eci_z * cos_e
    );
    Vector3D v_target(
        v_eci_x,
        v_eci_y * cos_e + v_eci_z * sin_e,
       -v_eci_y * sin_e + v_eci_z * cos_e
    );

    // t_target = (JD_эфемерид - JD_старта) * 86400
    const double t_target = (2461132.582318345 - 2461132.44111111) * 86400.0;

    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║       Artemis II — метод стрельбы            ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n\n";
    std::cout << "[main] t_target   = " << t_target << " с ("
              << t_target / 3600.0 << " ч)\n";
    std::cout << "[main] |r_target| = " << r_target.mod() / 1e3 << " км\n";
    std::cout << "[main] |v_target| = " << v_target.mod()       << " м/с\n";
    std::cout << "[main] r_target (ecl) = ("
              << r_target.data[0] / 1e3 << ", "
              << r_target.data[1] / 1e3 << ", "
              << r_target.data[2] / 1e3 << ") км\n";
    std::cout << "[main] v_target (ecl) = ("
              << v_target.data[0] << ", "
              << v_target.data[1] << ", "
              << v_target.data[2] << ") м/с\n\n";

    std::cout << "[main] Схема полёта:\n"
              << "  0 .. 486 с       — gravity turn (SRB + Core Stage)\n"
              << "  486 .. 2942 с    — коастинг на парковочной орбите (~1 виток)\n"
              << "  2942 .. 2962 с   — PRM (ICPS, circularisation)\n"
              << "  2962 .. ~5400 с  — коастинг-2 (выход к перигею TLI)\n"
              << "  ~5400 .. t_tli   — TLI (ICPS, prograde)\n"
              << "  t_tli .. " << t_target << " с — баллистика к точке эфемерид\n\n";

    // ── Метод стрельбы ────────────────────────────────────────────────────────
    std::cout << "[main] Запуск двумерного метода стрельбы...\n\n";





    System system_full(sim_cfg, rocket, method, mfldata, h);

    std::cout << "[main] Запуск полной симуляции → " << output << "\n";
    Telemetry  telemetry(system_full, output);
    Simulation simulator(system_full, telemetry);
    simulator.Run();

    std::cout << "[main] Готово.\n";
    return 0;
}