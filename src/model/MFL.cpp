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

// Целевая точка от профессора (JPL Horizons).
// ВАЖНО: переведено из км и км/с в метры и м/с!
const Vector3D TARGET_POS(-24442520.46, -14322896.36, -1284178.29);
const Vector3D TARGET_VEL(-1844090.0, -3873750.0, -336390.0);

MFL::MFL(const std::vector<std::pair<double, Vector3D>>& mfldata) : data(mfldata) {
    // Высчитываем нормаль к плоскости орбиты Луны через векторное произведение (h = r x v)
    Vector3D h = TARGET_POS.vecprod(TARGET_VEL);
    moon_plane_normal = h.normalize();
}



Control MFL::get_control(const System& system, const Vector& X, double t)
{
    // Оставляем заглушку. Теперь ракетой управляет напрямую get_n
    return Control{ 0 , 0 };
}


Vector3D MFL::get_n(const System& system, const Vector& X, double t)
{


	if (t >= 10859) {

		Vector3D diff = data.front().second - X.v;
	}
    // Защита от деления на ноль, если скорость вдруг нулевая
    Vector3D v_dir = (X.v.mod() > 1e-5) ? X.v.normalize() : X.r.normalize();

    // =========================================================================
    // --- ПОЗДНЯЯ СТАДИЯ: ИНТЕРПОЛЯЦИЯ (t >= 10858 с) ---
    // =========================================================================
    if (t >= 90000.0) {
       return system.X.v.normalize();
    }

    const double R_E        = 6371000.0;
    const double TARGET_ALT = 185000.0;          // 185 км
    const double R_TARGET   = R_E + TARGET_ALT;  // 6 556 000 м
    const double mu         = 3.986004418e14;

    double r_curr = X.r.mod();
    double h_curr = r_curr - R_E;

    // =========================================================================
    // --- ФАЗА 4: МАНЁВР ПОДЪЁМА АПОЦЕНТРА ДО 70 377 КМ (От 5250 до 10858 с) ---
    // =========================================================================
    if (t >= 5250.0) {
       const double TARGET_APOGEE_ALT = 70377000.0; // 70377 км в метрах

       // 1. Считаем модуль углового момента h = |r x v|
       Vector3D h_vec = X.r.vecprod(X.v);
       double h_mod = h_vec.mod();

       // 2. Считаем удельную энергию
       double v_sq = X.v.dot(X.v);
       double E_curr = (v_sq / 2.0) - (mu / r_curr);

       // Проверяем, эллиптическая ли орбита (E < 0)
       if (E_curr < 0.0) {
          // Большая полуось
          double a = -mu / (2.0 * E_curr);

          // Детерминант для апсидальных радиусов (под коренное выражение)
          double discriminant = a * a + (h_mod * h_mod) / (2.0 * E_curr);

          if (discriminant >= 0.0) {
             // Радиус апоцентра (больший корень)
             double r_apogee_curr = a + std::sqrt(discriminant);
             double h_apogee_curr = r_apogee_curr - R_E;

             // Точная проверка достижения целевой высоты апоцентра
             if (h_apogee_curr >= TARGET_APOGEE_ALT) {
                // ВАЖНО: Выставляем флаг симулятору, чтобы он выключил подачу топлива!
                return v_dir;
             }
          }
       }

       // Жжём строго по вектору скорости (Prograde)
       return v_dir;
    }

    // --- Фаза 1: вертикальный старт (0–130 с) ---
    Vector3D up_dir = X.r.normalize();
    if (t < 130) {
        return up_dir;
    }

    // Вычисляем компоненты скорости для Фаз 2 и 3
    double v_radial = X.v.dot(up_dir);
    Vector3D v_horiz_vec = X.v - up_dir * v_radial;
    double v_horiz = v_horiz_vec.mod();

    // Проверка на достижение опорной круговой орбиты 185х185 км (Конец Фазы 3)
    double v_orb = std::sqrt(mu / R_TARGET); // ~7793.15 м/с
    if (t < 5612.0 && h_curr >= TARGET_ALT - 1000.0 && v_horiz >= v_orb - 5.0 && std::abs(v_radial) < 5.0) {
        return v_dir; // Возвращаем вектор скорости вместо Vector3D(0,0,0) во избежание NaN
    }

    if (t < 450) {
       // --- Фаза 2 и 3: Активное выведение в плоскость и накат круговой орбиты ---
       const double A = -636638.01822281;
       const double B = -21603260.16260338;
       const double C =  252281532.75012207;
       Vector3D plane_normal = Vector3D(A, B, C).normalize();

       // Коррекция плоскости (убираем боковое смещение и боковую скорость)
       double dist_to_plane = X.r.dot(plane_normal);
       double v_in_normal   = X.v.dot(plane_normal);

       double plane_correction = -0.00005 * dist_to_plane - 0.5 * v_in_normal;
       plane_correction = std::clamp(plane_correction, -0.25, 0.25);

       // Строим идеальный вектор движения вперед внутри плоскости
       Vector3D prograde = v_horiz_vec;
       if (prograde.mod() < 1e-3) {
          prograde = plane_normal.vecprod(up_dir);
       }
       prograde = (prograde - plane_normal * prograde.dot(plane_normal)).normalize();

       // Управление Тангажем (Pitch) через удержание вертикальной скорости.
       double h_err = TARGET_ALT - h_curr;

       // Желаемая вертикальная скорость (затухающая по мере приближения к 185 км)
       double target_v_radial = 0.0;
       if (h_err > 0) {
          double net_acceleration = (mu / (r_curr * r_curr)) - (v_horiz * v_horiz / r_curr);
          net_acceleration = std::max(0.0, net_acceleration); // Защита от NaN

          target_v_radial = std::sqrt(2.0 * net_acceleration * h_err) * 0.5;
          if (target_v_radial > 150.0) target_v_radial = 150.0;
       } else {
          target_v_radial = 0.1 * h_err;
       }

       double v_radial_err = target_v_radial - v_radial;

       double pitch_cmd = 0.05 * v_radial_err;
       pitch_cmd = std::clamp(pitch_cmd, -0.3, 0.8);

       // Собираем финальный вектор тяги
       Vector3D n = prograde + (up_dir * pitch_cmd) + (plane_normal * plane_correction);

       return n.normalize();
    }

    // Если мы вышли за 450 секунд, но до 5612 секунд (ожидание точки включения)
    // Просто летим по инерции, сонаправленно вектору скорости
    return v_dir;
}

