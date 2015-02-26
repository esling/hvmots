//
//  classification.h
//  HV-MOTS-C
//
//  Created by Philippe Esling on 21/02/2014.
//  Copyright (c) 2014 Philippe Esling. All rights reserved.
//

#ifndef CLASSIFICATION_H_
# define CLASSIFICATION_H_

# include "types.h"
# include "main.h"

void                    classifyCollection(mParameters *params);
void                    classifyDatasetLOO(dataset *current, distances *dSet, criteria *crit);
void                    classifyDatasetTT(dataset *train, dataset *test, distances *distanceSet, criteria *criterias, mParameters *param);
void                    normalize_distance(float **data, int nobj, int rows);
int                     criteria_1NN(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_classes);
int                     criteria_5NN(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_classes);
int                     criteria_NC(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_classes);
int                     criteria_MOTS(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_classes);
int                     criteria_NPMOTS(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_classes);
int                     criteria_HVMOTS(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_classes);

#endif
