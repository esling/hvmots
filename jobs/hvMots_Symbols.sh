#!/bin/sh
#SBATCH -J HV-Symbols
#SBATCH -e HV-Symbols.err.txt
#SBATCH -o HV-Symbols.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configSymbols