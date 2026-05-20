import os

def parse_ephemeris(file_path):
    ARTEMIS_2_START_JD = 2461132.44111111
    SEC_IN_DAY = 86400.0

    records = []

    if not os.path.exists(file_path):
        print(f"Ошибка: Файл {file_path} не найден.")
        return records

    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    start_index = -1
    for i, line in enumerate(lines):
        if "$$SOE" in line:
            start_index = i + 1
            break

    if start_index == -1:
        print("Ошибка: Маркер $$SOE не найден в файле.")
        return records

    idx = start_index
    while idx < len(lines):
        line1 = lines[idx].strip()

        if "$$EOE" in line1 or not line1:
            break

        try:
            current_jd = float(line1.split()[0])
            t = (current_jd - ARTEMIS_2_START_JD) * SEC_IN_DAY

            line2 = lines[idx + 1].strip()
            for char in ["X", "Y", "Z", "="]:
                line2 = line2.replace(char, "")
            parts_coord = line2.split()
            x = float(parts_coord[0]) * 1000.0
            y = float(parts_coord[1]) * 1000.0
            z = float(parts_coord[2]) * 1000.0

            line3 = lines[idx + 2].strip()
            for char in ["VX", "VY", "VZ", "="]:
                line3 = line3.replace(char, "")
            parts_vel = line3.split()
            vx = float(parts_vel[0]) * 1000.0
            vy = float(parts_vel[1]) * 1000.0
            vz = float(parts_vel[2]) * 1000.0

            records.append({
                't': t - 1503, 'x': x, 'y': y, 'z': z, 'vx': vx, 'vy': vy, 'vz': vz
            })

            idx += 4

        except (IndexError, ValueError) as e:
            print(f"Ошибка парсинга на строке {idx}: {e}")
            break

    return records

# --- Настройки путей ---
input_file = "horizons_results.txt"       # Исходный файл от Horizons
output_file = "parsed_artemis.txt"  # Сюда запишем результат

# 1. Парсим данные из исходного файла
parsed_data = parse_ephemeris(input_file)

if parsed_data:
    # 2. Сохраняем результат в новый файл
    with open(output_file, 'w', encoding='utf-8') as out_f:
        # Записываем шапку (заголовки столбцов)
        out_f.write("t_sec x_m y_m z_m vx_ms vy_ms vz_ms\n")

        # Записываем каждую строчку через пробел
        for rec in parsed_data:
            out_f.write(f"{rec['t']:.4f} {rec['x']:.4f} {rec['y']:.4f} {rec['z']:.4f} {rec['vx']:.4f} {rec['vy']:.4f} {rec['vz']:.4f}\n")

    print(f"Успешно! Распарсено точек: {len(parsed_data)}")
    print(f"Результат сохранен в файл: {os.path.abspath(output_file)}")