#include "CLP.h"

#include "CLI11.hpp"

CLP::CLP(int argc, char** argv) : argc(argc), argv(argv)
{
    CLI::App app("Artemis-2 simulation.");

    app.add_option("-m,--method", m_method, "Numerical method: rk4, Adams");
    app.add_option("-s,--step", m_h, "Override simulation step size");
    app.add_option("-t,--thrust", m_path_to_thrust, "Path to thrust program");
    app.add_option("-f,--thottle", m_path_to_thottle, "Path to thottle program.");
    app.add_option("-o,--output", m_output, "Path to output file.");

    try {
    	app.parse(argc, argv);
	} catch (const CLI::ParseError& e) {
    	app.exit(e);
	}
};
