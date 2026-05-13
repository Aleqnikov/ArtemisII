#include "MFL.h"

#include "System.h"

Vector3D MFL::get_n(System system, double t)
{
    // получить временной промежуток, и отинтерполировать и выдать вектор.



    return Vector3D(-0.3953, 0.7837, 0.4791);

    if (t >= mfl.back().first)
        return mfl.back().second;

    for (size_t i = 1; i < mfl.size(); ++i) {
        if (t <= mfl[i].first) {
            double t0 = mfl[i-1].first;
            double t1 = mfl[i].first;
            double alpha = (t - t0) / (t1 - t0);

            const Vector3D& n0 = mfl[i-1].second;
            const Vector3D& n1 = mfl[i].second;

            // Линейная интерполяция + нормировка
            Vector3D interp = n0 * (1.0 - alpha) + n1 * alpha;
            double len = interp.mod();
            return (len > 1e-12) ? interp / len : interp;
        }
    }

    return mfl.back().second;
}
