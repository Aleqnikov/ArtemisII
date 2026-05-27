#include "MFL.h"
#include "System.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace launch {
    constexpr double UP_X   =  0.0233457021;
    constexpr double UP_Y   =  0.9494782251;
    constexpr double UP_Z   = -0.3129633815;

    constexpr double EAST_X = -0.9996462375;
    constexpr double EAST_Y =  0.0181802840;
    constexpr double EAST_Z = -0.0194133241;
}

const Vector3D TARGET_POS(-24442520.46, -14322896.36, -1284178.29);
const Vector3D TARGET_VEL(-1844090.0, -3873750.0, -336390.0);

MFL::MFL(void)  {
    Vector3D h = TARGET_POS.vecprod(TARGET_VEL);
    moon_plane_normal = h.normalize();
}

enum {
    START,
    FIRST,
    TLI,
    BALISTIC,
};

double getOrientedAngle(const Vector3D& r1, const Vector3D& r2, const Vector3D& h) {
    double dot = r1.dot(r2);
    double mag1 = r1.mod();
    double mag2 = r2.mod();

    double cos_theta = dot / (mag1 * mag2);
    double angle = acos(std::max(-1.0, std::min(1.0, cos_theta)));

    Vector3D w = r1.vecprod(r2);
    if (w.dot(h) < 0) {
        angle = -angle;
    }

    return angle;
}

// Вспомогательная функция: считает высоту апоцентра по вектору состояния.
// Возвращает -1.0 если орбита не эллиптическая.
static double calc_apogee_alt(const Vector& X, double mu, double R_E) {
    double r = X.r.mod();
    Vector3D h_vec = X.r.vecprod(X.v);
    double h_mod = h_vec.mod();
    double v_sq = X.v.dot(X.v);
    double E = (v_sq / 2.0) - (mu / r);

    if (E >= 0.0) return -1.0; // Гиперболическая — апогея нет

    double a = -mu / (2.0 * E);
    double disc = a * a + (h_mod * h_mod) / (2.0 * E);
    if (disc < 0.0) return -1.0;

    return (a + std::sqrt(disc)) - R_E;
}

