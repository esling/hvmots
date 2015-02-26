#!/bin/sh
#SBATCH -J HV-ToeSegmentation1
#SBATCH -e HV-ToeSegmentation1.err.txt
#SBATCH -o HV-ToeSegmentation1.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configToeSegmentation1