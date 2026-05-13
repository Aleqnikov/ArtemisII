#include "Simulation.h"

#include "pitch_control.h"

void Simulation::Run()
{
	double t_0  = 0;

	while (t_0 < system.t_end)
	{

		if (t_0 > 13000)
		{
			t_0 += 0;
		}
		step_control(system, t_0);
		telemetry.log();
	}
}