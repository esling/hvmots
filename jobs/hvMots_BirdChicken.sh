#!/bin/sh
#SBATCH -J HV-BirdChicken
#SBATCH -e HV-BirdChicken.err.txt
#SBATCH -o HV-BirdChicken.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configBirdChicken