Vector3D MFL::get_n(System& system, const Vector& X, double t)
{
    const double R_E        = 6371000.0;
    const double TARGET_ALT = 185000.0;
    const double R_TARGET   = R_E + TARGET_ALT;
    const double mu         = 3.986004418e14;

    double r_curr = X.r.mod();
    double h_curr = r_curr - R_E;
    Vector3D v_dir = (X.v.mod() > 1e-5) ? X.v.normalize() : X.r.normalize();

    // ── МОД 0: Выведение на опорную орбиту 185×185 км ───────────────────────
    if (system.mod == 0) {
        Vector3D up_dir = X.r.normalize();

        // ── 0а: Вертикальный подъём ──────────────────────────────────────────────
        if (t < 130) {
            return up_dir;
        }

        double v_radial = X.v.dot(up_dir);
        Vector3D v_horiz_vec = X.v - up_dir * v_radial;

        // ── 0б: Gravity turn до MECO (~437 сек) ─────────────────────────────────
        if (t < 440) {
           const double A = -636638.01822281;
           const double B = -21603260.16260338;
           const double C =  252281532.75012207;
           Vector3D plane_normal = Vector3D(A, B, C).normalize();

           double dist_to_plane = X.r.dot(plane_normal);
           double v_in_normal   = X.v.dot(plane_normal);

           double plane_correction = -0.00005 * dist_to_plane - 0.5 * v_in_normal;
           plane_correction = std::clamp(plane_correction, -0.25, 0.25);

           Vector3D prograde = v_horiz_vec;
           if (prograde.mod() < 1e-3) {
              prograde = plane_normal.vecprod(up_dir);
           }
           prograde = (prograde - plane_normal * prograde.dot(plane_normal)).normalize();

           double target_v_radial;
           if (t < 250.0) {
              double frac = (t - 130.0) / (250.0 - 130.0);
              target_v_radial = 300.0 * (1.0 - frac);
           } else {
              double h_err = TARGET_ALT - h_curr;
              target_v_radial = std::clamp(0.05 * h_err, -80.0, 20.0);
           }

           double v_radial_err = target_v_radial - v_radial;
           double pitch_cmd = std::clamp(0.04 * v_radial_err, -0.5, 0.6);

           Vector3D n = prograde + (up_dir * pitch_cmd) + (plane_normal * plane_correction);
           return n.normalize();
        }

        // ── 0в: Баллистика после MECO, ждём апогея ──────────────────────────────
        if (!system.prm_started) {
            if (h_curr > 150000.0 && v_radial < 30.0 && v_radial > -30.0) {
                system.prm_started = true;
                system.rocket.update_engine_programs(t, 1); // включаем ICPS
            }
            return v_dir;
        }

        // ── 0г: ICPS горит — поднимаем перигей до 185 км ────────────────────────
        const double TARGET_PERIGEE_ALT = 185000.0;

        // Вычисляем текущий перигей КАЖДЫЙ кадр, а не только когда target == 0
        double r      = X.r.mod();
        Vector3D hv   = X.r.vecprod(X.v);
        double h_mod  = hv.mod();
        double v_sq   = X.v.dot(X.v);
        double E      = v_sq / 2.0 - mu / r;

        double h_pe = -1.0;
        bool valid_orbit = false;

        if (E < 0.0) {
            double a    = -mu / (2.0 * E);
            double disc = a * a + (h_mod * h_mod) / (2.0 * E);
            disc = std::max(0.0, disc); // Защита от NaN при идеально круговой орбите

            h_pe = (a - std::sqrt(disc)) - R_E;
            valid_orbit = true;

            if (system.target_perigee == 0.0 && h_pe < TARGET_PERIGEE_ALT) {
                system.target_perigee = TARGET_PERIGEE_ALT;
            }
        }

        // РУЧНОЙ ПЕРЕХВАТ: Если step_control не умеет отключать двигатель по перигею, делаем это сами
        if (system.target_perigee > 0.0 && valid_orbit && h_pe >= TARGET_PERIGEE_ALT) {
            system.perigee_target_reached = true;
            system.rocket.update_engine_programs(t, 0); // Принудительно глушим двигатель (0 = выкл)
        }

        // Когда перигей поднят — переходим в мод 1
        if (t > 6000) {
            system.mod = 1;
            system.prm_started            = false;
            system.perigee_target_reached = false;
            system.target_perigee         = 0.0;
            return v_dir;
        }

        return v_dir; // prograde во время PRM
    }

    // ── МОД 1: PRM — разгон до апогея 70 377 км ─────────────────────────────
    if (system.mod == 1) {
        if (system.apogee_target_reached && t  > 8000) {
            system.mod = 2;
            system.apogee_target_reached = false;
            system.target_apogee = 0.0;
            return v_dir;
        }

        Vector3D start_point(4.24525e+06, 5.33487e+06, 492217);
        Vector3D v1(-6843.68, 4338.22, 377.987);
        Vector3D h = start_point.vecprod(v1);

        double angle = getOrientedAngle(start_point, system.X.r, h);
        if (angle >= 0 && angle < 0.1) {
            system.rocket.update_engine_programs(t, 1.05);
        }

        const double TARGET_APOGEE_ALT = 70366000.0;
        if (system.target_apogee == 0.0) {
            double h_ap = calc_apogee_alt(X, mu, R_E);
            if (h_ap > 0.0 && h_ap < TARGET_APOGEE_ALT) {
                system.target_apogee = TARGET_APOGEE_ALT;
            }
        }
    }

    // ── МОД 2: TLI — разгон до апогея 450 377 км ────────────────────────────
    if (system.mod == 2) {
        if (system.apogee_target_reached) {
            system.mod = 3;
            system.apogee_target_reached = false;
            system.target_apogee = 0.0;
            return v_dir;
        }

        Vector3D start_point(510787.0098, 6553954.4434, 558862.7756 );
        Vector3D v1(-10527.5579, 1556.2518, 37.1651);
        Vector3D h = start_point.vecprod(v1);

        double angle = getOrientedAngle(start_point, system.X.r, h);
        if (angle >= 0 && angle < 0.01) {
            system.rocket.update_engine_programs(t, 0.975);
        }

        const double TARGET_APOGEE_ALT = 457529000.0;
        if (system.target_apogee == 0.0) {
            double h_ap = calc_apogee_alt(X, mu, R_E);
            if (h_ap > 0.0 && h_ap < TARGET_APOGEE_ALT) {
                system.target_apogee = TARGET_APOGEE_ALT;
            }
        }
    }

	if (system.mod == 3) {
		if (t >= 315007) {
			system.rocket.update_engine_programs(t, 0.5);
		}

		if  (t > 315007 + 5.5) {
			system.rocket.update_engine_programs(t, 0);
			system.mod = 4;
		}
	}

    return v_dir;
}