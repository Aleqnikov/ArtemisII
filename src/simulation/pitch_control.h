#ifndef ARTEMISII_PITCH_CONTROL_H
#define ARTEMISII_PITCH_CONTROL_H

#include "model/System.h"
#include "core/ode.h"
#include <algorithm>
#include <vector>

void step_control(System& system, double& t_0)
{
    const double EPS = 1e-7;
    double original_h = system.h;

    std::vector<Stage*> active_stages = system.rocket.get_active_stages();

    Vector new_X = system.method(system.t_curr, system.X, system.h, f, &system);
    double fuel_used = system.X.m - new_X.m;

    double beta_total = 0.0;
    for (auto* stage : active_stages) {
        if (stage->fuel_w > EPS)
            beta_total += stage->engine.get_beta(system.t_curr);
    }

    Stage* burnout_stage = nullptr;
    int burnout_id = -1;

    if (beta_total > 0.0) {
        for (auto const& [id, node] : system.rocket.stage_graph) {
            if (node.in_degree != 0) continue;
            Stage& stage = system.rocket.stages[id];
            if (stage.fuel_w <= EPS) continue;

            double beta_i = stage.engine.get_beta(system.t_curr);
            double fuel_used_i = fuel_used * (beta_i / beta_total);

            if (fuel_used_i >= stage.fuel_w - EPS) {
                burnout_stage = &stage;
                burnout_id = id;
                break;
            }
        }
    }

    if (burnout_id != -1) {
        // --- Бинарный поиск момента выгорания (h) ---
        double lo = 0.0, hi = system.h, base_h = system.h;
        double beta_stage = burnout_stage->engine.get_beta(system.t_curr);

        for (int i = 0; i < 60; ++i) {
            base_h = (lo + hi) / 2.0;
            Vector test_X = system.method(system.t_curr, system.X, base_h, f, &system);
            double test_fuel_used = system.X.m - test_X.m;
            double fuel_used_i = (beta_total > 0.0) ? test_fuel_used * (beta_stage / beta_total) : 0.0;
            double remainder = burnout_stage->fuel_w - fuel_used_i;

            if (std::abs(remainder) < 1e-9) break;
            if (remainder <= 0.0) hi = base_h;
            else                  lo = base_h;
        }

        system.h = base_h;
        new_X = system.method(system.t_curr, system.X, system.h, f, &system);
        double final_fuel_used = system.X.m - new_X.m;

        for (auto* s : active_stages) {
            double beta_i = s->engine.get_beta(system.t_curr);
            double fuel_used_i = (beta_total > 0.0) ? final_fuel_used * (beta_i / beta_total) : 0.0;
            s->fuel_w = std::max(0.0, s->fuel_w - fuel_used_i);
        }
        burnout_stage->fuel_w = 0.0;

        // --- ЛОГИКА ОТДЕЛЕНИЯ С ЗАЩИТОЙ ПОСЛЕДНЕЙ СТУПЕНИ ---
        // Если в графе больше одной вершины, значит есть что отделять.
        // Если осталась одна — это наш "финальный объект", его не трогаем.
        if (system.rocket.stage_graph.size() > 1) {
            // Вычитаем сухую массу отделяемой ступени из текущего вектора состояния
            new_X.m -= system.rocket.stages[burnout_id].weight;
            // Убираем узел из графа и обновляем зависимости
            system.rocket.separate_stage(burnout_id);
        } else {
            // Если ступень последняя, она просто летит дальше с нулевым запасом топлива.
            // Масса в new_X.m уже правильная (она включает сухую массу этой ступени).
        }

    } else {
        // Обычный шаг
        for (auto* s : active_stages) {
            double beta_i = s->engine.get_beta(system.t_curr);
            double fuel_used_i = (beta_total > 0.0) ? fuel_used * (beta_i / beta_total) : 0.0;
            s->fuel_w = std::max(0.0, s->fuel_w - fuel_used_i);
        }
    }

    system.X = new_X;
    system.t_curr += system.h;
    t_0 = system.t_curr;
    system.h = original_h;
}

#endif //ARTEMISII_PITCH_CONTROL_H