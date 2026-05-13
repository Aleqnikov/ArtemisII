#include "Rocket.h"
#include <algorithm>
#include <set>

void Rocket::init_start(Vector X) {
    r = X.r;
    v = X.v;
}

void Rocket::build_graph(const std::map<int, std::vector<int>>& flight_plan) {
    stage_graph.clear();

    // 1. Создаем узлы для всех упомянутых ступеней
    for (auto const& [phase, ids] : flight_plan) {
        for (int id : ids) {
            if (stage_graph.find(id) == stage_graph.end()) {
                stage_graph[id] = StageNode{id, {}, 0};
            }
        }
    }

    // 2. Строим связи на основе переходов фаз
    auto it = flight_plan.begin();
    auto next = std::next(it);

    for (; next != flight_plan.end(); ++it, ++next) {
        std::set<int> current_active(it->second.begin(), it->second.end());
        std::set<int> next_active(next->second.begin(), next->second.end());

        std::vector<int> disappeared, appeared;

        // Ступени, которые исчезли (отстрелились)
        for (int id : current_active)
            if (!next_active.count(id)) disappeared.push_back(id);

        // Ступени, которые появились (зажглись)
        for (int id : next_active)
            if (!current_active.count(id)) appeared.push_back(id);

        // Логика: всё, что появилось, зависит от того, что исчезло
        // В твоем примере: исчезла 2 -> появилась 3. Значит 2 -> 3.
        for (int from : disappeared) {
            for (int to : appeared) {
                stage_graph[from].successors.push_back(to);
                stage_graph[to].in_degree++;
            }
        }
    }
}

// Активные ступени — те, у которых нет невыполненных зависимостей (in_degree == 0)
std::vector<Stage*> Rocket::get_active_stages() {
    std::vector<Stage*> result;
    for (auto& [id, node] : stage_graph) {
        if (node.in_degree == 0) {
            result.push_back(&stages[id]);
        }
    }
    return result;
}

std::vector<const Stage*> Rocket::get_active_stages() const {
    std::vector<const Stage*> result;
    for (auto const& [id, node] : stage_graph) {
        if (node.in_degree == 0) {
            result.push_back(&stages[id]);
        }
    }
    return result;
}

void Rocket::separate_stage(int id) {
    auto it = stage_graph.find(id);
    if (it == stage_graph.end()) return;

    // Уменьшаем in_degree у всех последователей
    for (int next_id : it->second.successors) {
        if (stage_graph.count(next_id)) {
            stage_graph[next_id].in_degree--;
        }
    }

    // Удаляем саму ступень из системы
    stage_graph.erase(it);
}

double Rocket::getMass() const {
    double m = 0.0;
    // Считаем массу всех ступеней, которые еще в графе (активные + будущие)
    for (auto const& [id, node] : stage_graph) {
        m += stages[id].weight + stages[id].fuel_w;
    }
    return m;
}

ThrustResult Rocket::compute_thrust(double t) const {
    double F = 0.0, beta = 0.0;
    for (const Stage* st : get_active_stages()) {
        if (st->fuel_w <= 0.0) continue;
        F    += st->engine.get_thrust(t);
        beta += st->engine.get_beta(t);
    }
    return {F, beta};
}

void Rocket::check_and_separate() {
    std::vector<int> to_remove;
    // Проверяем только активные (те, что работают прямо сейчас)
    for (auto const& [id, node] : stage_graph) {
        if (node.in_degree == 0 && stages[id].fuel_w <= 0.0) {
            to_remove.push_back(id);
        }
    }

    for (int id : to_remove) {
        separate_stage(id);
    }
}