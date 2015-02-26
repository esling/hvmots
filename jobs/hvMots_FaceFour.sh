#!/bin/sh
#SBATCH -J HV-FaceFour
#SBATCH -e HV-FaceFour.err.txt
#SBATCH -o HV-FaceFour.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configFaceFour