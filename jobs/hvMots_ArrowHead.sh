#!/bin/sh
#SBATCH -J HV-ArrowHead
#SBATCH -e HV-ArrowHead.err.txt
#SBATCH -o HV-ArrowHead.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configArrowHead