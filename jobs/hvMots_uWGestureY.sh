#!/bin/sh
#SBATCH -J HV-uWGestureY
#SBATCH -e HV-uWGestureY.err.txt
#SBATCH -o HV-uWGestureY.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configuWGestureY