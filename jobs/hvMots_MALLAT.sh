#!/bin/sh
#SBATCH -J HV-MALLAT
#SBATCH -e HV-MALLAT.err.txt
#SBATCH -o HV-MALLAT.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configMALLAT