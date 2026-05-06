#ifndef ARTEMISII_ENGINE_H
#define ARTEMISII_ENGINE_H


/**
 * Условно логика такая - "двигатель за какой то определенный промежуток времени расходует
 * какое то количество топлива, затем это сообщяется выше. То есть двигатель есть компонент ракеты
 * при этом он не то чтобы неразрывно связан со ступенью, он как бы существует в вакууме.
 */

struct Engine
{
    double I_sp;
    double T_max;

    /**
     * Тяга двигателя в каждый момент времени. Пока непонятно как определить корректно,
     * но это как бы временная вернсия.
     *                    (t, f(t))
     */
    std::vector<std::pair<double, double>> thottle;

    double get_thottle();
    double get_mu();

    Engine::Engine(double I_sp, double T_max, std::string thottle_str)
    : I_sp(I_sp), T_max(T_max) {};

};


#endif //ARTEMISII_ENGINE_H