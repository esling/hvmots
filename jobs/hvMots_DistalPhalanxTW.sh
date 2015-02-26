#!/bin/sh
#SBATCH -J HV-DistalPhalanxTW
#SBATCH -e HV-DistalPhalanxTW.err.txt
#SBATCH -o HV-DistalPhalanxTW.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configDistalPhalanxTW