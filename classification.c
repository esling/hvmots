/*
 *  classification.c
 *  HV-MOTS Classification
 *
 *	This file contains the main functions for classification of a time series dataset
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <sys/stat.h>
#include "transforms.h"
#include "statistics.h"
#include "classification.h"
#include "distances.h"
#include "export.h"
#include "hypervolume.h"
#include "optimize.h"
#include "pareto.h"
#include "import.h"
#include "types.h"
#include "main.h"

/*
 * @brief		Init the list of classification criteria to compute
 *
 * This function inits and fill a list of classification criteria that will be computed.
 * Each of the criteria is defined by a name and a function pointer to perform classification.
 * The computation state is set for all criteria based on the input parameter
 *
 * @param       int         State of computation for all criteria (compute or not)
 * @return		criteria*   List of classification criteria to apply
 *
 */
criteria                *init_criteria(int compute)
{
    criteria            *ret;
    int                 i;
    
    ret = malloc(sizeof(criteria));
    ret->nb_criteria = 6;
    ret->name = malloc(ret->nb_criteria * sizeof(char *));
    for (i = 0; i < ret->nb_criteria; i++)
        ret->name[i] = calloc(32, sizeof(char));
    ret->compute = calloc(ret->nb_criteria, sizeof(int));
    for (i = 0; i < ret->nb_criteria; i++)
        ret->compute[i] = compute;
    ret->functions = calloc(ret->nb_criteria, sizeof(int (*)(float **, int *, int, int, int, int)));
    sprintf(ret->name[0], "1-NN");
    ret->functions[0] = criteria_1NN;
    sprintf(ret->name[1], "5-NN");
    ret->functions[1] = criteria_5NN;
    sprintf(ret->name[2], "NC");
    ret->functions[2] = criteria_NC;
    sprintf(ret->name[3], "MOTS");
    ret->functions[3] = criteria_MOTS;
    sprintf(ret->name[4], "NP-MOTS");
    ret->functions[4] = criteria_NPMOTS;
    sprintf(ret->name[5], "HV-MOTS");
    ret->functions[5] = criteria_HVMOTS;
    return ret;
}

/*
 * @brief		Main function to classify a collection (list of datasets)
 *
 * This function is the main call of the classification system. It takes a set of parameters and launch computations.
 *
 * @param       mParameter* Global parameters to apply for the system
 *
 */
void                    classifyCollection(mParameters *param)
{
    collection          *datasets;
    dataset             *current, *test;
    distances           *distanceSet;
    criteria            *criterias;
    char                *directories;
    int                 i;
    
    datasets = importCollection(param->collection, 0);
    if (datasets == NULL)
        return;
    criterias = init_criteria(1);
    distanceSet = init_distances(1);
    directories = calloc(1024, sizeof(char));
    for (i = 0; i < datasets->size; i++)
    {
        current = NULL;
        printf("Processing dataset : %s - %s\n", datasets->name[i], datasets->path[i]);
        // Preparing output directories
        sprintf(directories, "%s/%s", param->output, datasets->name[i]);
        umask(0); mkdir(directories, 0777);
        sprintf(directories, "%s/%s/distances", param->output, datasets->name[i]);
        umask(0); mkdir(directories, 0777);
        sprintf(directories, "%s/%s/optimize_distance", param->output, datasets->name[i]);
        umask(0); mkdir(directories, 0777);
        sprintf(directories, "%s/%s/optimize_criteria", param->output, datasets->name[i]);
        umask(0); mkdir(directories, 0777);
        sprintf(directories, "%s/%s/results", param->output, datasets->name[i]);
        umask(0); mkdir(directories, 0777);
        if (datasets->type[i] == TYPE_LOO || datasets->type[i] == TYPE_TRAIN)
            current = importRaw(datasets->path[i], datasets->name[i], NULL, 0);
        if (datasets->type[i] == TYPE_LOO_MULTI || datasets->type[i] == TYPE_TRAIN_MULTI)
            current = importRawMultiple(datasets->path[i], datasets->name[i]);
        if (current == NULL) { printf("Skipping.\n"); continue; }
        if (datasets->type[i] == TYPE_LOO)
            classifyDatasetLOO(current, distanceSet, criterias);
        if (datasets->type[i] == TYPE_LOO_MULTI)
            classifyDatasetLOO(current, distanceSet, criterias);
        if (datasets->type[i] == TYPE_TRAIN)
        {
            test = importRaw(datasets->path_test[i], datasets->name[i], current->class_vals, current->nb_classes);
            if (test == NULL) { printf("Skipping.\n"); continue; }
            optimizeDatasetTT(current, distanceSet, criterias, param);
            classifyDatasetTT(current, test, distanceSet, criterias, param);
        }
        freeDataset(current, datasets->type[i]);
    }
    free(directories);
}

