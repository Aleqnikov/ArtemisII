#ifndef ARTEMISII_TELEMETRY_H
#define ARTEMISII_TELEMETRY_H

#include "core/physics.h"
#include <fstream>

// Класс для вывода телеметрии в потоковый файл.
class Telemetry
{
private:
    System& system;

    std::ofstream outStream; // Вывод программы

public:
    // Создает ссылку, открывает файловый дескриптор.
    Telemetry(System& system, std::string output);

	~Telemetry();
    // Получает информацию о системе, выводит необоходимые поля в поток.
    // Получает информацию из физики о положении Луны и Земли и их тоже выводит.
    void log();
};


#endif //ARTEMISII_TELEMETRY_H