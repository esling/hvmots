#!/bin/bash
#
#PBS -N HV-3
#PBS -e HV-BirdChicken.err.txt
#PBS -o HV-BirdChicken.out.txt
#PBS -l walltime=12:00:00,select=1:ncpus=5
#PBS -q np_f128c
#PBS -J 0-24
datasets=("BirdChicken" "OliveOil" "ItalyPower" "Gun_Point" "ToeSegmentation2" "FaceFour" "DistalPhalanxOutlineCorrect" "Herring" "Trace" "Lighting2" "DiatomSize" "MedicalImages" "PhalangesOutlinesCorrect" "OSULeaf" "Cricket_Y" "WordsSynonyms" "LargeKitchenAppliances" "RefrigerationDevices" "Haptics" "Wafer" "InlineSkate" "uWGestureZ" "MALLAT" "NonInv_ECG1" "StarLight")

cd ~/hvmots && ./hvMots -config main.cfg -collection jobs/config${datasets[$PBS_ARRAY_INDEX]}
