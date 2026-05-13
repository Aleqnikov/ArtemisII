#ifndef ARTEMISII_PITCH_CONTROL_H
#define ARTEMISII_PITCH_CONTROL_H

#include "model/System.h"
#include "core/ode.h"

void step_control(System& system)
{
    // Пробуем сделать шаг h
    Vector new_X = system.method(system.t_curr, system.X, system.h, f, &system);
    double fuel_used = system.X.m - new_X.m;

    // Считаем beta каждой активной ступени и суммарную beta_total
    auto active_ids = system.rocket.active_stage_ids(system.t_curr);

    double beta_total = 0.0;
    for (int id : active_ids)
        beta_total += system.rocket.stages[id].engine.get_beta(system.t_curr);

    // Проверяем — не исчерпалась ли топливо у какой-то ступени
    int burnout_id = -1;
    if (beta_total > 0.0) {
        for (int id : active_ids) {
            Stage& stage = system.rocket.stages[id];
            if (stage.fuel_w <= 0.0) continue;

            double beta_i = system.rocket.stages[id].engine.get_beta(system.t_curr);
            double fuel_used_i = fuel_used * (beta_i / beta_total);

            if (fuel_used_i >= stage.fuel_w) {
                burnout_id = id;
                break; // одна ступень за шаг
            }
        }
    }

    if (burnout_id >= 0) {
        // Бинарным поиском находим точный шаг, при котором топливо ступени исчерпывается
        Stage& stage = system.rocket.stages[burnout_id];
        double beta_stage = stage.engine.get_beta(system.t_curr);

        double lo = 0.0, hi = system.h;
        double base_h = system.h;
        const int max_iter = 60;

        for (int i = 0; i < max_iter; ++i) {
            base_h = (lo + hi) / 2.0;
            new_X = system.method(system.t_curr, system.X, base_h, f, &system);
            fuel_used = system.X.m - new_X.m;

            double fuel_used_i = (beta_total > 0.0)
                ? fuel_used * (beta_stage / beta_total)
                : 0.0;

            double remainder = stage.fuel_w - fuel_used_i;

            if (remainder > 0.0 && remainder / stage.fuel_w < 1e-6) break;

            if (remainder <= 0.0)
                hi = base_h;
            else
                lo = base_h;
        }

        // Обнуляем топливо выгоревшей ступени, остальным списываем пропорционально
        for (int id : active_ids) {
            Stage& s = system.rocket.stages[id];
            if (s.fuel_w <= 0.0) continue;

            double beta_i = system.rocket.stages[id].engine.get_beta(system.t_curr);
            double fuel_used_i = (beta_total > 0.0)
                ? fuel_used * (beta_i / beta_total)
                : 0.0;

            if (id == burnout_id)
                s.fuel_w = 0.0;
            else
                s.fuel_w -= fuel_used_i;
        }

        system.h = base_h;

    } else {
        // Всё нормально — просто списываем топливо пропорционально
        for (int id : active_ids) {
            Stage& s = system.rocket.stages[id];
            if (s.fuel_w <= 0.0) continue;

            double beta_i = system.rocket.stages[id].engine.get_beta(system.t_curr);
            double fuel_used_i = (beta_total > 0.0)
                ? fuel_used * (beta_i / beta_total)
                : 0.0;

            s.fuel_w -= fuel_used_i;
        }
    }

    system.X = new_X;
    system.t_curr += system.h;
}

#endif //ARTEMISII_PITCH_CONTROL_H
