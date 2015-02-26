#!/bin/sh
#SBATCH -J HV-MoteStrain
#SBATCH -e HV-MoteStrain.err.txt
#SBATCH -o HV-MoteStrain.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configMoteStrain