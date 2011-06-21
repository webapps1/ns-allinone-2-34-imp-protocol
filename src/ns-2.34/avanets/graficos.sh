#!/bin/bash

rm entrega.data

../ns example.tcl
./calc_entr.sh simple.tr

# 0.1
cat example.tcl  | sed -e "s/$cbr1 set interval_ 1.0/$cbr1 set interval_ 0.1/g" > example_0.1.tcl
../ns example_0.1.tcl
./calc_entr.sh simple.tr
rm example_0.1.tcl

#0.01
cat example.tcl  | sed -e "s/$cbr1 set interval_ 1.0/$cbr1 set interval_ 0.01/g" > example_0.01.tcl
../ns example_0.01.tcl
./calc_entr.sh simple.tr
rm example_0.01.tcl

#0.001
cat example.tcl  | sed -e "s/$cbr1 set interval_ 1.0/$cbr1 set interval_ 0.001/g" > example_0.001.tcl
../ns example_0.001.tcl
./calc_entr.sh simple.tr
rm example_0.001.tcl

gnuplot grafico_taxa
