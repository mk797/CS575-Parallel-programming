#!/bin/bash

for n in 1024 2048 302 4096 8192 16384 32768 65536131072 262144524288 10485762090152  41943048388608
do
    g++   project04.cpp  -DARRAYSIZE=$n  -o proj04  -lm  -fopenmp
    ./proj04
done

