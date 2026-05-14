#include "MFL.h"

#include "System.h"
#include <algorithm> // Обязательно для std::lower_bound


Vector3D MFL::get_n(System system, double t)
{
    if (mfl.empty()) return Vector3D(0, 0, 0);


    if (t < 80)
    {
        Vector3D v = system.X.r;
        double len = v.mod();
        return (len > 1e-12) ? v / len : v;
    }

    // --- 1. Обработка границ ---
    if (t <= mfl.front().first) {
        Vector3D v = system.X.v;
        double len = v.mod();
        return (len > 1e-12) ? v / len : v;
    }


    if (t >= mfl.back().first) {
        Vector3D v = mfl.back().second;
        double len = v.mod();
        return (len > 1e-12) ? v / len : v;
    }

    // --- 2. Бинарный поиск (O(log N)) ---
    // Ищем первую точку, время которой НЕ меньше t
    auto it = std::lower_bound(mfl.begin(), mfl.end(), t,
        [](const std::pair<double, Vector3D>& a, double val) {
            return a.first < val;
        });

    // Левая точка — это элемент перед найденным
    auto it_prev = std::prev(it);

    // --- 3. Линейная интерполяция ---
    double t0 = it_prev->first;
    double t1 = it->first;
    double alpha = (t - t0) / (t1 - t0);

    const Vector3D& n0 = it_prev->second;
    const Vector3D& n1 = it->second;

    // Считаем промежуточный вектор
    Vector3D interp = n0 * (1.0 - alpha) + n1 * alpha;

    // --- 4. Ручная нормировка ---
    double len = interp.mod();
    if (len > 1e-14) { // Проверка на нулевой вектор, чтобы не было деления на 0
        return interp / len;
    }

    return interp;
}