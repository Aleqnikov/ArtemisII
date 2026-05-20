#!/usr/bin/env python3
"""
compare_traj.py  —  сравнение двух файлов траектории.

Файл 1 (эталон):  t_sec  x_m  y_m  z_m  vx_ms  vy_ms  vz_ms
Файл 2 (расчёт):  t  rx  ry  rz  vx  vy  vz  ...  (остальные колонки игнорируются)

Для каждой строки файла 1 ищем строку файла 2 с минимальным |t1 - t2|.
Если |dt| > max_dt_threshold — точка пропускается.
"""

import sys, os, math, subprocess, bisect

FILE1      = sys.argv[1] if len(sys.argv) > 1 else "parsed_artemis.txt"
FILE2      = sys.argv[2] if len(sys.argv) > 2 else "exit.txt"
MAX_DT     = float(sys.argv[3]) if len(sys.argv) > 3 else 30.0  # сек

for f in (FILE1, FILE2):
    if not os.path.isfile(f):
        print(f"Ошибка: файл не найден — {f}"); sys.exit(1)

def norm3(x, y, z): return math.sqrt(x*x + y*y + z*z)

def parse_file(path, t_col, x_col, y_col, z_col, vx_col, vy_col, vz_col, skip_header=True):
    rows = []
    with open(path) as fh:
        for line in fh:
            line = line.strip()
            if not line or line.startswith('#'): continue
            p = line.split()
            cols_needed = max(t_col, x_col, y_col, z_col, vx_col, vy_col, vz_col) + 1
            if len(p) < cols_needed: continue
            try:
                rows.append((float(p[t_col]), float(p[x_col]), float(p[y_col]), float(p[z_col]),
                             float(p[vx_col]), float(p[vy_col]), float(p[vz_col])))
            except ValueError:
                continue  # пропускаем заголовок и мусор
    return rows

print(f"Читаю файл 1 (эталон): {FILE1}")
# t x y z vx vy vz  — колонки 0..6
data1 = parse_file(FILE1, 0, 1, 2, 3, 4, 5, 6)
print(f"  строк: {len(data1)},  t: {data1[0][0]:.1f} .. {data1[-1][0]:.1f}")

print(f"Читаю файл 2 (расчёт): {FILE2}")
# t rx ry rz vx vy vz  — колонки 0..6
data2 = parse_file(FILE2, 0, 1, 2, 3, 4, 5, 6)
print(f"  строк: {len(data2)},  t: {data2[0][0]:.1f} .. {data2[-1][0]:.1f}")

# Сортируем файл 2 по времени
data2.sort(key=lambda r: r[0])
times2 = [r[0] for r in data2]

t1_min, t1_max = data1[0][0],  data1[-1][0]
t2_min, t2_max = times2[0],    times2[-1]
overlap = (max(t1_min, t2_min), min(t1_max, t2_max))
print(f"\nПересечение по времени: {overlap[0]:.1f} .. {overlap[1]:.1f} с")
print(f"Порог dt_max = {MAX_DT} с\n")

def find_nearest(t_target):
    i = bisect.bisect_left(times2, t_target)
    if i > 0 and (i == len(times2) or abs(times2[i-1]-t_target) < abs(times2[i]-t_target)):
        i -= 1
    return data2[i], abs(times2[i] - t_target)

# Вычисляем ошибки
OUTPUT_DAT = "compare_result.dat"
results = []
skipped = 0

for row1 in data1:
    t1, x1, y1, z1, vx1, vy1, vz1 = row1
    row2, dt = find_nearest(t1)

    if t1 > 750000:
        break;
    if dt > MAX_DT:
        skipped += 1
        continue
    t2, rx2, ry2, rz2, vx2, vy2, vz2 = row2
    r1 = norm3(x1, y1, z1);  r2 = norm3(rx2, ry2, rz2)
    v1 = norm3(vx1, vy1, vz1); v2 = norm3(vx2, vy2, vz2)
    err_pos = (r2 - r1) / r1 if r1 != 0 else float('nan')
    err_vel = (v2 - v1) / v1 if v1 != 0 else float('nan')
    results.append((t1, err_pos, err_vel, t2 - t1))

