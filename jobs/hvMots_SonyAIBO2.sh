#!/bin/sh
#SBATCH -J HV-SonyAIBO2
#SBATCH -e HV-SonyAIBO2.err.txt
#SBATCH -o HV-SonyAIBO2.out.txt
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -p parallel
#SBATCH -t 4-0:00
cd .. && srun ./hvMots -config main.cfg -collection jobs/configSonyAIBO2