#ifndef ARTEMISII_MFL_H
#define ARTEMISII_MFL_H

#include <tuple>
/**
 * Missile Flight Control
 * Управление полетом ракеты. В данном файле находися программный коплекс
 * для управления полетом ракеты.
 */

// TODO опять же, у нас 3d
class MFL
{
private:
    //          (t, f(t))
    std::vector<std::pair<double, double>> mfl;
public:
    std::tuple<double, double, double> get_n(double t);

    MFL(std::vector<std::pair<double, double>>);
};


#endif //ARTEMISII_MFL_H