void                    normalize_distance(float **data, int nobj, int rows)
{
    float              *vector;
    int                 n, r;
    
    vector = malloc(nobj * sizeof(float));
    for (n = 0; n < nobj; n++)
        vector[n] = (float)data[0][n];
    for (r = 1; r < rows; r++)
        for (n = 0; n < nobj; n++)
            if (vector[n] < data[r][n])
                vector[n] = data[r][n];
    for (r = 0; r < rows; r++)
        for (n = 0; n < nobj; n++)
            data[r][n] /= vector[n];
    free(vector);
}

void                    classifyDatasetLOO(dataset *current, distances *distanceSet, criteria *criterias)
{
    int                 t1, t2, i;
    float               **distSet, *scores;
    int                 classFound;
    series              *ts1, *ts2;
    
    distSet = malloc(current->cardinality * sizeof(float *));
    scores = calloc(criterias->nb_criteria, sizeof(float));
    for (t1 = 0; t1 < current->cardinality; t1++)
    {
        for (t2 = 0; t2 < current->cardinality; t2++)
        {
            ts1 = current->data[t1];
            ts2 = current->data[t2];
            distSet[t2] = compute_distances(ts1, ts2, distanceSet);
        }
        normalize_distance(distSet, distanceSet->nb_compute, current->cardinality);
        for (i = 0; i < criterias->nb_criteria; i++)
            if (criterias->compute[i])
            {
                classFound = criterias->functions[i](distSet, current->classes, current->cardinality, distanceSet->nb_compute, t1, current->nb_classes);
                if (classFound != current->classes[t1])
                    scores[i]++;
            }
    }
    printf("Global results :\n");
    for (i = 0; i < criterias->nb_criteria; i++)
        printf("%s \t : %f\n", criterias->name[i], (scores[i] / (float)current->cardinality));
    free(scores);
    free(distSet);
    return;
}

