set datafile separator whitespace
set grid
set key top right
set format y "%.2e"
set xlabel "Время, с"

set mouse
set terminal qt 0 enhanced font "Arial,12" size 1400,500 title "Абс. ошибка положения" persist
set output
set title "Абсолютная ошибка положения  |r_ref| - |r_calc|,  м"
set ylabel "Ошибка, м"
plot "compare_result.dat" using 1:2 with lines lw 1.5 lc rgb "#1f77b4" title "abs err pos"

set terminal qt 1 enhanced font "Arial,12" size 1400,500 title "Ошибки положения" persist
set output
set title "Ошибки положения: абсолютная и относительная"
set ylabel "Абсолютная ошибка, м"
set y2label "Относительная ошибка"
set ytics nomirror
set y2tics
set format y2 "%.2e"
plot "compare_result.dat" using 1:2 with lines lw 1.5 lc rgb "#1f77b4" title "abs err pos" axes x1y1, \
     "compare_result.dat" using 1:3 with lines lw 1.5 lc rgb "#ff7f0e" title "rel err pos" axes x1y2
unset y2label
unset y2tics
set ytics mirror

set terminal qt 2 enhanced font "Arial,12" size 1400,500 title "Модули положения" persist
set output
set title "Модуль радиус-вектора: эталон vs расчёт"
set ylabel "Расстояние, м"
plot "compare_result.dat" using 1:6 with lines lw 1.5 lc rgb "#1f77b4" title "|r| эталон", \
     "compare_result.dat" using 1:7 with lines lw 1.5 lc rgb "#d62728" title "|r| расчёт"

set terminal qt 3 enhanced font "Arial,12" size 1400,500 title "Модули скоростей" persist
set output
set title "Модуль скорости: эталон vs расчёт"
set ylabel "Скорость, м/с"
plot "compare_result.dat" using 1:8 with lines lw 1.5 lc rgb "#2ca02c" title "|v| эталон", \
     "compare_result.dat" using 1:9 with lines lw 1.5 lc rgb "#d62728" title "|v| расчёт"

set terminal qt 4 enhanced font "Arial,12" size 1400,500 title "Ошибки скорости" persist
set output
set title "Ошибки скорости: абсолютная и относительная"
set ylabel "Абсолютная ошибка, м/с"
set y2label "Относительная ошибка"
set ytics nomirror
set y2tics
set format y2 "%.2e"
plot "compare_result.dat" using 1:4 with lines lw 1.5 lc rgb "#9467bd" title "abs err vel" axes x1y1, \
     "compare_result.dat" using 1:5 with lines lw 1.5 lc rgb "#e377c2" title "rel err vel" axes x1y2
unset y2label
unset y2tics
set ytics mirror
