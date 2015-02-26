//
//  transforms.c
//  HV-MOTS-C
//
//  Created by Philippe Esling on 24/03/2014.
//  Copyright (c) 2014 Philippe Esling. All rights reserved.
//

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "transforms.h"
#include "fftw3.h"

#define             inf     65536

fftw_plan           *fftPlans;
float               cutPoints[19][20] = {{-inf, 0, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -0.43, 0.43,  inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -0.67, 0,     0.67,  inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -0.84, -0.25, 0.25,  0.84,  inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -0.97, -0.43, 0,     0.43,  0.97,  inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.07, -0.57, -0.18, 0.18,  0.57,  1.07,  inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.15, -0.67, -0.32, 0,     0.32,  0.67,  1.15,   inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.22, -0.76, -0.43, -0.14, 0.14,  0.43,  0.76,   1.22,  inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.28, -0.84, -0.52, -0.25, 0.,    0.25,  0.52,   0.84,  1.28,  inf, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.34, -0.91, -0.6,  -0.35, -0.11, 0.11,  0.35,   0.6,   0.91,  1.34, inf, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.38, -0.97, -0.67, -0.43, -0.21, 0,     0.21,   0.43,  0.67,  0.97, 1.38, inf, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.43, -1.02, -0.74, -0.5,  -0.29, -0.1,  0.1,    0.29,  0.5,   0.74, 1.02, 1.43, inf, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.47, -1.07, -0.79, -0.57, -0.37, -0.18, 0,      0.18,  0.37,  0.57, 0.79, 1.07, 1.47, inf, inf, inf, inf, inf, inf},
                    {-inf, -1.5,  -1.11, -0.84, -0.62, -0.43, -0.25, -0.08,  0.08,  0.25,  0.43, 0.62, 0.84, 1.11, 1.5,  inf, inf, inf, inf, inf},
                    {-inf, -1.53, -1.15, -0.89, -0.67, -0.49, -0.32, -0.16,  0,     0.16,  0.32, 0.49, 0.67, 0.89, 1.15, 1.53,  inf, inf, inf, inf},
                    {-inf, -1.56, -1.19, -0.93, -0.72, -0.54, -0.38, -0.22,  -0.07, 0.07,  0.22, 0.38, 0.54, 0.72, 0.93, 1.19, 1.56,  inf, inf, inf},
                    {-inf, -1.59, -1.22, -0.97, -0.76, -0.59, -0.43, -0.28,  -0.14, 0,     0.14, 0.28, 0.43, 0.59, 0.76, 0.97, 1.22, 1.59,  inf, inf},
                    {-inf, -1.62, -1.25, -1,    -0.8,  -0.63, -0.48, -0.34,  -0.2,  -0.07, 0.07, 0.2,  0.34, 0.48, 0.63, 0.8,  1,    1.25, 1.62, inf},
                    {-inf, -1.64, -1.28, -1.04, -0.84,  -0.67, -0.52, -0.39, -0.25, -0.13, 0,    0.13, 0.25, 0.39, 0.52, 0.67, 0.84, 1.04, 1.28, 1.64}};

double              fmod_one(double x)
{
    double          res;
    
	res = x - lrint(x);
	if (res < 0.0)
		return res + 1.0;
	return res;
}

/*
 * @brief           Transform a series into a resampled version
 *
 * This function takes a raw time series in input and constructs a resampled version of it based on a given target length
 * that should be obtained after resampling. The computation is based on a linear interpolation operator.
 *
 * @param           series*     Series to be resampled
 * @param           int         Target length of the resulting resampling
 * @return          series*     Resampled series with identical properties
 *
 */