void                    classifyDatasetTT(dataset *train, dataset *test, distances *distanceSet, criteria *criterias, mParameters *param)
{
    char                *outputStr;
    FILE                *distFile, **outputFile;
    int                 t1, t2, i, k, c, l = 0;
    float               ***distSet, **tmpSet;
    float               *bestScores, *spaceScores;
    int                 *trueCompute, *curCompute;
    int                 classFound, nbDist, nbComb = 0;
    statistics          **criteriaStats, **bestStats, **spaceStats;
    series              *ts1, *ts2;
    
    distSet = malloc(test->cardinality * sizeof(float **));
    tmpSet = malloc(train->cardinality * sizeof(float *));
    trueCompute = distanceSet->compute;
    curCompute = calloc(distanceSet->nb_distances, sizeof(int));
    outputStr = calloc(1024, sizeof(char));
    outputFile = calloc(criterias->nb_criteria + 1, sizeof(FILE *));
    criteriaStats = calloc(criterias->nb_criteria, sizeof(statistics *));
    bestStats = calloc(criterias->nb_criteria, sizeof(statistics *));
    spaceStats = calloc(criterias->nb_criteria, sizeof(statistics *));
    bestScores = calloc(criterias->nb_criteria, sizeof(float));
    spaceScores = calloc(criterias->nb_criteria, sizeof(float));
    for (i = 0; i < criterias->nb_criteria; i++)
    {
        bestScores[i] = 1.0;
        spaceScores[i] = 1.0;
        criteriaStats[i] = init_statistics(train->nb_classes);
        sprintf(outputStr, "%s/%s/results/%s.txt", param->output, train->name, criterias->name[i]);
        outputFile[i] = fopen(outputStr, "w");
    }
    sprintf(outputStr, "%s/%s/results/globalResults.txt", param->output, train->name);
    outputFile[criterias->nb_criteria] = fopen(outputStr, "w");
    for (t1 = 0; t1 < test->cardinality; t1++)
    {
        distSet[t1] = calloc(train->cardinality, sizeof(float *));
        for (t2 = 0; t2 < train->cardinality; t2++)
            distSet[t1][t2] = calloc(distanceSet->nb_distances, sizeof(float));
        for (i = 0; i < distanceSet->nb_distances; i++)
            if (distanceSet->compute[i])
            {
                ts1 = test->data[t1];
                if (distanceSet->resample[i] != -1)
                    ts1 = transform_Resample(ts1, distanceSet->resample[i]);
                #pragma omp parallel for private(ts2)
                for (t2 = 0; t2 < train->cardinality; t2++)
                {
                    ts2 = train->data[t2];
                    if (distanceSet->resample[i] != -1)
                        ts2 = transform_Resample(ts2, distanceSet->resample[i]);
                    distSet[t1][t2][i] = distanceSet->functions[i](ts1, ts2, distanceSet->best[i]);
                    if (distanceSet->resample[i] != -1)
                        free(ts2);
                }
                if (distanceSet->resample[i] != -1)
                    free(ts1);
            }
    }
    for (i = 0; i < distanceSet->nb_distances; i++)
        if (distanceSet->compute[i])
        {
            sprintf(outputStr, "%s/%s/distances/test_%s.txt", param->output, train->name, distanceSet->name[i]);
            distFile = fopen(outputStr, "w");
            for (t1 = 0; t1 < test->cardinality; t1++)
            {
                for (t2 = 0; t2 < train->cardinality; t2++)
                    fprintf(distFile, "%f ", distSet[t1][t2][i]);
                fprintf(distFile, "\n");
            }
            fclose(distFile);
        }
    if (param->combineAll)
    {
        for (i = 0; i < distanceSet->nb_distances; i++)
            if (distanceSet->compute[i])
                nbComb++;
        nbComb = pow(2, nbComb);
    }
    for (c = 1; c < nbComb; c++)
    {
        for (k = c, l = 0, nbDist = 0; l < distanceSet->nb_distances; l++)
            if (trueCompute[l]) { curCompute[l] = (k & 0x1); k >>= 1; nbDist += curCompute[l];}
        for (i = 0; i < criterias->nb_criteria; i++)
            empty_statistics(criteriaStats[i]);
        for (t1 = 0; t1 < test->cardinality; t1++)
        {
            for (t2 = 0; t2 < train->cardinality; t2++)
            {
                tmpSet[t2] = calloc(nbDist, sizeof(float));
                for (l = 0, k = 0; l < distanceSet->nb_distances; l++)
                    if (curCompute[l])
                        tmpSet[t2][k++] = distSet[t1][t2][l];
            }
            normalize_distance(tmpSet, nbDist, train->cardinality);
            for (i = 0; i < criterias->nb_criteria; i++)
                if (criterias->compute[i])
                {
                    classFound = criterias->functions[i](tmpSet, train->classes, train->cardinality, nbDist, -1, train->nb_classes);
                    criteriaStats[i]->confusionMatrix[test->classes[t1]][classFound]++;
                    if (classFound != test->classes[t1])
                    {
                        criteriaStats[i]->classesErrors[test->classes[t1]]++;
                        criteriaStats[i]->nbErrors++;
                    }
                }
            for (t2 = 0; t2 < train->cardinality; t2++)
                free(tmpSet[t2]);
        }
        for (k = 0; k < (criterias->nb_criteria + 1); k++)
        {
            for (i = 0; i < distanceSet->nb_distances; i++)
                if (curCompute[i])
                    fprintf(outputFile[k], "%s ", distanceSet->name[i]);
            fprintf(outputFile[k], ":\n");
            if (k < criterias->nb_criteria)
            {
                criteriaStats[k]->error = (float)criteriaStats[k]->nbErrors / (float)test->cardinality;
                export_statistics(outputFile[k], criteriaStats[k]);
                if (criteriaStats[k]->error < bestScores[k])
                {
                    bestScores[k] = criteriaStats[k]->error;
                    if (bestStats[k] != NULL)
                        free(bestStats[k]);
                    bestStats[k] = duplicate_statistics(criteriaStats[k]);
                }
                if (c == criterias->bestSpaceID[k])
                {
                    spaceStats[k] = duplicate_statistics(criteriaStats[k]);
                    spaceScores[k] = criteriaStats[k]->error;
                }
                continue;
            }
            for (i = 0; i < criterias->nb_criteria; i++)
                fprintf(outputFile[k], "%f ", criteriaStats[i]->error);
            fprintf(outputFile[k], "\n");
        }
    }
    fprintf(outputFile[criterias->nb_criteria], "Best statistics :\n");
    for (i = 0; i < criterias->nb_criteria; i++)
        fprintf(outputFile[criterias->nb_criteria], "%f ", bestStats[i]->error);
    fprintf(outputFile[criterias->nb_criteria], "\n");
    fprintf(outputFile[criterias->nb_criteria], "Space statistics :\n");
    for (i = 0; i < criterias->nb_criteria; i++)
        fprintf(outputFile[criterias->nb_criteria], "%f ", spaceStats[i]->error);
    fprintf(outputFile[criterias->nb_criteria], "\n");
    for (i = 0; i < criterias->nb_criteria; i++)
    {
        fprintf(outputFile[i], "Best statistics :\n");
        export_statistics(outputFile[i], bestStats[i]);
        fprintf(outputFile[i], "Space statistics :\n");
        export_statistics(outputFile[i], spaceStats[i]);
        free_statistics(criteriaStats[i]);
        free_statistics(bestStats[i]);
        free_statistics(spaceStats[i]);
        fclose(outputFile[i]);
    }
    fclose(outputFile[i]);
    for (t1 = 0; t1 < test->cardinality; t1++)
    {
        for (t2 = 0; t2 < train->cardinality; t2++)
            free(distSet[t1][t2]);
        free(distSet[t1]);
    }
    free(bestScores);
    free(spaceScores);
    free(bestStats);
    free(spaceStats);
    free(criteriaStats);
    free(tmpSet);
    free(curCompute);
    free(outputStr);
    free(outputFile);
    free(distSet);
    return;
}

