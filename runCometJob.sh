#!/bin/bash

#SBATCH --job-name="dast3648-CSCI4576-HW6"  
#SBATCH --output="dast3648-HW6.%j.%N.out"  
#SBATCH --partition=compute  
#SBATCH --nodes=6
#SBATCH --ntasks-per-node=24
#SBATCH --export=ALL  
#SBATCH -t 00:10:00

ibrun -v ./StrawnDaniel_HW6-3 -m 144
ibrun -v ./StrawnDaniel_HW6-3 -m 576
ibrun -v ./StrawnDaniel_HW6-3 -m 1152
ibrun -v ./StrawnDaniel_HW6-3 -m 2304
