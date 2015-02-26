#!/bin/sh
#SBATCH -J HV-HandOutlines
#SBATCH -e HV-HandOutlines.err.txt
#SBATCH -o HV-HandOutlines.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configHandOutlines