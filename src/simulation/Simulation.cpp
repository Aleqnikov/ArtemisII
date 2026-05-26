#include "Simulation.h"
#include <iostream>
#include "pitch_control.h"

void Simulation::Run()
{
	double t_0  = 0;

	while (t_0 < system.t_end)
	{
		if (system.rocket.get_active_stages().size() == 1) {
			t_0 += 0;
		}
		if (system.X.r.mod() < 6371000)
		{
			break;
		}
		step_control(system, t_0);
		telemetry.log();
	}
}