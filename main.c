/*
 *  main.c
 *  HV-MOTS Classification
 *
 *	Main file for testing the HV-MOTS Classifier
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <sys/stat.h>
#include "import.h"
#include "export.h"
#include "distances.h"
#include "classification.h"
#include "main.h"

#define			VERSION "v.0.3"

void			printHelp()
{
	printf("hvMots \t: Testing HV-MOTS Classification paradigm\n");
	printf("Authors \t: Philippe Esling (philippe.esling@ircam.fr) - Carlos Agon (agonc@ircam.fr)\n");
	printf("Version \t: %s\n\n", VERSION);
	printf("Explanation of parameters :\n");
	printf("---------------------------\n\n");
	printf("[...]\n\n");
	printf("\n");
}

mParameters		*createDefaultParameters()
{
	mParameters	*ret;
	
	ret = malloc(sizeof(mParameters));
	ret->paperMode				= 1;	/**< Paper mode specify full statistics computation */
	ret->debugMode				= 1;	/**< Debug mode specify complete export of every steps */
	ret->benchmarkMode			= 0;	/**< Benchmark mode allows to test every possible combinations */
	ret->parallelize			= 1;	/**< Do we require the computation to be parallel */
	ret->numThreads				= -1;	/**< How many threads do we require (-1 : Maximum) */
    ret->testResampling         = 0;    /**< Do we optimize resampling of the series */
    ret->combineAll             = 1;
    ret->folds                  = 10;
    ret->repeats                = 2;
    ret->distanceTuning         = "loo";
    ret->spaceTuning            = "loo";
    ret->collection             = NULL;
    ret->output                 = NULL;
    ret->full                   = NULL;
    ret->summary                = NULL;
	return ret;
}

void			matchParameters(mParameters *params, char *variable, char *val, int cmdLine)
{
	int			value;
	
	if (cmdLine && !strcmp(variable, "config")) { importConfiguration(params, val); return; }
    if (!strcmp(variable, "collection")) { params->collection = calloc(strlen(val) + 1, sizeof(char)); sprintf(params->collection, "%s", val); return; }
    if (!strcmp(variable, "output")) { params->output = calloc(strlen(val) + 1, sizeof(char)); sprintf(params->output, "%s", val); return; }
    if (!strcmp(variable, "distanceTuning")) { params->distanceTuning = calloc(strlen(val) + 1, sizeof(char)); sprintf(params->distanceTuning, "%s", val); return; }
    if (!strcmp(variable, "spaceTuning")) { params->spaceTuning = calloc(strlen(val) + 1, sizeof(char)); sprintf(params->spaceTuning, "%s", val); return; }
	sscanf(val, "%d", &value);
	if (!strcmp(variable, "benchmarkMode")) { params->benchmarkMode = value; return; }
	if (!strcmp(variable, "combineAll")) { params->combineAll = value; return; }
	if (!strcmp(variable, "debugMode")) { params->debugMode = value; return; }
	if (!strcmp(variable, "paperMode")) { params->paperMode = value; return; }
	if (!strcmp(variable, "parallelize")) { params->parallelize = value; return; }
    if (!strcmp(variable, "numThreads")) { params->numThreads = value; return; }
    if (!strcmp(variable, "testResampling")) { params->testResampling = value; return; }
    if (!strcmp(variable, "folds")) { params->folds = value; return; }
    if (!strcmp(variable, "repeats")) { params->repeats = value; return; }
	printf("  [Unrecognized option : %s]\n", variable);
}

int				importConfiguration(mParameters *params, char *configFile)
{
	FILE		*cFile;
	char		*variable, *val;
	int			value;
	
	cFile = fopen(configFile, "r");
	val = calloc(512, sizeof(char));
	variable = calloc(512, sizeof(char));
	if (cFile == NULL)
	{
		printf("Config file %s does not exists. Performing analysis with default parameters.\n", configFile);
        free(variable);
        free(val);
		return -1;
	}
	printf("- Importing configuration \t: %s\n", configFile);
	while (1)
	{
		value = fscanf(cFile, "%s = %s", variable, val);
		if (feof(cFile)) { break; }
		if (value < 2) { continue; }
		matchParameters(params, variable, val, 0);
	}
	free(val);
	free(variable);
	return 0;
}

int				parseCommandLine(mParameters *params, int argc, char **argv)
{
	int			curArg;
	
	for (curArg = 1; curArg < argc; curArg += 2)
		if (argv[curArg][0] == '-')
			matchParameters(params, argv[curArg] + sizeof(char), argv[curArg + 1], 1);
	return 0;
}

int				main(int argc, char *argv[])
{
    mParameters	*params;
    
	// Create a default parameter structure
	params = createDefaultParameters();
	// Start by parsing the command line
	parseCommandLine(params, argc, argv);
    // Set up the parallelization system
	if (params->parallelize)
	{
        if (params->numThreads == -1) {params->numThreads = omp_get_num_procs();}
            omp_set_num_threads(params->numThreads);
		printf("- Parallel computation on \t: %d processors\n", params->numThreads);
	}
    // Preparing output directories
	umask(0); mkdir(params->output, 0777);
    // Launch the main computation
	classifyCollection(params);
	return 0;
}