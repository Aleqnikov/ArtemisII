#include "MFL.h"
#include "System.h"
#include <algorithm>
#include <cmath>

// ─── Константы старта (мыс Канаверал) в ЭКЛИПТИКЕ J2000 ──────────────────────
//
// ECI J2000:
//   up_ECI   = ( 0.0233457021,  0.9956189237,  0.0905424594)
//   east_ECI = (-0.9996462375,  0.0244022613, -0.0105796750)
//
// Поворот ECI → эклиптика (ε = 23.439°):
//   Y_ecl =  Y*cos(ε) + Z*sin(ε)
//   Z_ecl = -Y*sin(ε) + Z*cos(ε)

namespace launch {

    // Локальный базис на старте (эклиптика J2000)
    constexpr double UP_X   =  0.0233457021;
    constexpr double UP_Y   =  0.9494782251;
    constexpr double UP_Z   = -0.3129633815;

    constexpr double EAST_X = -0.9996462375;
    constexpr double EAST_Y =  0.0181802840;
    constexpr double EAST_Z = -0.0194133241;

    // Угловая скорость Земли в эклиптике J2000 [рад/с]
    // ω_ECI = (0, 0, 7.2921150e-5) → эклиптика:
    constexpr double OMEGA_X =  0.0;
    constexpr double OMEGA_Y =  2.9006367525e-05;
    constexpr double OMEGA_Z =  6.6903847127e-05;

    // ── Тайминги фаз ──────────────────────────────────────────────────────────
    constexpr double T_VERTICAL  =  10.0;   // с — вертикальный полёт
    constexpr double T_PITCH_END =  60.0;   // с — конец начального разворота
    constexpr double T_MECO      = 486.0;   // с — отключение Core Stage

    // ── Азимут запуска ────────────────────────────────────────────────────────
    // Рассчитан аналитически из условия совмещения плоскости полёта
    // с плоскостью орбиты Луны.
    //
    // Нормаль к орбите Луны в ECI (элементы JPL):
    //   i = 0.4943709725 рад,  Ω = 6.2136677400 рад
    //   n_ECI = (-sin(i)*sin(Ω),  sin(i)*cos(Ω),  cos(i))
    //         = (-0.43117, -0.18518, 0.88342) (примерно)
    //
    // Перевод n_ECI → эклиптика → условие h·n_ecl=0:
    //   ψ = atan2(-(north·n_ecl), east·n_ecl)
    //
    // Результат: PSI_LAUNCH ≈ 85.47° (азимут на северо-восток,
    // соответствует наклонению парковочной орбиты ~28.5°)
    constexpr double PSI_LAUNCH = 1.070404;

} // namespace launch

// ─── Вспомогательные функции ──────────────────────────────────────────────────

Vector3D MFL::angles_to_n(double theta, double psi,
                            const Vector3D& up,
                            const Vector3D& east,
                            const Vector3D& north)
{
    return up    *  std::sin(theta)
         + east  * (std::cos(theta) * std::sin(psi))
         + north * (std::cos(theta) * std::cos(psi));
}

Control MFL::velocity_to_control(const Vector3D& vel,
                                   const Vector3D& up,
                                   const Vector3D& east,
                                   const Vector3D& north)
{
    double len = vel.mod();
    if (len < 1e-12)
        return Control{ M_PI / 2.0, 0.0 };

    Vector3D nv = vel / len;
    double theta = std::asin(std::clamp(nv.dot(up), -1.0, 1.0));
    double psi   = std::atan2(nv.dot(east), nv.dot(north));
    return Control{ theta, psi };
}

// ─── Основной метод управления ────────────────────────────────────────────────

Control MFL::get_control(System system, double t)
{
    const Vector3D up   (launch::UP_X,   launch::UP_Y,   launch::UP_Z);
    const Vector3D east (launch::EAST_X, launch::EAST_Y, launch::EAST_Z);
    const Vector3D north = east.vecprod(up);

    // ── Фаза 1: вертикальный участок [0, T_VERTICAL) ─────────────────────────
    if (t < launch::T_VERTICAL) {
        return Control{ M_PI / 2.0, launch::PSI_LAUNCH };
    }

    // ── Фаза 2: начальный разворот [T_VERTICAL, T_PITCH_END) ─────────────────
    // Плавный наклон тангажа от 90° до ~87°. Азимут фиксирован = PSI_LAUNCH,
    // что обеспечивает вход в плоскость орбиты Луны с самого старта.
    if (t < launch::T_PITCH_END) {
        double alpha = (t - launch::T_VERTICAL)
                     / (launch::T_PITCH_END - launch::T_VERTICAL);
        double theta = M_PI / 2.0 - (3.0 * M_PI / 180.0) * std::sqrt(alpha);
        return Control{ theta, launch::PSI_LAUNCH };
    }

    // ── Фаза 3: gravity turn [T_PITCH_END, T_MECO) ───────────────────────────
    // Тяга по вектору ОТНОСИТЕЛЬНОЙ скорости (скорость - скорость поверхности).
    // psi естественным образом выходит на PSI_LAUNCH и поддерживает плоскость.
    if (t < launch::T_MECO) {
        const Vector3D& r = system.X.r;
        const Vector3D& v = system.X.v;

        // Скорость поверхности Земли: ω × r (в эклиптике)
        Vector3D v_surface(
            launch::OMEGA_Y * r.data[2] - launch::OMEGA_Z * r.data[1],
            launch::OMEGA_Z * r.data[0] - launch::OMEGA_X * r.data[2],
            launch::OMEGA_X * r.data[1] - launch::OMEGA_Y * r.data[0]
        );

        Vector3D v_rel = v - v_surface;
        return velocity_to_control(v_rel, up, east, north);
    }

    // ── Фаза 4: после MECO — prograde по инерциальной скорости ───────────────
    //
    // Охватывает все остальные этапы:
    //   [T_MECO .. 2942] — коастинг на парковочной орбите
    //   [2942  .. 2962]  — PRM (circularisation, ICPS включён по yaml)
    //   [2962  .. 6812]  — коастинг-2, выход к перигею TLI
    //   [6812  .. 7392]  — TLI (ICPS включён по yaml, горим prograde)
    //   [7392  .. конец] — баллистика (тяги нет)
    //
    // Во всех случаях направление тяги — prograde (вдоль вектора скорости).
    // Когда ICPS выключен (throttle=0 по yaml), тяга = 0 и направление неважно.
    // Когда ICPS включён — prograde даёт максимальный прирост энергии (эффект Оберта).
    {
        const Vector3D& v = system.X.v;
        return velocity_to_control(v, up, east, north);
    }
}

Vector3D MFL::get_n(System system, double t)
{
    const Vector3D up   (launch::UP_X,   launch::UP_Y,   launch::UP_Z);
    const Vector3D east (launch::EAST_X, launch::EAST_Y, launch::EAST_Z);
    const Vector3D north = up.vecprod(east);

    Control ctrl = get_control(system, t);
    return angles_to_n(ctrl.theta, ctrl.psi, up, east, north);
}