#!/bin/sh
#SBATCH -J HV-Gun_Point
#SBATCH -e HV-Gun_Point.err.txt
#SBATCH -o HV-Gun_Point.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configGun_Point