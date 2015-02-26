#!/bin/sh
#SBATCH -J HV-ProximalPhalanxOutlineAgeGroup
#SBATCH -e HV-ProximalPhalanxOutlineAgeGroup.err.txt
#SBATCH -o HV-ProximalPhalanxOutlineAgeGroup.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configProximalPhalanxOutlineAgeGroup