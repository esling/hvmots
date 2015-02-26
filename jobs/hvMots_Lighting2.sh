#!/bin/sh
#SBATCH -J HV-Lighting2
#SBATCH -e HV-Lighting2.err.txt
#SBATCH -o HV-Lighting2.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configLighting2