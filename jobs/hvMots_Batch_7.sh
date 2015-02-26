#!/bin/bash
#
#PBS -N HV-DistalPhal
#PBS -e HV-DistalPhalanxTW.err.txt
#PBS -o HV-DistalPhalanxTW.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=64
#PBS -q np_f64c
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configDistalPhalanxTW
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configSynthetic
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configTrace
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configECGFiveDays
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/config50words
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configHaptics
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configYoga
