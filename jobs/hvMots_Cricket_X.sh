#!/bin/sh
#SBATCH -J HV-Cricket_X
#SBATCH -e HV-Cricket_X.err.txt
#SBATCH -o HV-Cricket_X.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configCricket_X