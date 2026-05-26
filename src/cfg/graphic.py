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
                continue
    return rows

print(f"Читаю файл 1 (эталон): {FILE1}")
data1 = parse_file(FILE1, 0, 1, 2, 3, 4, 5, 6)
print(f"  строк: {len(data1)},  t: {data1[0][0]:.1f} .. {data1[-1][0]:.1f}")

print(f"Читаю файл 2 (расчёт): {FILE2}")
data2 = parse_file(FILE2, 0, 1, 2, 3, 4, 5, 6)
print(f"  строк: {len(data2)},  t: {data2[0][0]:.1f} .. {data2[-1][0]:.1f}")

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
# Колонки: t1  abs_err_pos  rel_err_pos  abs_err_vel  rel_err_vel  r1  r2  v1  v2
OUTPUT_DAT = "compare_result.dat"
results = []
skipped = 0

for row1 in data1:
    t1, x1, y1, z1, vx1, vy1, vz1 = row1
    row2, dt = find_nearest(t1)

    if t1 > 750000:
        break
    if dt > MAX_DT:
        skipped += 1
        continue

    t2, rx2, ry2, rz2, vx2, vy2, vz2 = row2
    r1 = norm3(x1, y1, z1)
    r2 = norm3(rx2, ry2, rz2)
    v1 = norm3(vx1, vy1, vz1)
    v2 = norm3(vx2, vy2, vz2)

    abs_err_pos = r1 - r2
    rel_err_pos = abs_err_pos / r1 if r1 != 0 else float('nan')
    abs_err_vel = v1 - v2
    rel_err_vel = abs_err_vel / v1 if v1 != 0 else float('nan')

    results.append((t1, abs_err_pos, rel_err_pos, abs_err_vel, rel_err_vel, r1, r2, v1, v2))

print(f"Совпавших точек: {len(results)},  пропущено (dt>{MAX_DT}с): {skipped}")

if not results:
    print("Нет совпадающих точек! Проверьте диапазоны времени или увеличьте MAX_DT.")
    sys.exit(1)

with open(OUTPUT_DAT, 'w') as fh:
    fh.write("# t1_sec  abs_err_pos  rel_err_pos  abs_err_vel  rel_err_vel  r1  r2  v1  v2\n")
    for row in results:
        fh.write("  ".join(f"{v:.6e}" for v in row) + "\n")
print(f"Результаты сохранены: {OUTPUT_DAT}")

# Статистика
abs_eps = [r[1] for r in results]
rel_eps = [r[2] for r in results]
abs_evs = [r[3] for r in results]
rel_evs = [r[4] for r in results]
print(f"\nabs_err_pos: min={min(abs_eps):.3e}  max={max(abs_eps):.3e}  mean={sum(abs_eps)/len(abs_eps):.3e}")
print(f"rel_err_pos: min={min(rel_eps):.3e}  max={max(rel_eps):.3e}  mean={sum(rel_eps)/len(rel_eps):.3e}")
print(f"abs_err_vel: min={min(abs_evs):.3e}  max={max(abs_evs):.3e}  mean={sum(abs_evs)/len(abs_evs):.3e}")
print(f"rel_err_vel: min={min(rel_evs):.3e}  max={max(rel_evs):.3e}  mean={sum(rel_evs)/len(rel_evs):.3e}")

# ── Gnuplot ──────────────────────────────────────────────────────────────────

GNUPLOT_INTERACTIVE = "plot_interactive.gp"
GNUPLOT_PNG         = "plot_png.gp"

PNG_POS_ABS  = "err_pos_abs.png"
PNG_POS_BOTH = "err_pos_both.png"   # абс + отн на одном
PNG_POS_MOD  = "pos_modules.png"    # |r1| и |r2| на одном
PNG_VEL_MOD  = "vel_modules.png"    # |v1| и |v2| на одном
PNG_VEL_ERR  = "err_vel_both.png"   # абс + отн ошибки скорости

COMMON = f"""\
set datafile separator whitespace
set grid
set key top right
set format y "%.2e"
set xlabel "Время, с"
"""

# ── График 1: абсолютная ошибка положения ────────────────────────────────────
PLOT1 = f"""\
set title "Абсолютная ошибка положения  |r_ref| - |r_calc|,  м"
set ylabel "Ошибка, м"
plot "{OUTPUT_DAT}" using 1:2 with lines lw 1.5 lc rgb "#1f77b4" title "abs err pos"
"""

