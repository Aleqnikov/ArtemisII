set datafile separator whitespace
set grid
set key top right
set format y "%.2e"
set mouse
set autoscale

# ── Общие настройки осей (применятся ко всем графикам) ──
set xlabel "Время, с"
set ylabel "Относительная ошибка"

# ── График 1: позиция ────────────────────────────────────
set title "Относительная ошибка модуля радиус-вектора (|r2|-|r1|)/|r1|"

# Интерактивное окно (зум колёсиком/выделением, пан правой кнопкой)
set terminal qt 0 enhanced font "Arial,12" size 1400,500 title "Позиция" persist
set output
plot "compare_result.dat" using 1:2 with linespoints pt 7 ps 0.5 lw 1 lc rgb "#1f77b4" title "err\_pos"

# Сохранить тот же график в PNG
set terminal pngcairo enhanced font "Arial,12" size 1400,500
set output "err_position.png"
replot

# ── График 2: скорость ───────────────────────────────────
set title "Относительная ошибка модуля вектора скорости (|v2|-|v1|)/|v1|"

set terminal qt 1 enhanced font "Arial,12" size 1400,500 title "Скорость" persist
set output
plot "compare_result.dat" using 1:3 with linespoints pt 7 ps 0.5 lw 1 lc rgb "#d62728" title "err\_vel"

set terminal pngcairo enhanced font "Arial,12" size 1400,500
set output "err_velocity.png"
replot