int                     criteria_1NN(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_class)
{
    float               curBest = FLT_MAX, curDist;
    int                 i, j, curClass = -1;
    
    for (i = 0; i < n_elt; i++)
    {
        if (i == cur)
            continue;
        for (curDist = 0, j = 0; j < n_feat; j++)
            curDist += pow(distances[i][j], 2);
        curDist = sqrt(curDist / n_feat);
        if (curDist < curBest)
        {
            curBest = curDist;
            curClass = classes[i];
        }
    }
    return curClass;
}

int                     criteria_5NN(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_class)
{
    float               *curBest, *classBest, curDist;
    int                 i, j, *curClass, bestClass = 0;
    
    curBest = calloc(5, sizeof(float));
    curClass = calloc(5, sizeof(int));
    for (i = 0; i < 5; i++)
        curBest[i] = FLT_MAX;
    for (i = 0; i < n_elt; i++)
    {
        if (i == cur)
            continue;
        for (curDist = 0, j = 0; j < n_feat; j++)
            curDist += pow(distances[i][j], 2);
        curDist = sqrt(curDist / n_feat);
        if (curDist > curBest[4])
            continue;
        for (j = 4; j >= 0; j--)
        {
            if (curDist > curBest[j])
                break;
            if (j < 4)
            {
                curBest[j+1] = curBest[j];
                curClass[j+1] = curClass[j];
            }
            curBest[j] = curDist;
            curClass[j] = classes[i];
        }
    }
    classBest = calloc(n_class, sizeof(float));
    for (i = 0; i < 5; i++)
        classBest[curClass[i]] += 1.0 / curBest[i];
    for (i = 0, curDist = -1; i < n_class; i++)
        if (classBest[i] > curDist)
        {
            curDist = classBest[i];
            bestClass = i;
        }
    free(curClass);
    free(curBest);
    free(classBest);
    return bestClass;
}

int                     criteria_NC(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_class)
{
    float               curNC, *curClassDist, curBest = FLT_MAX;
    int                 i, j, k, curClass = -1, curClassSize;
    
    curClassDist = calloc(n_feat, sizeof(float));
    for (i = 0; i < n_class; i++)
    {
        curClassSize = 0;
        for (k = 0; k < n_feat; k++)
            curClassDist[k] = 0;
        for (j = 0; j < n_elt; j++)
            if (classes[j] == i && (i != cur))
                for (k = 0, curClassSize++; k < n_feat; k++)
                    curClassDist[k] += distances[j][k];
        if (curClassSize == 0)
            continue;
        for (k = 0, curNC = 0; k < n_feat; k++)
            curNC += pow(curClassDist[k] / curClassSize, 2);
        curNC = sqrt(curNC / n_feat);
        if (curNC < curBest)
        {
            curBest = curNC;
            curClass = i;
        }
    }
    free(curClassDist);
    return curClass;
}

