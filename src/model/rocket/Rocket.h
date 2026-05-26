#ifndef ARTEMISII_ROCKET_H
#define ARTEMISII_ROCKET_H

#include "model/rocket/stage.h"
#include "core/Vector3d.h"
#include "core/Vector.h"
#include <vector>
#include <map>

struct ThrustResult {
	double F_total;
	double beta_total;
};

struct StageNode {
	int id;
	std::vector<int> successors; // Ступени, которые зависят от этой
	int in_degree = 0;           // Сколько ступеней должны отделиться перед этой
};

struct Rocket {
	Vector3D r;
	Vector3D v;

	std::vector<Stage> stages;
	std::map<int, StageNode> stage_graph; // Наш граф зависимостей

	double getMass() const;
	void init_start(Vector X);

	// Переименовали для ясности
	void build_graph(const std::map<int, std::vector<int>>& flight_plan);

	std::vector<Stage*>       get_active_stages();
	std::vector<const Stage*> get_active_stages() const;

	ThrustResult compute_thrust(double t) const;
	void separate_stage(int id);
	void check_and_separate();
	void update_engine_programs(double t, double power);

	Rocket() = default;
};

#endif