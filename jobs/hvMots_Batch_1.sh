#!/bin/bash
#
#PBS -N HV-1
#PBS -e HV-Coffee.err.txt
#PBS -o HV-Coffee.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=5
#PBS -q np_f128c
#PBS -J 0-24
datasets=("Coffee" "Beef" "DistalPhalanxTW" "MiddlePhalanxOutlineAgeGroup" "ProximalPhalanxOutlineAgeGroup" "ArrowHead" "Car" "SonyAIBO1" "Lighting7" "ShapeletSim" "SwedishLeaf" "MoteStrain" "ECGFiveDays" "Fish" "Computers" "Cricket_Z" "FaceAll" "ScreenType" "ShapesAll" "Two_Patterns" "ElectricDevices" "uWGestureX" "Yoga" "FordA" "NonInv_ECG2")

cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/config${datasets[$PBS_ARRAY_INDEX]}
