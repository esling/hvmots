//
//  statistics.c
//  HV-MOTS-C
//
//  Created by Philippe Esling on 26/03/2014.
//  Copyright (c) 2014 Philippe Esling. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "statistics.h"

statistics          *init_statistics(int nb_classes)
{
    statistics      *ret;
    int             i;
    
    ret = calloc(1, sizeof(statistics));
    ret->error = 0.0f;
    ret->nbErrors = 0;
    ret->combo = 0;
    ret->nbClass = nb_classes;
    ret->classesErrors = calloc(nb_classes, sizeof(int));
    ret->confusionMatrix = calloc(nb_classes, sizeof(int *));
    for (i = 0; i < nb_classes; i++)
        ret->confusionMatrix[i] = calloc(nb_classes, sizeof(int));
    return ret;
}

statistics          *duplicate_statistics(statistics *ret)
{
    statistics      *result;
    int             i, j;
    
    result = calloc(1, sizeof(statistics));
    result->error = ret->error;
    result->combo = ret->combo;
    result->nbErrors = ret->nbErrors;
    result->classesErrors = calloc(ret->nbClass, sizeof(int));
    for (i = 0; i < ret->nbClass; i++)
        result->classesErrors[i] = ret->classesErrors[i];
    result->confusionMatrix = calloc(ret->nbClass, sizeof(int *));
    for (i = 0; i < ret->nbClass; i++)
    {
        result->confusionMatrix[i] = calloc(ret->nbClass, sizeof(int));
        for (j = 0; j < ret->nbClass; j++)
            result->confusionMatrix[i][j] = ret->confusionMatrix[i][j];
    }
    return result;
}

void                export_statistics(FILE *out, statistics *ret)
{
    int             i, j;
    
    fprintf(out, "Errors \t: %d\n", ret->nbErrors);
    fprintf(out, "Rate \t: %f\n", ret->error);
    fprintf(out, "Classes errors :\n");
    for (i = 0; i < ret->nbClass; i++)
        fprintf(out, "%d ", ret->classesErrors[i]);
    fprintf(out, "\n");
    for (i = 0; i < ret->nbClass; i++)
    {
        for (j = 0; j < ret->nbClass; j++)
            fprintf(out, "%d ", ret->confusionMatrix[i][j]);
        fprintf(out, "\n");
    }
}

void                empty_statistics(statistics *ret)
{
    int             i, j;
    
    ret->error = 0.0f;
    ret->nbErrors = 0;
    ret->combo = 0;
    for (i = 0; i < ret->nbClass; i++)
        ret->classesErrors[i] = 0;
    for (i = 0; i < ret->nbClass; i++)
        for (j = 0; j < ret->nbClass; j++)
            ret->confusionMatrix[i][j] = 0;
}

void                free_statistics(statistics *ret)
{
    int             i;
    
    free(ret->classesErrors);
    for (i = 0; i < ret->nbClass; i++)
        free(ret->confusionMatrix[i]);
    free(ret->confusionMatrix);
}
