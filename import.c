/*
 *  import.c
 *  HV-MOTS Classification
 *
 *	This file contains the C import functions for several types of time series dataset. Currently implemented
 *		- Raw (Space/tab separated)
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#include "types.h"
#include "import.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * @brief		Import a time series dataset from a raw (tab-separated) file
 *
 * This function imports a set of time series from a text file. The format is considered to be for each series
 *   idClass t0 t1 t2 t3 ... tN
 *
 * @param		filename	String path to dataset
 * @return		dataset*	Dataset structure
 *
 */
dataset			*importRaw(char *filename, char *name, int *c_vals, int nb_class)
{
	FILE		*fID;
	dataset		*result;
	series		*curSeries;
	char		*curLine, *pch;
	float		base;
    int         i;
	
	// Try opening
	fID = fopen(filename, "r");
	if (fID == NULL) { printf("File %s not found\n", filename); return NULL; }
	result = calloc(1, sizeof(dataset));
    result->type = DATA_UNIVARIATE;
	result->data = NULL;
    result->classes = NULL;
    result->class_vals = NULL;
    result->nb_classes = 0;
    if (c_vals != NULL)
    {
        result->class_vals = c_vals;
        result->nb_classes = nb_class;
    }
	result->length = 0;
	result->cardinality = 0;
	result->path = calloc(strlen(filename) + 1, sizeof(char));
	memcpy(result->path, filename, strlen(filename) * sizeof(char));
	result->name = result->path;
	if (name != NULL)
	{
		result->name = calloc(strlen(name) + 1, sizeof(char));
		memcpy(result->name, name, strlen(name) * sizeof(char));
	}
	curLine = calloc(65536, sizeof(char));
	while (1)
	{
		if ((fscanf(fID, "%[^\n]\n", curLine) < 1) || feof(fID) || curLine == NULL)
			break;
		curSeries = calloc(1, sizeof(series));
		curSeries->id = result->cardinality++;
		curSeries->length = 0;
		curSeries->values = NULL;
		curSeries->mean = 0;
        curSeries->variance = 0;
		pch = strtok(curLine, " ");
		// First value is class index
		sscanf(pch, "%f", &base);
        for (i = 0; i < result->nb_classes; i++)
            if (result->class_vals[i] == (int)base)
                break;
        if (i == result->nb_classes)
        {
            result->nb_classes++;
            result->class_vals = realloc(result->class_vals, (result->nb_classes) * sizeof(int));
            result->class_vals[result->nb_classes - 1] = (int)base;
        }
		curSeries->class = i;
        result->classes = realloc(result->classes, result->cardinality * sizeof(int));
        result->classes[result->cardinality - 1] = curSeries->class;
		pch = strtok(NULL, " ");
		// Parse each values
		while (pch != NULL)
		{
			curSeries->values = realloc(curSeries->values, (curSeries->length + 1) * sizeof(float));
			sscanf(pch, "%f", &base);
			curSeries->values[curSeries->length++] = base;
			curSeries->mean += base;
			pch = strtok(NULL, " ");
		}
		curSeries->mean /= curSeries->length;
        for (i = 0; i < curSeries->length; i++)
            curSeries->variance += pow((curSeries->values[i] - curSeries->mean), 2);
        curSeries->variance /= curSeries->length;
        curSeries->variance = sqrtf(curSeries->variance);
        for (i = 0; i < curSeries->length; i++)
            curSeries->values[i] = (curSeries->values[i] - curSeries->mean) / curSeries->variance;
		result->data = realloc(result->data, result->cardinality * sizeof(series *));
		result->data[result->cardinality - 1] = curSeries;
		if (curSeries->length > result->length)
			result->length = curSeries->length;
	}
	free(curLine);
	return result;
}

/*
 * @brief		Import a time series dataset from a raw (tab-separated) file
 *
 * This function imports a set of time series from a text file. The format is considered to be for each series
 *   idClass t0 t1 t2 t3 ... tN
 *
 * @param		filename	String path to dataset
 * @return		dataset*	Dataset structure
 *
 */
