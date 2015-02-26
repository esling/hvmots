#!/bin/sh
#SBATCH -J HV-BeetleFly
#SBATCH -e HV-BeetleFly.err.txt
#SBATCH -o HV-BeetleFly.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configBeetleFly