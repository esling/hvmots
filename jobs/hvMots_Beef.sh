#!/bin/sh
#SBATCH -J HV-Beef
#SBATCH -e HV-Beef.err.txt
#SBATCH -o HV-Beef.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configBeef