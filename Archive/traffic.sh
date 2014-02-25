#! /bin/bash

L=1000
t=2000
vmax=5
p=0.2
c=0.3
r="$t/$L"

command="./traffic $L $t"

( #echo 'unset key'
    #echo 'set tic scale 0'
    echo "set size ratio $t/$L"
    echo "set xrange [0:$L]"
    echo "set lmargin 0"
    echo "set bmargin 0"
    echo "set rmargin 0"
    echo "set tmargin 0"
    echo "set yrange [0:$t] reverse"
    echo 'set pm3d map'
    echo 'set palette gray'
    echo 'unset colorbox'
    echo 'set term postscript eps enhanced color'
    echo 'set output "plotplot.eps"'
    #echo 'set term x11'
    echo "splot \"<$command\" matrix with image notitle"  ) > temp.gp


gnuplot  temp.gp
open plotplot.eps
#\rm temp.gp


