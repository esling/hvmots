#!/bin/sh
#BSUB -J HV-loo-ShapeletSim
#BSUB -e HV-ShapeletSim.err.txt
#BSUB -o HV-ShapeletSim.out.txt
#BSUB -n 16
#BSUB -R "span[ptile=32]"
#BSUB -R "rusage[mem=8192]"
#BSUB -M 8294304
cd ~/hvmots_loo && ./hvMots_red -config main.cfg -collection jobs/configShapeletSim