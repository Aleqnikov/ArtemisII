#include "Telemetry.h"

#include "core/physics.h"

Telemetry::Telemetry(System& system, std::string output)
{
	this->system = &system;
	outStream.open(output);
}

~Telemetry();
{
	if (outStream.is_open()) {
		outStream.close();
	}
}

void Telemetry::log()
{
	outStrem << system.log() << moon.current_pos(system.t_curr) << earth.current_angle(system.t_curr) << std::endl;
}