series              *transform_Resample(series *raw, int targetLength)
{
    series          *result;
    double          last_ratio, src_ratio, input_index, rem;
    long            in_count, in_used ;
	long            out_count, out_gen ;
	float           last_value[1];
    
    result = calloc(1, sizeof(series));
    result->id = raw->id;
    result->length = targetLength;
    result->class = raw->class;
    result->values = calloc(targetLength, sizeof(float));
    last_value[0] = raw->values[0];
    in_count = raw->length;
	out_count = targetLength;
	in_used = 0; out_gen = 0 ;
	src_ratio = (double)targetLength / (double)raw->length;
    last_ratio = src_ratio;
	input_index = 0.0;
	/* Calculate samples before first sample in input array. */
	while (input_index < 1.0 && out_gen < out_count)
	{
		if (in_used + 1 * (1.0 + input_index) >= in_count)
			break;
		if (out_count > 0 && fabs(last_ratio - src_ratio) > 1e-20)
			src_ratio = last_ratio + out_gen * (src_ratio - last_ratio) / out_count;
		result->values[out_gen] = (float) (last_value[0] + input_index * (raw->values[0] - last_value[0]));
        out_gen++;
		input_index += 1.0 / src_ratio;
    }
	rem = fmod_one (input_index);
	in_used += lrint(input_index - rem);
	input_index = rem;
	/* Main processing loop. */
	while (out_gen < out_count && in_used + input_index < in_count)
	{
		if (out_count > 0 && fabs(last_ratio - src_ratio) > 1e-20)
			src_ratio = last_ratio + out_gen * (src_ratio - last_ratio) / out_count;
        result->values[out_gen] = (float) (raw->values[in_used] + input_index * (raw->values[in_used] - raw->values[in_used - 1]));
        out_gen++;
        /* Figure out the next index. */
		input_index += 1.0 / src_ratio;
		rem = fmod_one(input_index);
		in_used += lrint(input_index - rem);
		input_index = rem;
    }
    return result;
}

/*
 * @brief           Transform a series into its Auto-Correlation Function (ACF).
 *
 * This function takes a raw time series in input and returns its Auto-Correlation Function (ACF)
 * The computation is performed by using the FFT and then Inverse FFT of the input series based on the equation
 * C(m) = E[X(n+m)*conj(X(n))] = E[X(n)*conj(X(n-m))]
 *
 * @param           series*     Input series to be processed
 * @return          series*     Auto-Correlation Function (ACF) of the series.
 *
 */
series              *transform_ACF(series *raw)
{
    series          *result;
    float           mean = 0, var = 0;
    int             i, lag;
    
    for (i = 0; i < raw->length; i++)
        mean += raw->values[i];
    mean /= raw->length;
    for (i = 0; i < raw->length; i++)
        var += pow((raw->values[i] - mean), 2);
    var /= raw->length;
    result = calloc(1, sizeof(series));
    result->id = raw->id;
    result->length = raw->length;
    result->class = raw->class;
    result->values = calloc(result->length, sizeof(float));
    for (lag = 0; lag < result->length; lag++)
    {
        for (i = 0; i < (result->length - lag); i++)
            result->values[lag] += ((raw->values[i] - mean) * (raw->values[i + lag] - mean) / (result->length - lag));
        result->values[lag] /= var;
    }
    return result;
}

/*
 * OLD Version with FFT
 *
series              *transform_ACF(series *raw)
{
    series          *result;
    fftw_complex    *data, *fft_result;
    fftw_plan       plan_forward;
    int             fftSize, i;
    
    result = calloc(1, sizeof(series));
    result->id = raw->id;
    result->length = raw->length;
    result->class = raw->class;
    fftSize = pow(2, (int)log2(2 * raw->length - 1));
    result->values = calloc(fftSize / 2, sizeof(float));
    #pragma omp critical (plan)
    {
        data = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize );
        fft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
    }
    memset(data, 0, sizeof(fftw_complex) * fftSize);
    memset(fft_result, 0, sizeof(fftw_complex) * fftSize);
    #pragma omp critical (plan)
    plan_forward = fftw_plan_dft_1d(fftSize, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
    // Copy the chunk into our buffer
    for(i = 0; i < fftSize; i++)
    {
        if (i < raw->length) { data[i][0] = raw->values[i]; data[i][1] = 0.0; }
        else { data[i][0] = 0.0; data[i][1] = 0.0; }
    }
    // Perform the FFT
    fftw_execute(plan_forward);
    // Copy FFT result into the first data vector
    for (i = 0; i < fftSize / 2; i++)
    {
        data[i][0] = pow(fft_result[i][0], 2);
        data[i][1] = 0.0;
    }
//    fftw_destroy_plan(plan_forward);
    #pragma omp critical (plan)
    plan_forward = fftw_plan_dft_1d(fftSize, data, fft_result, FFTW_BACKWARD, FFTW_ESTIMATE);
    // Then perform the inverse FFT
    fftw_execute(plan_forward);
    // Copy correlation result into our result vector
    for (i = 0; i < fftSize / 2; i++)
        result->values[i] = fabs(fft_result[i][0]);
//    fftw_destroy_plan(plan_forward);
    #pragma omp critical (plan)
    {
        fftw_free(data);
        fftw_free(fft_result);
        fftw_cleanup();
    }
    return result;
}
 */

