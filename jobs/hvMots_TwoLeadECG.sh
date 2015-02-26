#!/bin/sh
#SBATCH -J HV-TwoLeadECG
#SBATCH -e HV-TwoLeadECG.err.txt
#SBATCH -o HV-TwoLeadECG.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configTwoLeadECG