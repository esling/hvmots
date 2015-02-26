#!/bin/sh
#SBATCH -J HV-50words
#SBATCH -e HV-50words.err.txt
#SBATCH -o HV-50words.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/config50words