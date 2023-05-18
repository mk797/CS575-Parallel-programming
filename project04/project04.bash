#!/bin/bash

	printf "Arraysize,NonsimdMul,SimdMul,performanceMul,NonSimdmulSum,SimdmulSum,Performance_MulSUm\n";

for n in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608
do
  for t in 4 8 16 32 64
    do
    g++   project04.cpp  -DARRAYSIZE=$n -DNUMT=$t  -o proj04  -lm  -fopenmp
    ./proj04
    done
done

