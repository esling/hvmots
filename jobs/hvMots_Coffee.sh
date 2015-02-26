#!/bin/sh
#SBATCH -J HV-Coffee
#SBATCH -e HV-Coffee.err.txt
#SBATCH -o HV-Coffee.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configCoffee