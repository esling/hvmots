#!/bin/bash
#
#PBS -N HV-2
#PBS -e HV-BeetleFly.err.txt
#PBS -o HV-BeetleFly.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=5
#PBS -q np_f128c
#PBS -J 0-24
datasets=("BeetleFly" "Plane" "DistalPhalanxOutlineAgeGroup" "MiddlePhalanxTW" "ProximalPhalanxTW" "Synthetic" "ToeSegmentation1" "MiddlePhalanxOutlineCorrect" "ProximalPhalanxOutlineCorrect" "SonyAIBO2" "TwoLeadECG" "CBF" "Adiac" "Earthquakes" "Cricket_X" "50words" "FacesUCR" "SmallKitchenAppliances" "Symbols" "Chlorine" "ARSim" "uWGestureY" "CinECG" "FordB" "HandOutlines")

cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/config${datasets[$PBS_ARRAY_INDEX]}
