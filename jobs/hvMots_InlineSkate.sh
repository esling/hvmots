#!/bin/sh
#BSUB -J HV-loo-InlineSkate
#BSUB -e HV-InlineSkate.err.txt
#BSUB -o HV-InlineSkate.out.txt
#BSUB -n 16
#BSUB -R "span[ptile=32]"
#BSUB -R "rusage[mem=8192]"
#BSUB -M 8294304
cd ~/hvmots_loo && ./hvMots_red -config main.cfg -collection jobs/configInlineSkate