print(f"Совпавших точек: {len(results)},  пропущено (dt>{MAX_DT}с): {skipped}")

if not results:
    print("Нет совпадающих точек! Проверьте диапазоны времени или увеличьте MAX_DT.")
    sys.exit(1)

with open(OUTPUT_DAT, 'w') as fh:
    fh.write("# t1_sec   rel_err_pos   rel_err_vel   dt_match_sec\n")
    for t1, ep, ev, dt in results:
        fh.write(f"{t1:.4f}  {ep:.6e}  {ev:.6e}  {dt:.4f}\n")
print(f"Результаты сохранены: {OUTPUT_DAT}")

# Статистика
eps = [r[1] for r in results]; evs = [r[2] for r in results]
print(f"\nerr_pos: min={min(eps):.3e}  max={max(eps):.3e}  mean={sum(eps)/len(eps):.3e}")
print(f"err_vel: min={min(evs):.3e}  max={max(evs):.3e}  mean={sum(evs)/len(evs):.3e}")

# Gnuplot
GNUPLOT_INTERACTIVE = "plot_interactive.gp"
GNUPLOT_PNG        = "plot_png.gp"
PNG_POS = "err_position.png"
PNG_VEL = "err_velocity.png"

# Общие команды графиков
PLOT_COMMON = f"""\
set datafile separator whitespace
set grid
set key top right
set format y "%.2e"
set xlabel "Время, с"
set ylabel "Относительная ошибка"
"""

PLOT_POS = f"""\
set title "Относительная ошибка модуля радиус-вектора (|r2|-|r1|)/|r1|"
plot "{OUTPUT_DAT}" using 1:2 with linespoints pt 7 ps 0.5 lw 1 lc rgb "#1f77b4" title "err\\_pos"
"""

PLOT_VEL = f"""\
set title "Относительная ошибка модуля вектора скорости (|v2|-|v1|)/|v1|"
plot "{OUTPUT_DAT}" using 1:3 with linespoints pt 7 ps 0.5 lw 1 lc rgb "#d62728" title "err\\_vel"
"""

# Интерактивный скрипт — два окна, persist держит их открытыми
gp_interactive = PLOT_COMMON + f"""\
set mouse
set terminal qt 0 enhanced font "Arial,12" size 1400,500 title "Позиция" persist
set output
""" + PLOT_POS + f"""\
set terminal qt 1 enhanced font "Arial,12" size 1400,500 title "Скорость" persist
set output
""" + PLOT_VEL

# PNG скрипт — просто сохраняет файлы
gp_png = PLOT_COMMON + f"""\
set terminal pngcairo enhanced font "Arial,12" size 1400,500
set output "{PNG_POS}"
""" + PLOT_POS + f"""\
set output "{PNG_VEL}"
""" + PLOT_VEL

with open(GNUPLOT_INTERACTIVE, 'w') as fh:
    fh.write(gp_interactive)
with open(GNUPLOT_PNG, 'w') as fh:
    fh.write(gp_png)


try:
    # Сначала сохраняем PNG
    subprocess.run(["gnuplot", GNUPLOT_PNG], check=True)
    print(f"PNG сохранены: {PNG_POS}, {PNG_VEL}")
    # Потом открываем интерактивные окна (не блокируют — persist)
    subprocess.Popen(["gnuplot", GNUPLOT_INTERACTIVE])
    print("Интерактивные окна открыты (зум — колёсико/выделение, пан — правая кнопка)")
except FileNotFoundError:
    print(f"\ngnuplot не найден.")
    print(f"Интерактив: gnuplot {GNUPLOT_INTERACTIVE}")
    print(f"PNG:        gnuplot {GNUPLOT_PNG}")
except subprocess.CalledProcessError as e:
    print(f"\nОшибка gnuplot: {e}")