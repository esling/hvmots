#!/bin/sh
#SBATCH -J HV-ECG200
#SBATCH -e HV-ECG200.err.txt
#SBATCH -o HV-ECG200.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configECG200