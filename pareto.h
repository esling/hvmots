/*
 *  pareto.h
 *  HV-MOTS Classification
 *
 *	This file contains the functions for computing the Pareto front.
 *	Adapted from the code of Yi Cao : y.cao@cranfield.ac.uk
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#ifndef PARETO_H_
#define PARETO_H_

void            paretofront(int *front, double *M, unsigned int row, unsigned int col);

#endif
