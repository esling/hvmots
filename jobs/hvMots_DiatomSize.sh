#!/bin/sh
#SBATCH -J HV-DiatomSize
#SBATCH -e HV-DiatomSize.err.txt
#SBATCH -o HV-DiatomSize.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configDiatomSize