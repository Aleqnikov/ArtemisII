#ifndef ARTEMISII_TELEMETRY_H
#define ARTEMISII_TELEMETRY_H

#include "core/physics.h"
#include <fstream>


class System;

class Telemetry {
private:
    System& system; // Это ссылка
    std::ofstream outStream;
public:
    Telemetry(System& s, std::string output);
    ~Telemetry();
    void log();
};

#endif //ARTEMISII_TELEMETRY_H