#!/bin/sh
#SBATCH -J HV-FacesUCR
#SBATCH -e HV-FacesUCR.err.txt
#SBATCH -o HV-FacesUCR.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configFacesUCR