int                     criteria_MOTS(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_class)
{
    double              *data;
    float               *classDist, curDist;
    int                 i, j, k = 0, *front, bestClass = -1;
    
    data = malloc(n_feat * n_elt * sizeof(double));
    for (i = 0; i < n_elt; i++)
        for (j = 0; j < n_feat; j++)
        {
            if (i != cur) { data[k++] = (double)distances[i][j]; }
            else { data[k++] = FLT_MAX; }
        }
    classDist = calloc(n_class, sizeof(float));
    front = calloc(n_elt, sizeof(int));
	paretofront(front, data, n_elt, n_feat);
    for (i = 0; i < n_elt; i++)
        if (front[i] > 0)
        {
            for (j = 0, curDist = 0; j < n_feat; j++)
                if (i != cur)
                    curDist += pow(distances[i][j], 2);
            classDist[classes[i]] += (1.0 / sqrt(curDist / n_feat));
        }
    for (i = 0, curDist = -1; i < n_class; i++)
        if (classDist[i] > curDist)
        {
            curDist = classDist[i];
            bestClass = i;
        }
    free(data);
    free(front);
    free(classDist);
    return bestClass;
}

int                     criteria_NPMOTS(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_class)
{
    double              curBest = DBL_MAX, *curClassDist, curDist, tmpDist;
    int                 i, j, k, l, curClass = -1, curClassSize, *front, n_fr;
    
    for (i = 0; i < n_class; i++)
    {
        curClassSize = 0;
        curClassDist = NULL;
        for (j = 0, k = 0; j < n_elt; j++)
            if (classes[j] == i && (j != cur))
            {
                curClassDist = realloc(curClassDist, ((++curClassSize) * n_feat) * sizeof(double));
                for (l = 0; l < n_feat; l++)
                    curClassDist[k++] = (double)distances[j][l];
            }
        if (curClassSize == 0)
            continue;
        front = calloc(curClassSize, sizeof(int));
        paretofront(front, curClassDist, curClassSize, n_feat);
        for (curDist = 0, j = 0, n_fr = 0; j < curClassSize; j++)
            if (front[j] > 0)
            {
                for (tmpDist = 0, l = 0; l < n_feat; l++)
                    tmpDist += pow(curClassDist[(j * n_feat) + l], 2);
                curDist += (tmpDist / n_feat);
                n_fr++;
            }
        curDist = sqrt(curDist / n_fr);
        if (curDist < curBest)
        {
            curBest = curDist;
            curClass = i;
        }
        free(front);
        free(curClassDist);
    }
    return curClass;
}

double                  *data_maximum(double *data, int nobj, int rows)
{
    double              *vector;
    int                 n, r, k;
    
    vector = malloc(nobj * sizeof(double));
    for (k = 0; k < nobj; k++)
        vector[k] = data[k];
    for (r = 1 ; r < rows; r++)
        for (n = 0; n < nobj; n++, k++)
            if (vector[n] < data[k])
                vector[n] = data[k];
    return vector;
}

double                  hypervolume_compute(float **distances, int popsize, int nobj)
{
    double              *reference;     /* Reference vector */
    double              *data;          /* Reshaped vector */
    double              volume = 0;     /* Hypervolume computation */
    int                 i, j, k = 0;
    
    /* Transpose the objective matrice*/
    data = malloc(nobj * popsize * sizeof(double));
    for (i=0;i<popsize;i++)
        for (j=0;j<nobj;j++)
            data[k++] = (double)distances[i][j];
    reference = calloc(nobj, sizeof(double));
    for (i = 0; i < nobj; i++)
        reference[i] = 2;
    volume = fpli_hv(data, nobj, popsize, reference);
    free(data);
    free(reference);
    return volume;
}

int                     criteria_HVMOTS(float **distances, int *classes, int n_elt, int n_feat, int cur, int n_class)
{
    float               curBest = -1;
    float               curHV, **curClassDist;
    int                 i, j, curClass = -1, curClassSize;
    
    for (i = 0; i < n_class; i++)
    {
        curClassSize = 0;
        curClassDist = NULL;
        for (j = 0; j < n_elt; j++)
            if (classes[j] == i && (j != cur))
            {
                curClassDist = realloc(curClassDist, (++curClassSize) * sizeof(float *));
                curClassDist[curClassSize - 1] = distances[j];
            }
        if (curClassSize == 0)
            continue;
        curHV = hypervolume_compute(curClassDist, curClassSize, n_feat);
        if (curHV > curBest)
        {
            curBest = curHV;
            curClass = i;
        }
        free(curClassDist);
    }
    return curClass;
}