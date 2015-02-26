#!/bin/sh
#BSUB -J HV-loo-NonInv_ECG2
#BSUB -e HV-NonInv_ECG2.err.txt
#BSUB -o HV-NonInv_ECG2.out.txt
#BSUB -n 16
#BSUB -R "span[ptile=32]"
#BSUB -R "rusage[mem=8192]"
#BSUB -M 8294304
cd ~/hvmots_loo && ./hvMots_red -config main.cfg -collection jobs/configNonInv_ECG2