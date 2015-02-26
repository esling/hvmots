#!/bin/sh
#SBATCH -J HV-DistalPhalanxOutlineCorrect
#SBATCH -e HV-DistalPhalanxOutlineCorrect.err.txt
#SBATCH -o HV-DistalPhalanxOutlineCorrect.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configDistalPhalanxOutlineCorrect