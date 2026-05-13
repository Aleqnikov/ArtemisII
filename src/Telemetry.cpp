#include "Telemetry.h"
#include "core/physics.h"
#include "core/moon.h"
#include "core/earth.h"
#include "model/System.h"

Telemetry::Telemetry(System& s, std::string output) : system(s) {
	outStream.open(output);
}

Telemetry::~Telemetry() {
	if (outStream.is_open()) {
		outStream.close();
	}
}

void Telemetry::log() {
	outStream << system.log()  << moon_position(system.t_curr).to_string() << std::endl;
}