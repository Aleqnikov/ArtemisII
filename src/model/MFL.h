#ifndef ARTEMISII_MFL_H
#define ARTEMISII_MFL_H

#include <vector>
#include <utility>
#include "../core/Vector3d.h"
#include "clp/Control.h"

// Объявляем классы заранее
class System;
struct Vector;

// Тот самый "пульт управления", где можно подбирать параметры траектории
struct ControlParameters {
	double launch_azimuth = 0.505;
	double t_kick         = 15.0;
	double pitch_rate     = 0.002;
	double pitch_final    = 0.15;  // ~8.6° — минимальный угол к горизонту
	double k_plane        = 0.005;
};

class MFL {
private:
	std::vector<std::pair<double, Control>> data;

	ControlParameters params;  // Экземпляр наших параметров
	Vector3D moon_plane_normal; // Вектор нормали к плоскости орбиты Луны

public:
	// Конструктор теперь будет сам высчитывать плоскость при создании
	MFL(const std::vector<std::pair<double, Control>>& mfldata);

	Vector3D get_n(const System& system, const Vector& X, double t);
	Control get_control(const System& system, const Vector& X, double t);

	static Vector3D angles_to_n(double theta, double psi,
						 const Vector3D& up,
						 const Vector3D& east,
						 const Vector3D& north);

	static Control velocity_to_control(const Vector3D& vel,
							   const Vector3D& up,
							   const Vector3D& east,
							   const Vector3D& north);
};

#endif // ARTEMISII_MFL_H