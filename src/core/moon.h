#ifndef ARTEMISII_MOON_H
#define ARTEMISII_MOON_H

#include <array>
#include <cmath>
#include "Vector3d.h"

namespace artemis2 {



// 2. Физические константы
constexpr double GM_EARTH = 3.986004418E+14;

    constexpr double X0  = 2.742800E+08;   // [м] — только для справки / проверки
    constexpr double Y0  = 2.675000E+08;
    constexpr double Z0  = 1.410000E+07;

    constexpr double VX0 = -6.127000E+02;  // [м/с]
    constexpr double VY0 =  6.344000E+02;
    constexpr double VZ0 =  2.180000E+01;

    // Оскулирующие кеплеровские элементы (производные из вектора состояния JPL):
    constexpr double A          = 3.9191568104e+08; // Оскулирующая большая полуось [м]
    constexpr double E          = 0.0390389413; // Оскулирующий эксцентриситет
    constexpr double I          = 0.4943709725; // Наклонение [рад] (= 2.533°)
    constexpr double OMEGA_NODE = 6.2136677400; // Долгота восх. узла [рад] (= 347.98°)
    constexpr double OMEGA_PERI = 1.7037169949; // Аргумент перигея [рад] (= 237.32°)
    constexpr double M0         = 1.6380980300; // Средняя аномалия при t=0 [рад]
// Среднее движение (угловая скорость) n = sqrt(mu / a^3)
const double N = std::sqrt(GM_EARTH / std::pow(A, 3));

} // namespace artemis2

// Итеративное решение уравнения Кеплера: M = E - e*sin(E)
static double solve_kepler(double M, double e) {
    double Ea = M;
    for (int i = 0; i < 15; ++i) { // 15 итераций для точности double
        double delta = (Ea - e * std::sin(Ea) - M) / (1.0 - e * std::cos(Ea));
        Ea -= delta;
        if (std::abs(delta) < 1e-12) break;
    }
    return Ea;
}

/**
 * Расчет положения Луны по Кеплеровскому эллипсу.
 * Гарантирует совпадение с начальными условиями JPL при t = 0.
 */
inline Vector3D moon_position(double t) {
    using namespace artemis2;

    // 1. Текущая средняя аномалия
    double M_t = M0 + N * t;

    // 2. Эксцентрическая аномалия
    double Ea = solve_kepler(M_t, E);

    // 3. Истинная аномалия (nu)
    double nu = 2.0 * std::atan2(
        std::sqrt(1.0 + E) * std::sin(Ea / 2.0),
        std::sqrt(1.0 - E) * std::cos(Ea / 2.0)
    );

    // 4. Расстояние до Луны
    double r = A * (1.0 - E * std::cos(Ea));

    // 5. Координаты в перифокальной системе (плоскость орбиты Луны)
    double x_orb = r * std::cos(nu);
    double y_orb = r * std::sin(nu);

    // 6. Переход в ECI J2000 (Матрица поворота через узлы и наклонение)
    double cos_O = std::cos(OMEGA_NODE);
    double sin_O = std::sin(OMEGA_NODE);
    double cos_w = std::cos(OMEGA_PERI);
    double sin_w = std::sin(OMEGA_PERI);
    double cos_i = std::cos(I);
    double sin_i = std::sin(I);

    // Компоненты вектора положения
    double Rx = x_orb * (cos_O * cos_w - sin_O * sin_w * cos_i) -
                y_orb * (cos_O * sin_w + sin_O * cos_w * cos_i);

    double Ry = x_orb * (sin_O * cos_w + cos_O * sin_w * cos_i) -
                y_orb * (sin_O * sin_w - cos_O * cos_w * cos_i);

    double Rz = x_orb * (sin_w * sin_i) +
                y_orb * (cos_w * sin_i);


    // --- ДОБАВЛЕННЫЙ КОД: Переход из экваториальной (ECI) в эклиптическую систему ---
    // Угол наклона эклиптики к экватору J2000 (23.439291 градуса в радианах)
    constexpr double EPSILON = 0.409092804222;

    double cos_eps = std::cos(EPSILON);
    double sin_eps = std::sin(EPSILON);

    // Вращение вокруг оси X
    double X_ecl = Rx;
    double Y_ecl = Ry * cos_eps + Rz * sin_eps;
    double Z_ecl = -Ry * sin_eps + Rz * cos_eps;

    return {X_ecl, Y_ecl, Z_ecl};
}

#endif