/*
 * @brief		Transform a collection of series into their Principal Components Analysis (PCA).
 *
 * TODO
 *
 * @param		series*     Series to be transformed
 * @return		series*     Resampled series with identical properties
 *
 */
series              *transform_PCA(series *raw)
{
    series          *result;
    
    result = calloc(1, sizeof(series));
    result->id = raw->id;
    result->length = raw->length;
    result->class = raw->class;
    return result;
}

/*
 * @brief		Transform a series into its Fourier Transform (FFT).
 *
 * This function takes a raw time series in input and returns its Fast Fourier Transform (FFT)
 *
 * @param		series*     Series to be transformed
 * @param       int         Size of the FFT to use
 * @return		series*     Fourier transform of the series
 *
 */
series              *transform_FFT(series *raw, int fftSize)
{
    series          *result;
    fftw_complex    *data, *fft_result;
    fftw_plan       plan_forward;
    int             i;
    
    result = calloc(1, sizeof(series));
    result->id = raw->id;
    result->length = fftSize;
    result->class = raw->class;
    result->values = calloc(fftSize, sizeof(float));
#pragma omp critical (plan)
    {
        data = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize );
        fft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
        if (fftPlans == NULL)
            fftPlans = calloc(8193, sizeof(fftw_plan));
        if (fftPlans[fftSize] == NULL)
            fftPlans[fftSize] = fftw_plan_dft_1d(fftSize, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
        plan_forward = fftPlans[fftSize];
    }
    memset(data, 0, sizeof(fftw_complex) * fftSize);
    memset(fft_result, 0, sizeof(fftw_complex) * fftSize);
    // Copy the chunk into our buffer
    for(i = 0; i < fftSize; i++)
    {
        if (i < raw->length) { data[i][0] = raw->values[i]; data[i][1] = 0.0; }
        else { data[i][0] = 0.0; data[i][1] = 0.0; }
    }
    // Perform the FFT on our chunk
    fftw_execute_dft(plan_forward, data, fft_result);
    // Copy FFT result into our new vector
    for (i = 0; i < fftSize / 2; i++)
        result->values[i] = fabs(fft_result[i][0]);
#pragma omp critical (plan)
    {
        fftw_free(data);
        fftw_free(fft_result);
        fftw_cleanup();
    }
    return result;
}

/*
 * @brief		Transform a series into the Symbolic Aggregate approXimation (SAX)
 *
 * This function takes a raw time series in input and returns its Symbolic Aggregate approXimation (SAX).
 * The transform requires a size of alphabet (alphaSize) and number of temporal bins (timeBins).
 *
 * @param		series*     Series to be transformed
 * @param       int         Size of the alphabet (cardinality)
 * @param       int         Number of temporal bins
 * @return		char*       SAX representation of the series
 *
 */
char                *transform_SAX(series *raw, int alphaSize, int timeBins)
{
    int             winSize, i, j;
    float           *symb, curValue;
    char            *result;
    
    if (alphaSize < 2 || alphaSize > 20) { alphaSize = 16; }
    if (timeBins == -1) { timeBins = 16; }
    result = calloc(timeBins + 1, sizeof(char));
    winSize = floor(raw->length / (float)timeBins);
    symb = cutPoints[alphaSize - 2];
    for (i = 0; i < timeBins; i++)
    {
        for (j = 0, curValue = 0; j < winSize; j++)
            curValue += raw->values[j + (i * winSize)];
        curValue /= winSize;
        for (j = 0; j < alphaSize; j++)
            if (curValue < symb[j])
                break;
        result[i] = (j - 1) + 'A';
    }
    return result;
}

