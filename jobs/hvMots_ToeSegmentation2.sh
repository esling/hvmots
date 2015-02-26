#!/bin/sh
#SBATCH -J HV-ToeSegmentation2
#SBATCH -e HV-ToeSegmentation2.err.txt
#SBATCH -o HV-ToeSegmentation2.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configToeSegmentation2