# ── График 2: абс + относ ошибка положения (два Y) ───────────────────────────
PLOT2 = f"""\
set title "Ошибки положения: абсолютная и относительная"
set ylabel "Абсолютная ошибка, м"
set y2label "Относительная ошибка"
set ytics nomirror
set y2tics
set format y2 "%.2e"
plot "{OUTPUT_DAT}" using 1:2 with lines lw 1.5 lc rgb "#1f77b4" title "abs err pos" axes x1y1, \\
     "{OUTPUT_DAT}" using 1:3 with lines lw 1.5 lc rgb "#ff7f0e" title "rel err pos" axes x1y2
unset y2label
unset y2tics
set ytics mirror
"""

# ── График 2б: модули радиус-векторов эталона и расчёта ──────────────────────
PLOT2B = f"""\
set title "Модуль радиус-вектора: эталон vs расчёт"
set ylabel "Расстояние, м"
plot "{OUTPUT_DAT}" using 1:6 with lines lw 1.5 lc rgb "#1f77b4" title "|r| эталон", \\
     "{OUTPUT_DAT}" using 1:7 with lines lw 1.5 lc rgb "#d62728" title "|r| расчёт"
"""

# ── График 3: модули скоростей эталона и расчёта ─────────────────────────────
PLOT3 = f"""\
set title "Модуль скорости: эталон vs расчёт"
set ylabel "Скорость, м/с"
plot "{OUTPUT_DAT}" using 1:8 with lines lw 1.5 lc rgb "#2ca02c" title "|v| эталон", \\
     "{OUTPUT_DAT}" using 1:9 with lines lw 1.5 lc rgb "#d62728" title "|v| расчёт"
"""

# ── График 4: абс + относ ошибка скорости (два Y) ────────────────────────────
PLOT4 = f"""\
set title "Ошибки скорости: абсолютная и относительная"
set ylabel "Абсолютная ошибка, м/с"
set y2label "Относительная ошибка"
set ytics nomirror
set y2tics
set format y2 "%.2e"
plot "{OUTPUT_DAT}" using 1:4 with lines lw 1.5 lc rgb "#9467bd" title "abs err vel" axes x1y1, \\
     "{OUTPUT_DAT}" using 1:5 with lines lw 1.5 lc rgb "#e377c2" title "rel err vel" axes x1y2
unset y2label
unset y2tics
set ytics mirror
"""

# ── PNG скрипт ────────────────────────────────────────────────────────────────
gp_png = COMMON + f"""
set terminal pngcairo enhanced font "Arial,12" size 1400,500

set output "{PNG_POS_ABS}"
""" + PLOT1 + f"""
set output "{PNG_POS_BOTH}"
""" + PLOT2 + f"""
set output "{PNG_POS_MOD}"
""" + PLOT2B + f"""
set output "{PNG_VEL_MOD}"
""" + PLOT3 + f"""
set output "{PNG_VEL_ERR}"
""" + PLOT4

# ── Интерактивный скрипт — 4 окна ────────────────────────────────────────────
gp_interactive = COMMON + f"""
set mouse
set terminal qt 0 enhanced font "Arial,12" size 1400,500 title "Абс. ошибка положения" persist
set output
""" + PLOT1 + f"""
set terminal qt 1 enhanced font "Arial,12" size 1400,500 title "Ошибки положения" persist
set output
""" + PLOT2 + f"""
set terminal qt 2 enhanced font "Arial,12" size 1400,500 title "Модули положения" persist
set output
""" + PLOT2B + f"""
set terminal qt 3 enhanced font "Arial,12" size 1400,500 title "Модули скоростей" persist
set output
""" + PLOT3 + f"""
set terminal qt 4 enhanced font "Arial,12" size 1400,500 title "Ошибки скорости" persist
set output
""" + PLOT4

with open(GNUPLOT_PNG, 'w') as fh:
    fh.write(gp_png)
with open(GNUPLOT_INTERACTIVE, 'w') as fh:
    fh.write(gp_interactive)

try:
    subprocess.run(["gnuplot", GNUPLOT_PNG], check=True)
    print(f"\nPNG сохранены: {PNG_POS_ABS}, {PNG_POS_BOTH}, {PNG_POS_MOD}, {PNG_VEL_MOD}, {PNG_VEL_ERR}")
    subprocess.Popen(["gnuplot", GNUPLOT_INTERACTIVE])
    print("Интерактивные окна открыты (5 шт.)")
except FileNotFoundError:
    print(f"\ngnuplot не найден.")
    print(f"Интерактив: gnuplot {GNUPLOT_INTERACTIVE}")
    print(f"PNG:        gnuplot {GNUPLOT_PNG}")
except subprocess.CalledProcessError as e:
    print(f"\nОшибка gnuplot: {e}")