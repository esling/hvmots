#!/bin/bash
#
#PBS -N HV-ItalyPower
#PBS -e HV-ItalyPower.err.txt
#PBS -o HV-ItalyPower.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=64
#PBS -q np_f64c
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configItalyPower
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configCar
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configSonyAIBO2
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configPhalangesOutlinesCorrect
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configFaceAll
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configChlorine
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configMALLAT
