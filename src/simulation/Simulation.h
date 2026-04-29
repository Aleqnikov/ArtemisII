#ifndef ARTEMISII_SIMULATION_H
#define ARTEMISII_SIMULATION_H


#include "../model/System.h"
#include "../Telemetry.h"

class Simulation
{
private:
    System system;

    Telemetry telemetry;

public:
    void Run()
    {
        double t_0 = 0;

        while (t_0 != System.t)
        {
            step_control(system);

            telemetry.log();
        }
    };

    Simulation(System system, Telemetry telemetry);

};


#endif //ARTEMISII_SIMULATION_H