//
//  main.h
//  HV-MOTS-C
//
//  Created by Philippe Esling on 20/05/2014.
//  Copyright (c) 2014 Philippe Esling. All rights reserved.
//

#ifndef HV_MOTS_C_main_h
#define HV_MOTS_C_main_h

typedef struct		mParameters
{
	// Computing modes //
	
	int				paperMode;              /**< Paper mode specify full statistics computation */
	int				debugMode;              /**< Debug mode specify complete export of every steps */
	int				benchmarkMode;          /**< Benchmark mode allows to test every possible combinations */
	
	// Distributed computation //
	
	int				parallelize;            /**< Do we require the computation to be parallel */
	int				numThreads;             /**< How many threads do we require (-1 : Maximum) */
	
	// Datasets parameters //
    
    char            *collection;        /**< The input collection file */
    
    // Computation options //
    
    int             combineAll;
    
    // Output files //
    
    char            *output;
    FILE            *summary;
    FILE            *full;
    
    // Tuning parameters //
    
    int             testResampling;
    int             folds;
    int             repeats;
    char            *distanceTuning;
    char            *spaceTuning;
    
} mParameters;

int                 importConfiguration(mParameters *params, char *configFile);

#endif
