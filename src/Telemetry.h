#ifndef ARTEMISII_TELEMETRY_H
#define ARTEMISII_TELEMETRY_H

#include "core/physics.h"

// Класс для вывода телеметрии в потоковый файл.
class Telemetry
{
private:
    System& system;

    std::iostrem; // Поток куда выводим.

public:
    // Создает ссылку, открывает файловый дескриптор.
    Telemetry(System& system, std::string output);

    // Получает информацию о системе, выводит необоходимые поля в поток.
    // Получает информацию из физики о положении Луны и Земли и их тоже выводит.
    void log();
};


#endif //ARTEMISII_TELEMETRY_H