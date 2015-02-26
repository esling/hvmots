#!/bin/sh
#SBATCH -J HV-Adiac
#SBATCH -e HV-Adiac.err.txt
#SBATCH -o HV-Adiac.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configAdiac