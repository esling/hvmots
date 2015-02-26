#!/bin/sh
#SBATCH -J HV-Yoga
#SBATCH -e HV-Yoga.err.txt
#SBATCH -o HV-Yoga.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configYoga