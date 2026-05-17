#ifndef ARTEMISII_CONTROL_H
#define ARTEMISII_CONTROL_H

/**
 * Параметры управления ракетой через два угла.
 *   theta — тангаж (pitch): угол между вектором тяги и горизонталью, рад [-π/2, π/2]
 *   psi   — рыскание (yaw): азимут в горизонтальной плоскости, рад [-π, π]
 */
struct Control {
    double theta;
    double psi;
};

#endif // ARTEMISII_CONTROL_H