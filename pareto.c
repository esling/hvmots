/*
 *  pareto.c
 *  HV-MOTS Classification
 *
 *	This file contains the functions for computing the Pareto front.
 *	Adapted from the code of Yi Cao : y.cao@cranfield.ac.uk
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "pareto.h"

void paretofront(int *front, double *M, unsigned int row, unsigned int col)
{
    unsigned int t,s,i,j,j1,j2;
    int *checklist, coldominatedflag;
    
    checklist = (int *)calloc(row, sizeof(int));
    for(t = 0; t<row; t++) checklist[t] = 1;
    for(s = 0; s<row; s++) {
        t=s;
        if (!checklist[t]) continue;
        checklist[t]=0;
        coldominatedflag=1;
        for(i=t+1;i<row;i++) {
            if (!checklist[i]) continue;
            checklist[i]=0;
            for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
                if (M[j1] <= M[j2]) {
                    checklist[i]=1;
                    break;
                }
            }
            if (!checklist[i]) continue;
            coldominatedflag=0;
            for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
                if (M[j1] > M[j2]) {
                    coldominatedflag=1;
                    break;
                }
            }
            if (!coldominatedflag)
            {
                front[t]=0;
                checklist[i]=0;
                coldominatedflag=1;
                t=i;
            }
        }
        front[t]=coldominatedflag;
        if (t>s) {
            for (i=s+1; i<t; i++) {
                if (!checklist[i]) continue;
                checklist[i]=0;
                for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
                    if (M[j1] <= M[j2]) {
                        checklist[i]=1;
                        break;
                    }
                }
            }
        }
    }
    free(checklist); 
}