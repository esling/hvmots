//
//  optimize.h
//  HV-MOTS-C
//
//  Created by Philippe Esling on 27/03/2014.
//  Copyright (c) 2014 Philippe Esling. All rights reserved.
//

#ifndef OPTIMIZE_H_
# define OPTIMIZE_H_

#include "types.h"
#include "main.h"

void                    optimizeDistance(dataset *train, distances *distanceSet, criteria *criterias, int id, mParameters *param);
void                    optimizeDatasetTT(dataset *train, distances *distanceSet, criteria *criterias, mParameters *param);

#endif
