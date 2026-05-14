import re

def parse_horizons(filename):
    with open(filename, 'r') as f:
        content = f.read()

    # Ищем блок между $$SOE и $$EOE
    data_block = re.search(r'\$\$SOE(.*?)\$\$EOE', content, re.DOTALL)
    if not data_block:
        data_block = re.search(r'\$\$SOE(.*)', content, re.DOTALL)

    lines = data_block.group(1).strip().split('\n')
    results = []

    # Множитель для перевода КМ в Метры
    KM_TO_M = 1000

    for i in range(len(lines)):
        if ' = A.D.' in lines[i]:
            t = lines[i].split('=')[0].strip()
            # Ищем X, Y, Z на следующей строке
            coords = re.findall(r'[XYZ]\s*=\s*([^\sE]+(?:E[+-]\d+)?)', lines[i+1])

            if len(coords) == 3:
                # Пересчитываем каждую координату в метры
                x = float(coords[0]) * KM_TO_M
                y = float(coords[1]) * KM_TO_M
                z = float(coords[2]) * KM_TO_M

                # Сохраняем обратно в 4 колонки
                results.append(f"{t} {x} {y} {z}")

    return results

# Сохранение
parsed_data = parse_horizons('horizons_results.txt')
with open('output.txt', 'w') as out:
    out.write('\n'.join(parsed_data))