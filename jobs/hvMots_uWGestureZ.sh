#!/bin/sh
#SBATCH -J HV-uWGestureZ
#SBATCH -e HV-uWGestureZ.err.txt
#SBATCH -o HV-uWGestureZ.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configuWGestureZ