set datafile separator whitespace
set grid
set key top right
set format y "%.2e"
set xlabel "Время, с"
set ylabel "Относительная ошибка"
set mouse
set terminal qt 0 enhanced font "Arial,12" size 1400,500 title "Позиция" persist
set output
set title "Относительная ошибка модуля радиус-вектора (|r2|-|r1|)/|r1|"
plot "compare_result.dat" using 1:2 with linespoints pt 7 ps 0.5 lw 1 lc rgb "#1f77b4" title "err\_pos"
set terminal qt 1 enhanced font "Arial,12" size 1400,500 title "Скорость" persist
set output
set title "Относительная ошибка модуля вектора скорости (|v2|-|v1|)/|v1|"
plot "compare_result.dat" using 1:3 with linespoints pt 7 ps 0.5 lw 1 lc rgb "#d62728" title "err\_vel"
