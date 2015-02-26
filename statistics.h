//
//  statistics.h
//  HV-MOTS-C
//
//  Created by Philippe Esling on 26/03/2014.
//  Copyright (c) 2014 Philippe Esling. All rights reserved.
//

#ifndef STATISTICS_H_
# define STATISTICS_H_

typedef struct      statistics_t
{
    float           error;
    int             combo;
    int             nbClass;
    int             nbErrors;
    int             *classesErrors;
    int             **confusionMatrix;
} statistics;

statistics          *init_statistics(int nb_classes);
statistics          *duplicate_statistics(statistics *ret);
void                export_statistics(FILE *out, statistics *ret);
void                empty_statistics(statistics *ret);
void                free_statistics(statistics *ret);

#endif
