#ifndef ARTEMISII_ENGINE_H
#define ARTEMISII_ENGINE_H

#include "core/physics.h"

/**
 * Условно логика такая - "двигатель за какой то определенный промежуток времени расходует
 * какое то количество топлива, затем это сообщяется выше. То есть двигатель есть компонент ракеты
 * при этом он не то чтобы неразрывно связан со ступенью, он как бы существует в вакууме.
 */

struct Engine
{
    double I_sp;
    double T_max;

    /**
     * Тяга двигателя в каждый момент времени. Пока непонятно как определить корректно,
     * но это как бы временная вернсия.
     *                    (t, f(t))
     */
    std::vector<std::pair<double, double>> throttle;

    double get_throttle(double t) const {
        if (throttle.empty()) return 0.0;
        if (t <= throttle.front().first) return throttle.front().second;
        if (t >= throttle.back().first)  return throttle.back().second;
        for (size_t i = 1; i < throttle.size(); ++i) {
            if (t <= throttle[i].first) {
                auto [t0, f0] = throttle[i-1];
                auto [t1, f1] = throttle[i];
                return f0 + (f1 - f0) * (t - t0) / (t1 - t0);
            }
        }
        return 0.0;
    }

    double get_thrust(double t) const {
        return T_max * get_throttle(t);
    }

    double get_beta(double t) const {
        
        double F = get_thrust(t);
        return (I_sp > 0.0 && F > 0.0) ? F / (I_sp * Consts::g0) : 0.0;
    }

    bool is_burning(double t) const { return get_throttle(t) > 0.0; }

    Engine(double I_sp, double T_max, std::string thottle_str)
    : I_sp(I_sp), T_max(T_max) {};

};


#endif //ARTEMISII_ENGINE_H