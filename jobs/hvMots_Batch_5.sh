#!/bin/bash
#
#PBS -N HV-Plane
#PBS -e HV-Plane.err.txt
#PBS -o HV-Plane.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=128
#PBS -q np_f128c
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configPlane
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configToeSegmentation2
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configLighting7
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configCBF
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configCricket_Y
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configShapesAll
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configuWGestureY
cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/configStarLight
