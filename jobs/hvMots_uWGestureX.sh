#!/bin/sh
#SBATCH -J HV-uWGestureX
#SBATCH -e HV-uWGestureX.err.txt
#SBATCH -o HV-uWGestureX.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configuWGestureX