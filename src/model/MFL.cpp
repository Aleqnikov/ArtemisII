#include "MFL.h"
#include "System.h"
#include <algorithm>
#include <cmath>

namespace launch {

    constexpr double UP_X   =  0.0233457021;
    constexpr double UP_Y   =  0.9956189237;
    constexpr double UP_Z   =  0.0905424594;

    constexpr double EAST_X = -0.9996462375;
    constexpr double EAST_Y =  0.0244022613;
    constexpr double EAST_Z = -0.0105796750;


    constexpr double OMEGA_X =  0.0;
    constexpr double OMEGA_Y =  2.9006367525e-05;
    constexpr double OMEGA_Z =  6.6903847127e-05;
}
Vector3D MFL::get_n(System system, double t) {
    if (mfl.empty()) return Vector3D(0, 0, 0);


    if (t >= mfl.front().first) {
        if (t >= mfl.back().first) {
            Vector3D v = mfl.back().second;
            double len = v.mod();
            return (len > 1e-12) ? v / len : v;
        }
        auto it = std::lower_bound(mfl.begin(), mfl.end(), t,
            [](const std::pair<double, Vector3D>& a, double val) {
                return a.first < val;
            });
        auto it_prev = std::prev(it);
        double t0 = it_prev->first, t1 = it->first;
        double alpha = (t - t0) / (t1 - t0);
        Vector3D interp = it_prev->second * (1.0 - alpha) + it->second * alpha;
        double len = interp.mod();
        return (len > 1e-14) ? interp / len : interp;
    }

    const double T_VERTICAL  = 10.0;
    const double T_PITCH_END = 60.0;
    const double T_MECO      = 486.0;

    Vector3D up  (launch::UP_X,   launch::UP_Y,   launch::UP_Z);
    Vector3D east(launch::EAST_X, launch::EAST_Y, launch::EAST_Z);


    if (t < T_VERTICAL) {
        return up;
    }


    if (t < T_PITCH_END) {
        double alpha = (t - T_VERTICAL) / (T_PITCH_END - T_VERTICAL);
        double pitch = (3.0 * M_PI / 180.0) * std::sqrt(alpha);
        Vector3D n = up * std::cos(pitch) + east * std::sin(pitch);
        return n;
    }


    if (t < T_MECO) {
        Vector3D r = system.X.r;
        Vector3D v = system.X.v;


        double surf_x = launch::OMEGA_Y * r.data[2] - launch::OMEGA_Z * r.data[1];
        double surf_y = launch::OMEGA_Z * r.data[0] - launch::OMEGA_X * r.data[2];
        double surf_z = launch::OMEGA_X * r.data[1] - launch::OMEGA_Y * r.data[0];

        Vector3D v_rel(
            v.data[0] - surf_x,
            v.data[1] - surf_y,
            v.data[2] - surf_z
        );

        double len = v_rel.mod();
        return (len > 1e-12) ? v_rel / len : up;
    }


    {
        Vector3D v = system.X.v;
        double len = v.mod();
        return (len > 1e-12) ? v / len : up;
    }
}