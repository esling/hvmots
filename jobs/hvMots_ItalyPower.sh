#!/bin/sh
#SBATCH -J HV-ItalyPower
#SBATCH -e HV-ItalyPower.err.txt
#SBATCH -o HV-ItalyPower.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configItalyPower