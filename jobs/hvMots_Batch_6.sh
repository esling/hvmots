#!/bin/bash
#
#PBS -N HV-OliveOil
#PBS -e HV-OliveOil.err.txt
#PBS -o HV-OliveOil.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=128
#PBS -q np_f128c
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configOliveOil
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configArrowHead
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configProximalPhalanxOutlineCorrect
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configMedicalImages
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configCricket_Z
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configSymbols
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configuWGestureZ
