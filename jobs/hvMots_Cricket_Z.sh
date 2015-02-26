#!/bin/sh
#BSUB -J HV-loo-Cricket_Z
#BSUB -e HV-Cricket_Z.err.txt
#BSUB -o HV-Cricket_Z.out.txt
#BSUB -n 16
#BSUB -R "span[ptile=32]"
#BSUB -R "rusage[mem=8192]"
#BSUB -M 8294304
cd ~/hvmots_loo && ./hvMots_red -config main.cfg -collection jobs/configCricket_Z