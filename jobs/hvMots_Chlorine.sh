#!/bin/sh
#SBATCH -J HV-Chlorine
#SBATCH -e HV-Chlorine.err.txt
#SBATCH -o HV-Chlorine.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configChlorine