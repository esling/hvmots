#!/bin/sh
#SBATCH -J HV-MedicalImages
#SBATCH -e HV-MedicalImages.err.txt
#SBATCH -o HV-MedicalImages.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configMedicalImages