dataset			*importRawMultiple(char *filename, char *name)
{
	FILE		*fID;
	dataset		*result;
	series		*curSeries;
	char		*curLine, *pch;
	float		base;
    int         i, tc;
	
	// Try opening
	fID = fopen(filename, "r");
	if (fID == NULL) { printf("File %s not found\n", filename); return NULL; }
	result = calloc(1, sizeof(dataset));
    result->type = DATA_MULTIVARIATE;
    result->classes = NULL;
    result->class_names = NULL;
    result->features = NULL;
    result->nb_features = 0;
    result->nb_classes = 0;
	result->length = 0;
	result->cardinality = 0;
    result->domain = calloc(64, sizeof(char));
	result->path = calloc(strlen(filename) + 1, sizeof(char));
	memcpy(result->path, filename, strlen(filename) * sizeof(char));
	result->name = result->path;
	if (name != NULL)
	{
		result->name = calloc(strlen(name) + 1, sizeof(char));
		memcpy(result->name, name, strlen(name) * sizeof(char));
	}
	curLine = calloc(65536, sizeof(char));
    fscanf(fID, "%s\n", result->name);
    fscanf(fID, "%s\n", result->domain);
    fscanf(fID, "%f\n", &(result->accuracy));
    fscanf(fID, "%[^\n]\n", curLine);
    pch = strtok(curLine, " ");
    // Parse each values
    while (pch != NULL)
    {
        result->class_names = realloc(result->class_names, (result->nb_classes + 1) * sizeof(char *));
        result->class_names[result->nb_classes] = calloc(strlen(pch) + 1, sizeof(char));
        memcpy(result->class_names[result->nb_classes++], pch, strlen(pch));
        pch = strtok(NULL, " ");
    }
    fscanf(fID, "%[^\n]\n", curLine);
    pch = strtok(curLine, " ");
    // Parse each values
    while (pch != NULL)
    {
        result->feat_names = realloc(result->feat_names, (result->nb_features + 1) * sizeof(char *));
        result->feat_names[result->nb_features] = calloc(strlen(pch) + 1, sizeof(char));
        memcpy(result->feat_names[result->nb_features++], pch, strlen(pch));
        pch = strtok(NULL, " ");
    }
	while (1)
	{
        if (fscanf(fID, "%d\n", &(tc)) < 1 || feof(fID))
            break;
        result->classes = realloc(result->classes, (result->cardinality + 1) * sizeof(int));
        result->classes[result->cardinality++] = tc;
		result->features = realloc(result->features, result->cardinality * sizeof(series **));
		result->features[result->cardinality - 1] = calloc(result->nb_features, sizeof(series *));
        for (i = 0; i < result->nb_features; i++)
        {
            if ((fscanf(fID, "%[^\n]\n", curLine) < 1) || curLine == NULL)
                break;
            curSeries = calloc(1, sizeof(series));
            curSeries->id = result->cardinality;
            curSeries->class = tc;
            curSeries->length = 0;
            curSeries->values = NULL;
            curSeries->mean = 0;
            pch = strtok(curLine, " ");
            // Parse each values
            while (pch != NULL)
            {
                curSeries->values = realloc(curSeries->values, (curSeries->length + 1) * sizeof(float));
                sscanf(pch, "%f", &base);
                curSeries->values[curSeries->length++] = base;
                curSeries->mean += base;
                pch = strtok(NULL, " ");
            }
            curSeries->mean /= curSeries->length;
            if (curSeries->length > result->length)
                result->length = curSeries->length;
            result->features[result->cardinality - 1][i] = curSeries;
        }
	}
	free(curLine);
	return result;
}

