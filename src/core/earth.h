#include <cmath>

// ─── Константы вращения Земли ─────────────────────────────────────────────
// Epoch: 2025-Apr-07 00:00:00.0 UTC (старт Artemis II)
// Θ₀ — звёздный угол Гринвича (GAST) на момент запуска
// Источник: IERS / USNO формула для θ_GMST

namespace earth_rotation {

// Звёздный угол Гринвича на эпоху Artemis II [рад]
// JD₀ = 2460771.5 (2025-Apr-07 00:00 UTC)
// GMST₀ = 100.4606 + 36000.7700·T + ... → ≈ 197.34° для данной эпохи
constexpr double THETA0 = 197.34 * M_PI / 180.0;  // [рад]

// Угловая скорость вращения Земли (звёздные сутки)
// ω⊕ = 2π / 86164.0905 с  (сидерические сутки)
constexpr double OMEGA_EARTH = 7.2921150e-5;  // [рад/с]

// Поправка на нутацию ψ (упрощённо, главный член)
// Δψ ≈ −17.2" · sin(Ω_moon),  Ω_moon ≈ 11° на J2025.3
// cos(ε) ≈ 0.9174  (наклон эклиптики ε ≈ 23.44°)
constexpr double DPSI_AMP   = -17.2 / 3600.0 * M_PI / 180.0; // [рад]
constexpr double OMEGA_NODE =  11.0 * M_PI / 180.0;           // Ω узла [рад]
constexpr double NODE_RATE  = -0.0529539 * M_PI / 180.0       // [рад/сут]
                              / 86400.0;                       // → [рад/с]
constexpr double COS_EPS    = 0.91748;  // cos(23.44°)

} // namespace earth_rotation

// ─── Функция ─────────────────────────────────────────────────────────────────
/// Возвращает текущий звёздный угол Гринвича (GAST) — угол поворота
/// Земли вокруг своей оси в системе ECI J2000.
///
/// @param t  Время от момента запуска Artemis II [секунды]
/// @return   Угол θ(t) ∈ [0, 2π) [рад]
double Earth_Angle(double t) {
    using namespace earth_rotation;

    // 1. GMST: линейный рост со скоростью ω⊕
    const double gmst = THETA0 + OMEGA_EARTH * t;

    // 2. Поправка на нутацию (уравнение экватора)
    //    GAST = GMST + Δψ · cos(ε)
    //    Долгота узла Луны медленно меняется: Ω(t) = Ω₀ + Ω̇·t
    const double omega_node_t = OMEGA_NODE + NODE_RATE * t;
    const double equation_of_equinoxes = DPSI_AMP * std::cos(omega_node_t)
                                        * COS_EPS;

    const double gast = gmst + equation_of_equinoxes;

    // 3. Нормировка в [0, 2π)
    const double two_pi = 2.0 * M_PI;
    return gast - two_pi * std::floor(gast / two_pi);
}