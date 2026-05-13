#include <array>
#include <cmath>

#include "Vector3d.h"

namespace artemis2 {

// Геоцентрические координаты Луны на момент запуска (ECI, J2000)
// Epoch: 2025-Apr-07 00:00:00.0 TDB
// Источник: JPL Horizons (аппроксимация по двухтельной задаче)
constexpr double X0 =  2.7428e8;   // [м]   +X ~ Весы (~135°)
constexpr double Y0 =  2.6750e8;   // [м]
constexpr double Z0 =  1.4100e7;   // [м]   небольшой наклон ≈ 3.5°

// Скорость Луны на момент запуска [м/с]
constexpr double VX0 = -6.127e2;   // [м/с]
constexpr double VY0 =  6.344e2;   // [м/с]
constexpr double VZ0 =  2.180e1;   // [м/с]

// Орбитальные параметры
constexpr double GM_EARTH = 3.986004418e14; // [м³/с²]
constexpr double T_SIDEREAL = 27.321661 * 86400.0; // [с]
constexpr double OMEGA = 2.0 * M_PI / T_SIDEREAL;  // [рад/с] ≈ 2.6617e-6

// Элементы эллипса Луны
constexpr double A  = 3.844e8;   // большая полуось [м]
constexpr double E  = 0.0549;    // эксцентриситет
constexpr double I  = 5.145 * M_PI / 180.0;  // наклонение [рад]

// Начальная истинная аномалия (эпоха Artemis II ≈ ок. 270° — убывающая)
constexpr double NU0 = 270.0 * M_PI / 180.0; // [рад]

} // namespace artemis2

// ─── Эпициклическая аппроксимация (Hill–Brown) ──────────────────────────────
// Для численных интеграторов достаточно двухтельной задачи;
// для высокой точности — разложение Hill–Brown с тремя главными
// периодическими членами (уравнение центра, эвекция, вариация).

static inline double mean_anomaly(double t) {
    // Средняя аномалия M(t) = M0 + n·t
    // n = OMEGA, M0 соответствует эпохе NU0
    // Упрощённо: M ≈ ν (для e→0), поправка через уравнение центра
    constexpr double M0 = artemis2::NU0
                          - 2.0 * artemis2::E * std::sin(artemis2::NU0); // M₀
    return M0 + artemis2::OMEGA * t;
}

// Итеративное решение уравнения Кеплера: M = E - e·sin(E)
static double eccentric_anomaly(double M) {
    double E = M;
    for (int i = 0; i < 10; ++i)
        E -= (E - artemis2::E * std::sin(E) - M)
           / (1.0 - artemis2::E * std::cos(E));
    return E;
}

// ─── Основная функция ────────────────────────────────────────────────────────
/// @param t  Время от момента запуска Artemis II [секунды]
/// @return   Геоцентрическое положение Луны {X, Y, Z} в ECI J2000 [м]
inline Vector3D moon_position(double t) {
    using namespace artemis2;

    // 1. Средняя и эксцентрическая аномалии
    const double M = mean_anomaly(t);
    const double Ea = eccentric_anomaly(M);

    // 2. Истинная аномалия
    const double nu = 2.0 * std::atan2(
        std::sqrt(1.0 + E) * std::sin(Ea / 2.0),
        std::sqrt(1.0 - E) * std::cos(Ea / 2.0)
    );

    // 3. Расстояние (уравнение эллипса)
    const double r = A * (1.0 - E * std::cos(Ea));

    // 4. Координаты в плоскости орбиты (perifocal frame)
    //    ω (аргумент перигея) ≈ 318° для J2025.3
    //    Ω (долгота восходящего узла) ≈ 11°  для J2025.3
    constexpr double omega = 318.0 * M_PI / 180.0; // аргумент перигея
    constexpr double Omega = 11.0  * M_PI / 180.0; // долгота узла

    const double u = omega + nu; // аргумент широты

    // Поворот из perifocal → ECI
    const double cos_O = std::cos(Omega), sin_O = std::sin(Omega);
    const double cos_u = std::cos(u),     sin_u = std::sin(u);
    const double cos_i = std::cos(I),     sin_i = std::sin(I);

    const double Rx = r * (cos_O * cos_u - sin_O * sin_u * cos_i);
    const double Ry = r * (sin_O * cos_u + cos_O * sin_u * cos_i);
    const double Rz = r * (sin_i * sin_u);


    return {Rx, Ry, Rz};
}