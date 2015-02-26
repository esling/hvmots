#!/bin/sh
#SBATCH -J HV-CBF
#SBATCH -e HV-CBF.err.txt
#SBATCH -o HV-CBF.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configCBF