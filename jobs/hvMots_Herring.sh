#!/bin/sh
#SBATCH -J HV-Herring
#SBATCH -e HV-Herring.err.txt
#SBATCH -o HV-Herring.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configHerring