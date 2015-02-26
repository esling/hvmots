#!/bin/sh
#SBATCH -J HV-Lighting7
#SBATCH -e HV-Lighting7.err.txt
#SBATCH -o HV-Lighting7.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configLighting7