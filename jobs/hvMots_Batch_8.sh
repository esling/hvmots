#!/bin/bash
#
#PBS -N HV-DistalPhal
#PBS -e HV-DistalPhalanxOutlineAgeGroup.err.txt
#PBS -o HV-DistalPhalanxOutlineAgeGroup.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=64
#PBS -q np_f64c
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configDistalPhalanxOutlineAgeGroup
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configFaceFour
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configShapeletSim
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configAdiac
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configWordsSynonyms
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configTwo_Patterns
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configCinECG
