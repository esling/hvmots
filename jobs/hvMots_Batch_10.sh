#!/bin/bash
#
#PBS -N HV-MiddlePhal
#PBS -e HV-MiddlePhalanxOutlineAgeGroup.err.txt
#PBS -o HV-MiddlePhalanxOutlineAgeGroup.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=64
#PBS -q np_f64c
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configMiddlePhalanxOutlineAgeGroup
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configToeSegmentation1
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configLighting2
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configFish
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configFacesUCR
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configWafer
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configFordA
