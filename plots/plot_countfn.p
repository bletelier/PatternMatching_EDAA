set terminal postscript eps size 8,4 enhanced color \
    font 'Helvetica,20' linewidth 2
set encoding utf8
set datafile separator ","
set output "count_fn.eps"
set grid
set termoption dashed
set xlabel "{/:Bold Tamaño de patrón [}{/:Italic p}{/:Bold ]}"
set ylabel "{/:Bold Mediana de tiempo [}{/:Italic ns}{/:Bold ]}"
set key outside
set logscale y

plot '<grep "fmi" ../res/dna.csv' using 2:7 title "fmi" with linespoints, \
     '<grep "csa" ../res/dna.csv' using 2:7 title "csa" with linespoints, \
     '<grep "sa" ../res/dna.csv' using 2:7 title "sa" with linespoints, \
