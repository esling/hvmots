/*
 *  types.h
 *  HV-MOTS Classification
 *
 *	This file contains the header definitions for all types.
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#ifndef TYPES_H_
# define TYPES_H_

# define        TYPE_LOO            0
# define        TYPE_TRAIN          1
# define        TYPE_LOO_MULTI      2
# define        TYPE_TRAIN_MULTI    3
# define        DATA_UNIVARIATE     4
# define        DATA_MULTIVARIATE   5

# define        PARAM_INT           0
# define        PARAM_FLOAT         1
# define        PARAM_DOUBLE        2
# define        PARAM_STRING        3

typedef struct	series_t
{
	int			id;
	int			length;
	int			class;
	float		*values;
	float		*times;
	float		mean;
	float		variance;
} series;

typedef struct	dataset_t
{
    int         type;
	char		*name;
	char		*path;
    char        *domain;
    char        **class_names;
    char        **feat_names;
	int			cardinality;
    float       accuracy;
	int			length;
    int         nb_features;
    int         nb_classes;
    int         *classes;
    series      ***features;
	series		**data;
    int         *class_vals;
} dataset;

typedef struct	collection_t
{
	int			size;
    int         *type;
	char		**name;
	char		**path;
    char        **path_test;
	dataset		**data;
    dataset     **test;
} collection;

typedef struct  parameter_t
{
    char        *name;
    int         type;
    float       min;
    float       max;
    float       step;
    float       best;
    float       error;
    int         opt_size;
    float       *optimized;
} parameter;

typedef struct  distances_t
{
    char        **name;
    int         nb_compute;
    int         nb_distances;
    float       (**functions)(series *, series *, float *);
    parameter   ***params;
    float       **best;
    int         *resample;
    int         *compute;
    int         *args;
} distances;

typedef struct  criteria_t
{
    char        **name;
    int         nb_compute;
    int         nb_criteria;
    int         (**functions)(float **, int *, int, int, int, int);
    int         *bestSpaceID;
    int         **bestSpace;
    int         *compute;
    int         *nbBest;
    int         *args;
} criteria;

#endif