collection		*importCollection(char *filename, int loadData)
{
	FILE		*fID;
	collection	*result;
	char		*baseDir, *curName, *curType;
    char        *curPath, *curPathTest;
	
	// Try opening
	fID = fopen(filename, "r");
	if (fID == NULL) { printf("File %s not found\n", filename); return NULL; }
	result = calloc(1, sizeof(collection));
	result->size = 0;
	result->name = NULL;
	result->path = NULL;
    result->path_test = NULL;
	result->data = NULL;
    result->test = NULL;
	baseDir = calloc(4096, sizeof(char));
	curName = calloc(4096, sizeof(char));
    if (fscanf(fID, "%s\t%s\n", curName, baseDir) < 2 || feof(fID) || strcmp(curName, "Directory"))
    {
        printf("Badly formatted file - Base directory not specified (no Directory tag)\n");
        free(result); free(baseDir); free(curName);
        return NULL;
    }
	curType = calloc(4096, sizeof(char));
	curPath = calloc(4096, sizeof(char));
	curPathTest = calloc(4096, sizeof(char));
	while (1)
	{
		if ((fscanf(fID, "%s\t%s\t%s\t%s\n", curName, curType, curPath, curPathTest) < 4) || feof(fID))
			break;
		result->name = realloc(result->name, (result->size + 1) * sizeof(char *));
		result->path = realloc(result->path, (result->size + 1) * sizeof(char *));
		result->path_test = realloc(result->path_test, (result->size + 1) * sizeof(char *));
		result->data = realloc(result->data, (result->size + 1) * sizeof(dataset *));
		result->test = realloc(result->test, (result->size + 1) * sizeof(dataset *));
		result->type = realloc(result->type, (result->size + 1) * sizeof(int));
        result->type[result->size] = -1;
        if (strcmp(curType, "LOO") == 0)
            result->type[result->size] = TYPE_LOO;
        if (strcmp(curType, "TRAIN") == 0)
            result->type[result->size] = TYPE_TRAIN;
        if (strcmp(curType, "LOO_MULTI") == 0)
            result->type[result->size] = TYPE_LOO_MULTI;
        if (strcmp(curType, "TRAIN_MULTI") == 0)
            result->type[result->size] = TYPE_TRAIN_MULTI;
		result->name[result->size] = calloc(strlen(curName) + 1, sizeof(char));
		memcpy(result->name[result->size], curName, strlen(curName));
		result->path[result->size] = calloc(strlen(baseDir) + strlen(curPath) + 2, sizeof(char));
		sprintf(result->path[result->size], "%s/%s", baseDir, curPath);
		if (loadData)
        {
            if (result->type[result->size] == TYPE_LOO_MULTI || result->type[result->size] == TYPE_TRAIN_MULTI)
                result->data[result->size] = importRaw(curPath, curName, NULL, 0);
            else
                result->data[result->size] = importRawMultiple(curPath, curName);
        }
        result->path_test[result->size] = calloc(strlen(baseDir) + strlen(curPathTest) + 2, sizeof(char));
        if (strcmp(curPathTest, "-"))
        {
            sprintf(result->path_test[result->size], "%s/%s", baseDir, curPathTest);
            if (loadData)
                result->test[result->size] = importRaw(curPathTest, curName, result->data[result->size]->class_vals, result->data[result->size]->nb_classes);
        }
		result->size++;
	}
    free(baseDir);
    free(curPath);
    free(curType);
    free(curPathTest);
    free(curName);
	return result;
}

void			freeDataset(dataset *dSet, int type)
{
	int			i, k;
	
	free(dSet->name);
    free(dSet->path);
    free(dSet->classes);
    if (type == TYPE_LOO_MULTI || type == TYPE_TRAIN_MULTI)
    {
        free(dSet->domain);
        for (i = 0; i < dSet->nb_features; i++)
            free(dSet->feat_names[i]);
        free(dSet->feat_names);
        for (i = 0; i < dSet->nb_classes; i++)
            free(dSet->class_names[i]);
        free(dSet->class_names);
    }
	for (i = 0; i < dSet->cardinality; i++)
	{
        if (type == TYPE_LOO_MULTI || type == TYPE_TRAIN_MULTI)
        {
            for (k = 0; k < dSet->nb_features; k++)
            {
                free(dSet->features[i][k]->values);
                free(dSet->features[i][k]);
            }
            free(dSet->features[i]);
        }
        else
        {
            free(dSet->data[i]->values);
            free(dSet->data[i]);
        }
	}
    if (type == TYPE_LOO_MULTI || type == TYPE_TRAIN_MULTI)
        free(dSet->features);
    else
        free(dSet->data);
	free(dSet);
}