reset
set title "Page Cache Hit Count"
set xlabel "Files"
set ylabel "Page Hits"
set autoscale

#plot "pip-channel-hop.txt" using 1:4 t "Without Interfarence" w linespoint lt 4 pt 3 lc rgb "blue"

set style data histograms
set style histogram cluster gap 3
#set boxwidth 0.25

set style fill pattern border lt 0 

plot "table.txt" using 2:xtic(1) fill solid 1.0 lw 2 lc rgb "red"


set terminal postscript portrait enhanced mono dashed lw 0.5 "Helvetica" 4 
set output "page_hits.eps"
replot
