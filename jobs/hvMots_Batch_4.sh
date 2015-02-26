#!/bin/bash
#
#PBS -N HV-Beef
#PBS -e HV-Beef.err.txt
#PBS -o HV-Beef.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=128
#PBS -q np_f128c
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configBeef
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configProximalPhalanxTW
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configHerring
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configMoteStrain
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configCricket_X
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configRefrigerationDevices
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configuWGestureX
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configHandOutlines
