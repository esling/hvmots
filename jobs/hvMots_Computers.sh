#!/bin/sh
#SBATCH -J HV-Computers
#SBATCH -e HV-Computers.err.txt
#SBATCH -o HV-Computers.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configComputers