#include <cmath>

// ─── Константы вращения Земли ─────────────────────────────────────────────
// Epoch: 2026-Apr-01 22:35:00 UTC — реальный старт Artemis II
// JD₀ = 2461132.4410

namespace earth_rotation {

    // GMST на момент старта [рад]
    // По формуле IAU 1982: GMST = 169.0466° для JD 2461132.441
    // GAST = GMST + EqEq = 169.0482°  (поправка нутации ~5.65")
    constexpr double THETA0 = 169.0482 * M_PI / 180.0;  // [рад]  ← было 197.34°

    // Угловая скорость вращения Земли (звёздные сутки)
    constexpr double OMEGA_EARTH = 7.2921150e-5;  // [рад/с]  ← верно

    // Нутация (уравнение экватора): главный член Δψ · cos(ε)
    // Δψ ≈ −17.20" · sin(Ω_moon) — амплитуда
    constexpr double DPSI_AMP = -17.20 / 3600.0 * M_PI / 180.0; // [рад]  ← верно

    // Долгота восходящего узла Луны на момент старта
    // Ω(t=0) = 125.04455° - 0.05295377° · (JD - 2451545) = 337.35°
    constexpr double OMEGA_NODE = 337.35 * M_PI / 180.0;  // [рад]  ← было 11°

    // Скорость изменения Ω [рад/с]  ← верно
    constexpr double NODE_RATE = -0.0529539 * M_PI / 180.0 / 86400.0;

    // cos(ε₀),  ε₀ = 23.439° — наклон эклиптики  ← верно
    constexpr double COS_EPS = 0.91748;

} // namespace earth_rotation


/// Текущий звёздный угол Гринвича (GAST) в системе ECI J2000.
/// @param t  Время от момента старта Artemis II [секунды]
/// @return   θ(t) ∈ [0, 2π) [рад]
double Earth_Angle(double t) {
    using namespace earth_rotation;

    // 1. GMST: линейный рост
    const double gmst = THETA0 + OMEGA_EARTH * t;

    // 2. Уравнение экватора (нутация): GAST = GMST + Δψ·cos(ε)
    const double omega_node_t = OMEGA_NODE + NODE_RATE * t;
    const double equation_of_equinoxes = DPSI_AMP * std::cos(omega_node_t)
                                       * COS_EPS;

    const double gast = gmst + equation_of_equinoxes;

    // 3. Нормировка в [0, 2π)
    const double two_pi = 2.0 * M_PI;
    return gast - two_pi * std::floor(gast / two_pi);
}