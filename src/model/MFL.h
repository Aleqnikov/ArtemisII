#ifndef ARTEMISII_MFL_H
#define ARTEMISII_MFL_H

#include <vector>
#include <utility>
#include "../core/Vector3d.h"

class Control;

class System;

/**
 * Missile Flight Control
 * Управление полётом ракеты через два угла:
 *   theta (θ) — тангаж: угол между вектором тяги и горизонталью [-π/2, π/2]
 *   psi   (ψ) — рыскание: азимут в горизонтальной плоскости [-π, π]
 *
 * Локальный базис (up, east, north) задаётся константами старта.
 * Вектор тяги восстанавливается как:
 *   n = up·sin(θ) + east·cos(θ)·sin(ψ) + north·cos(θ)·cos(ψ)
 */


class MFL {
private:
    // Таблица управления: время → (θ, ψ)
    std::vector<std::pair<double, Control>> mfl;

    // Перевод углов в единичный вектор тяги
    static Vector3D angles_to_n(double theta, double psi,
                                 const Vector3D& up,
                                 const Vector3D& east,
                                 const Vector3D& north);

    // Перевод вектора скорости в углы управления
    static Control velocity_to_control(const Vector3D& vel,
                                        const Vector3D& up,
                                        const Vector3D& east,
                                        const Vector3D& north);

public:
    explicit MFL(std::vector<std::pair<double, Control>> mfl_data)
        : mfl(std::move(mfl_data)) {}

    // Основной метод: возвращает единичный вектор тяги в момент t
    Vector3D get_n(System system, double t);

    // Вспомогательный: получить текущие углы (для логирования/телеметрии)
    Control get_control(System system, double t);
};

#endif // ARTEMISII_MFL_H