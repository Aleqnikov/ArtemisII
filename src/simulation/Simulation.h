#ifndef ARTEMISII_SIMULATION_H
#define ARTEMISII_SIMULATION_H

#include "model/System.h"
#include "Telemetry.h"

class Simulation
{
private:
    System system;

    Telemetry telemetry;

public:
    void Run();


    Simulation(System& system, Telemetry& telemetry) : system(system), telemetry(telemetry) {};

};


#endif //ARTEMISII_SIMULATION_H