/*
 *  export.c
 *  HV-MOTS Classification
 *
 *	This file contains the C export functions for several types of time series dataset. Currently implemented
 *		- Console output
 *		- Raw (Space/tab separated)
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#include "import.h"
#include "export.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * @brief		Export a time series dataset to the console
 *
 * This function prints a dataset of time series to the main (console) output.
 *
 * @param		result		Dataset structure
 *
 */
void			exportConsole(dataset *result)
{
	series		*curSeries;
	int			i, j;
	
	printf("Dataset %s\n", result->name);
	printf("  * Path \t: %s\n", result->path);
	printf("  * Size \t: %d\n", result->cardinality);
	printf("  * Length \t: %d\n", result->length);
	for (i = 0; i < result->cardinality; i++)
	{
		curSeries = result->data[i];
		printf("Series n.%d [%d pts] - Class %d\n", curSeries->id, curSeries->length, curSeries->class);
		printf(" - Mean \t: %f\n", curSeries->mean);
		printf(" - Variance \t: %f\n", curSeries->variance);
		for (j = 0; j < curSeries->length; j++)
			printf("%f ", curSeries->values[j]);
		printf("\n");
	}
}

/*
 * @brief		Export a multivariate time series dataset to the console
 *
 * This function prints a dataset of time series to the main (console) output.
 *
 * @param		result		Dataset structure
 *
 */
void			exportConsoleMultiple(dataset *result)
{
	series		*curSeries;
	int			i, j, k;
	
	printf("Dataset %s\n", result->name);
	printf("  * Path \t: %s\n", result->path);
	printf("  * Size \t: %d\n", result->cardinality);
	printf("  * Length \t: %d\n", result->length);
    printf("  * Classes \t: %d\n", result->nb_classes);
    for (i = 0; i < result->nb_classes; i++)
        printf("%s ", result->class_names[i]);
    printf("\n");
    printf("  * Features \t: %d\n", result->nb_features);
    for (i = 0; i < result->nb_features; i++)
        printf("%s ", result->feat_names[i]);
    printf("\n");
	for (i = 0; i < result->cardinality; i++)
	{
        for (k = 0; k < result->nb_features; k++)
        {
            curSeries = result->features[i][k];
            printf("Series n.%d [%d pts] - Class %d\n", curSeries->id, curSeries->length, curSeries->class);
            printf(" - Mean \t: %f\n", curSeries->mean);
            printf(" - Variance \t: %f\n", curSeries->variance);
            for (j = 0; j < curSeries->length; j++)
                printf("%f ", curSeries->values[j]);
            printf("\n");
        }
	}
}