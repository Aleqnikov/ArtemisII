#include "Simulation.h"

#include "pitch_control.h"

void Simulation::Run()
{
	double t_0  = 0;

	while (t_0 < system.t_end)
	{
		step_control(system);
		telemetry.log();
	}
}