/*
 *  distances.c
 *  HV-MOTS Classification
 *
 *	This file contains the C implementation of several time series distances. Currently implemented :
 *		- L1-Norm
 *		- Euclidean
 *		- Lp-Norms
 *		- LI-Norm
 *		- Longest Common SubSequence (LCSS)
 *		- Edit Distance on Real values (EDR)
 *		- Edit with Real Penalty (ERP)
 *
 *  Parts of this code has been re-adapted from Ding et. al "Benchmarking ..." 2008
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#include <omp.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include "distances.h"
#include "transforms.h"
#include "types.h"
#include "main.h"
#include "zlib.h"

#define CHUNK   65536

#ifndef min
#define min(x, y)	((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define	max(x, y)	((x > y) ? (x) : (y))
#endif
#ifndef euc
#define euc(x, y)   sqrt(pow(x - y, 2))
#endif

/*
 * @brief		Compute the L1 distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @return		float	L1 distance
 *
 */
float			dist_L1(series *ts1, series *ts2)
{
	float		res;
	int			i, len;
	
	len = ts1->length;
	if (ts1->length != ts2->length)
	{
		printf("Distances [L1] : Warning - Series not of equal size\n");
		len = min(ts1->length, ts2->length);
	}
	for (i = 0, res = 0; i < len; i++)
		res += fabsf(ts1->values[i] - ts2->values[i]);
	return res;
}

/*
 * @brief		Compute the Euclidean (L2) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @return		float	Euclidean distance
 *
 */
float			dist_Euclidean(series *ts1, series *ts2)
{
	float		res;
	int			i, len;
	
	len = ts1->length;
	if (ts1->length != ts2->length)
	{
		printf("Distances [Euclidean] : Warning - Series not of equal size\n");
		len = min(ts1->length, ts2->length);
	}
	for (i = 0, res = 0; i < len; i++)
		res += pow(ts1->values[i] - ts2->values[i], 2);
	return sqrt(res);
}


/*
 * @brief		Compute the Lp norm for any value of p between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @return		float	Lp distance
 *
 */
float			dist_Lp(series *ts1, series *ts2, float p)
{
	float		res;
	int			i, len;
	
	len = ts1->length;
	if (ts1->length != ts2->length)
	{
		printf("Distances [LP] : Warning - Series not of equal size\n");
		len = min(ts1->length, ts2->length);
	}
	for (i = 0, res = 0; i < len; i++)
		res += pow(fabsf(ts1->values[i] - ts2->values[i]), p);
	return pow(res, 1.0f / p);
}

/*
 * @brief		Compute the LI norm between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @return		float	LI distance
 *
 */
float			dist_LI(series *ts1, series *ts2)
{
	float		res, tmp;
	int			i, len;
	
	len = ts1->length;
	if (ts1->length != ts2->length)
	{
		printf("Distances [LI] : Warning - Series not of equal size\n");
		len = min(ts1->length, ts2->length);
	}
	for (i = 0, res = 0, tmp = 0; i < len; i++, res = (tmp > res ? tmp : res))
		tmp = fabsf(ts1->values[i] - ts2->values[i]);
	return res;
}

/*
 * @brief		Compute the Longest Common SubSequence (LCSS) norm between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		warp	Size of the warping window (default (-1) : INT_MAX)
 * @param		thresh	Threshold for considering a match between points (default (-1) : 0.24288682362)
 * @return		float	LCSS distance
 *
 */
float			dist_LCSS(series *ts1, series *ts2, float warp, float thresh)
{
	float		**df, res;
	int			i, j, m, n;
	
	// Use default values if -1 is specified
	if (warp == -1) { warp = 1.0; }
	if (thresh == -1) { thresh = 0.24288682362; }		
	m = ts1->length; n = ts2->length;
    warp *= ts1->length;
	df = calloc(m + 1, sizeof(float *));
	for (i = 0; i < m + 1; i++)
		df[i] = calloc(n + 1, sizeof(float));
	for (i = 1; i < m + 1; i++)
		for (j = 1; j < n + 1; j++)
		{
			if ((abs(i - j) < warp) && (fabsf(ts1->values[i - 1] - ts2->values[j - 1]) < thresh))
				df[i][j] = df[i - 1][j - 1] + 1;
			else
			{
				if (df[i - 1][j] >= df[i][j - 1])
					df[i][j] = df[i - 1][j];
				else
					df[i][j] = df[i][j - 1];
			}
		}
	res = 1.0 - (df[m][n] / min(m, n));
	for (i = 0; i < m + 1; i++)
		free(df[i]);
	free(df);
	return res;
}

/*
 * @brief		Compute the Edit distance with Real Penalty (ERP) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		gap		Gap penalty for matching
 * @return		float	ERP distance
 *
 */
float			dist_ERP(series *ts1, series *ts2, float gap)
{
	float		**df, res;
	float		diffH, diffV, diffD;
	float		s1, s2, s3, p1, p2;
	int			i, j, k, m, n;
	
	// Use default values if -1 is specified
	if (gap == -1) { gap = 0.0; }
	m = ts1->length; n = ts2->length;
	df = calloc(n + 1, sizeof(float *));
	for (i = 0; i <= n; i++)
		df[i] = calloc(m + 1, sizeof(float));
	for (i = 1; i <= m; i++)
		for (k = 0; k < i; k++)
			df[0][i] = df[0][i] - sqrt(pow(ts1->values[k] - gap, 2));
	for (j = 1; j <= n; j++)
		for (k = 0; k < j; k++)
			df[j][0] = df[j][0] - sqrt(pow(ts2->values[k] - gap, 2));
	for (i = 1; i <= m; i++)
		for (j = 1; j <= n; j++)
		{
			p1 = ts1->values[i - 1]; 
			p2 = ts2->values[j - 1];
			diffD = sqrt(pow(p1 - p2, 2));
			diffH = sqrt(pow(p1 - gap, 2));
			diffV = sqrt(pow(p2 - gap, 2));
			s1 = df[j - 1][i - 1] - diffD;
			s2 = df[j - 1][i] - diffV;
			s3 = df[j][i - 1] - diffH;
			df[j][i] = max(max(s1, s2), s3);
		}
	res = -df[n][m];
	for (i = 0; i < n + 1; i++)
		free(df[i]);
	free(df);
	return res;
}

/*
 * @brief		Compute the Edit Distance on Real values (EDR) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		warp	Size of the warping window (default (-1) : INT_MAX)
 * @param		thresh	Threshold for considering a match between points (default (-1) : 0.24288682362)
 * @return		float	EDR distance
 *
 */
float			dist_EDR(series *ts1, series *ts2, float warp, float thresh, float hit, float miss, float gap)
{
	float		**df, res;
	float		s1, s2, s3, p1, p2;
	int			i, j, m, n;
	
	// Use default values if -1 is specified
	if (warp == -1) { warp = FLT_MAX; }
	if (thresh == -1) { thresh = 0.24288682362; }
    if (miss == -1) { miss = 1; }
    if (gap == -1) { gap = 1; }
	m = ts1->length + 1; n = ts2->length + 1;
    warp *= ts1->length;
	df = calloc(n, sizeof(float *));
	for (i = 0; i < n; i++)
	{
		df[i] = calloc(m, sizeof(float));
		df[i][0] = -i;
	}
	for (i = 1; i < m; i++)
		df[0][i] = -i;
	for (i = 1; i < m; i++)
		for (j = 1; j < n; j++)
		{
			p1 = ts1->values[i - 1]; 
			p2 = ts2->values[j - 1];
			if ((abs(i - j) <= warp) && (fabsf(p1 - p2) <= thresh))
                s1 = hit;
            else
                s1 = miss;
			s1 = df[j - 1][i - 1] + s1;
			s2 = df[j - 1][i] + gap;
			s3 = df[j][i - 1] + gap;
			df[j][i] = max(s1, s2);
            df[j][i] = max(df[j][i], s3);
		}
	res = -df[n - 1][m - 1];
	for (i = 0; i < n; i++)
		free(df[i]);
	free(df);
	return res;
}

/*
 * @brief		Compute the Dynamic Time Warping (DTW) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		warp	Size of the warping window (default (-1) : 10% of ts1)
 * @param		window	Dynamic windowing constraints (to impose Itakura, Sakoe-Chiba or purely dynamic constraints)
 * @return		float	DTW distance
 *
 */
float			dist_DTW(series *ts1, series *ts2, float warp, int **window)
{
	float		**df, res;
	float		min1, min2;
	int			i, j, m, n;
	
	// Use default values if -1 is specified
	if (warp == -1) { warp = 0.1f; }
	m = ts1->length; n = ts2->length;
    warp *= ts1->length;
	df = calloc(m, sizeof(float *));
	for (i = 0; i < m; i++)
		df[i] = calloc(n, sizeof(float));
	df[0][0] = sqrt(pow(ts1->values[0] - ts2->values[0], 2));
	for (i = 1; i < m; i++)
		df[i][0] = df[i - 1][0] + sqrt(pow(ts1->values[i] - ts2->values[0], 2));
	for (j = 1; j < n; j++)
		df[0][j] = df[0][j - 1] + sqrt(pow(ts1->values[0] - ts2->values[j], 2));
	for (i = 1; i < m; i++)
		for (j = 1; j < n; j++)
		{
			if ((abs(i - j) <= warp) || (window != NULL && window[i][j]))
			{
				min1 = min(df[i][j - 1], df[i - 1][j]);
				min2 = min(min1, df[i - 1][j - 1]);
				df[i][j] = min2 + pow(ts1->values[i] - ts2->values[j], 2);
			}
			else 
				df[i][j] = INT_MAX;
		}
	res = sqrt(df[m - 1][n - 1]);
	for (i = 0; i < m; i++)
		free(df[i]);
	free(df);
	return res;
}

/*
 * @brief		Compute the Derivative Dynamic Time Warping (DTW) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		warp	Size of the warping window (default (-1) : 10% of ts1)
 * @param		window	Dynamic windowing constraints (to impose Itakura, Sakoe-Chiba or purely dynamic constraints)
 * @return		float	DTW distance
 *
 */
float			dist_DDTW(series *ts1, series *ts2, float warp, int **window)
{
    float		**df, res;
    float       *ds1, *ds2;
    float		min1, min2;
    int			i, j, m, n;
    
    // Use default values if -1 is specified
    if (warp == -1) { warp = 0.1f; }
    m = ts1->length; n = ts2->length;
    warp *= ts1->length;
    ds1 = calloc(m, sizeof(float));
    ds2 = calloc(n, sizeof(float));
    for (i = 0; i < m; i++)
        ds1[i] = (i > 0 && (i < (m - 1))) ? (((ts1->values[i] - ts1->values[i - 1]) + ((ts1->values[i + 1] - ts1->values[i - 1]) / 2)) / 2) : ts1->values[i];
    for (i = 0; i < n; i++)
        ds2[i] = (i > 0 && (i < (n - 1))) ? (((ts2->values[i] - ts2->values[i - 1]) + ((ts2->values[i + 1] - ts2->values[i - 1]) / 2)) / 2) : ts2->values[i];
    df = calloc(m, sizeof(float *));
    for (i = 0; i < m; i++)
        df[i] = calloc(n, sizeof(float));
    df[0][0] = sqrt(pow(ds1[0] - ds2[0], 2));
    for (i = 1; i < m; i++)
        df[i][0] = df[i - 1][0] + sqrt(pow(ds1[i] - ds2[0], 2));
    for (j = 1; j < n; j++)
        df[0][j] = df[0][j - 1] + sqrt(pow(ds1[0] - ds2[j], 2));
    for (i = 1; i < m; i++)
        for (j = 1; j < n; j++)
        {
            if ((abs(i - j) <= warp) || (window != NULL && window[i][j]))
            {
                min1 = min(df[i][j - 1], df[i - 1][j]);
                min2 = min(min1, df[i - 1][j - 1]);
                df[i][j] = min2 + pow(ds1[i] - ds2[j], 2);
            }
            else
                df[i][j] = INT_MAX;
        }
    res = sqrt(df[m - 1][n - 1]);
    for (i = 0; i < m; i++)
        free(df[i]);
    free(df);
    free(ds1);
    free(ds2);
    return res;
}

/*
 * @brief		Compute the Weighted Dynamic Time Warping (DTW) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		penalty	Penalty g applied in the weighted scheme (following logistic function)
 * @return		float	WDTW distance
 *
 */
float			dist_WDTW(series *ts1, series *ts2, float penalty)
{
    float		**df, res;
    float		min1, min2, warp = 0.3;
    int			i, j, m, n;
    
    // Use default values if -1 is specified
    if (penalty == -1) { penalty = 0.05f; }
    m = ts1->length; n = ts2->length;
    warp *= ts1->length;
    df = calloc(m, sizeof(float *));
    for (i = 0; i < m; i++)
        df[i] = calloc(n, sizeof(float));
    df[0][0] = sqrt(pow(ts1->values[0] - ts2->values[0], 2));
    for (i = 1; i < m; i++)
        df[i][0] = df[i - 1][0] + sqrt((1 / (1 + exp(-penalty * (i - m / 2)))) * pow(ts1->values[i] - ts2->values[0], 2));
    for (j = 1; j < n; j++)
        df[0][j] = df[0][j - 1] + sqrt((1 / (1 + exp(-penalty * (i - n / 2)))) * pow(ts1->values[0] - ts2->values[j], 2));
    for (i = 1; i < m; i++)
        for (j = 1; j < n; j++)
        {
            if (abs(i - j) <= warp)
            {
                min1 = min(df[i][j - 1], df[i - 1][j]);
                min2 = min(min1, df[i - 1][j - 1]);
                df[i][j] = min2 + ((1 / (1 + exp(-penalty * (abs(i - j) - m / 2)))) * pow(ts1->values[i] - ts2->values[j], 2));
            }
            else
                df[i][j] = INT_MAX;
        }
    res = sqrt(df[m - 1][n - 1]);
    for (i = 0; i < m; i++)
        free(df[i]);
    free(df);
    return res;
}

/*
 * @brief		Compute the Weighted Derivative Dynamic Time Warping (DTW) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		penalty	Penalty g applied in the weighted scheme (following logistic function)
 * @return		float	WDDTW distance
 *
 */
float			dist_WDDTW(series *ts1, series *ts2, float penalty)
{
    float		**df, res;
    float       *ds1, *ds2;
    float		min1, min2, warp = 0.3;
    int			i, j, m, n;
    
    // Use default values if -1 is specified
    if (penalty == -1) { penalty = 0.05f; }
    m = ts1->length; n = ts2->length;
    warp *= ts1->length;
    df = calloc(m, sizeof(float *));
    ds1 = calloc(m, sizeof(float));
    ds2 = calloc(n, sizeof(float));
    for (i = 0; i < m; i++)
        ds1[i] = (i > 0 && (i < (m - 1))) ? (((ts1->values[i] - ts1->values[i - 1]) + ((ts1->values[i + 1] - ts1->values[i - 1]) / 2)) / 2) : ts1->values[i];
    for (i = 0; i < n; i++)
        ds2[i] = (i > 0 && (i < (n - 1))) ? (((ts2->values[i] - ts2->values[i - 1]) + ((ts2->values[i + 1] - ts2->values[i - 1]) / 2)) / 2) : ts2->values[i];
    for (i = 0; i < m; i++)
        df[i] = calloc(n, sizeof(float));
    df[0][0] = sqrt(pow(ds1[0] - ds2[0], 2));
    for (i = 1; i < m; i++)
        df[i][0] = df[i - 1][0] + sqrt((1 / (1 + exp(-penalty * (i - m / 2)))) * pow(ds1[i] - ds2[0], 2));
    for (j = 1; j < n; j++)
        df[0][j] = df[0][j - 1] + sqrt((1 / (1 + exp(-penalty * (i - n / 2)))) * pow(ds1[0] - ds2[j], 2));
    for (i = 1; i < m; i++)
        for (j = 1; j < n; j++)
        {
            if (abs(i - j) <= warp)
            {
                min1 = min(df[i][j - 1], df[i - 1][j]);
                min2 = min(min1, df[i - 1][j - 1]);
                df[i][j] = min2 + ((1 / (1 + exp(-penalty * (abs(i - j) - m / 2)))) * pow(ds1[i] - ds2[j], 2));
            }
            else
                df[i][j] = INT_MAX;
        }
    res = sqrt(df[m - 1][n - 1]);
    for (i = 0; i < m; i++)
        free(df[i]);
    free(df);
    free(ds1);
    free(ds2);
    return res;
}

/*
 * @brief		Compute the Time Warp Edit (TWED) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		penalty	Penalty parameter (lambda)
 * @param		stiff	Stiffness parameter (gamma)
 * @return		float	TWED distance
 *
 */
float			dist_TWED(series *ts1, series *ts2, float penalty, float stiff)
{
    float		**df, res;
    float		min1, min2, min3;
    int			i, j, m, n;
    
    // Use default values if -1 is specified
    if (stiff == -1) { stiff = 0.1f; }
    if (penalty == -1) { penalty = 0.1f; }
    m = ts1->length; n = ts2->length;
    stiff *= ts1->length;
    df = calloc(m + 1, sizeof(float *));
    for (i = 0; i < m + 1; i++)
        df[i] = calloc(n + 1, sizeof(float));
    df[0][0] = 0;
    df[1][0] = pow(ts1->values[0], 2);
    df[0][1] = pow(ts2->values[0], 2);
    for (i = 2; i < m + 1; i++)
        df[i][0] = df[i - 1][0] + pow(ts1->values[i - 2] - ts1->values[i - 1], 2);
    for (j = 2; j < n + 1; j++)
        df[0][j] = df[0][j - 1] + pow(ts2->values[j - 2] - ts2->values[j - 1], 2);
    for (i = 1; i < m + 1; i++)
        for (j = 1; j < n + 1; j++)
        {
            if (i > 1 && j > 1)
                min1 = df[i - 1][j - 1] + stiff * abs(i - j) * 2 + pow(ts1->values[i - 1] - ts2->values[j - 1], 2) + pow(ts1->values[i - 2] - ts2->values[j - 2], 2);
            else
                min1 = df[i - 1][j - 1] + stiff * abs(i - j) + pow(ts1->values[i - 1] - ts2->values[j - 1], 2);
            if (i > 1)
                min2 = df[i - 1][j] + pow(ts1->values[i - 1] - ts1->values[i - 2], 2) + stiff + penalty;
            else
                min2 = df[i - 1][j] + pow(ts1->values[i - 1], 2) + penalty;
            if (j > 1)
                min3 = df[i][j - 1] + pow(ts2->values[j - 1] - ts2->values[j - 2], 2) + stiff + penalty;
            else
                min3 = df[i][j - 1] + pow(ts1->values[j - 1], 2) + penalty;
            df[i][j] = min(min1, min(min2, min3));
        }
    res = df[m][n];
    for (i = 0; i < m + 1; i++)
        free(df[i]);
    free(df);
    return res;
}

float           costMSM(float ai, float ai1, float bj, float penalty)
{
    return penalty + (((ai1 <= ai && ai <= bj) || (ai1 >= ai && ai >= bj)) ? 0 : min(abs(ai - ai1), abs(ai - bj)));
}

/*
 * @brief		Compute the Move Split Merge (MSM) distance between two time series
 *
 * @param		ts1		1st time series
 * @param		ts2		2nd time series
 * @param		penalty	Penalty of the distance
 * @return		float	DTW distance
 *
 */
float			dist_MSM(series *ts1, series *ts2, float penalty)
{
    float		**df, res;
    float		min1, min2;
    int			i, j, m, n;
    
    // Use default values if -1 is specified
    if (penalty == -1) { penalty = 0.1f; }
    m = ts1->length; n = ts2->length;
    df = calloc(m, sizeof(float *));
    for (i = 0; i < m; i++)
        df[i] = calloc(n, sizeof(float));
    df[0][0] = abs(ts1->values[0] - ts2->values[0]);
    for (i = 1; i < m; i++)
        df[i][0] = df[i - 1][0] + costMSM(ts1->values[i], ts1->values[i - 1], ts2->values[0], penalty);
    for (j = 1; j < n; j++)
        df[0][j] = df[0][j - 1] + costMSM(ts2->values[j], ts2->values[0], ts2->values[j - 1], penalty);
    for (i = 1; i < m; i++)
        for (j = 1; j < n; j++)
        {
            min1 = df[i - 1][j] + costMSM(ts1->values[i], ts1->values[i - 1], ts2->values[j], penalty);
            min2 = df[i][j - 1] + costMSM(ts2->values[j], ts1->values[i], ts2->values[j - 1], penalty);
            df[i][j] = min(df[i - 1][j - 1] + abs(ts1->values[i] - ts2->values[j]), min(min1, min2));
        }
    res = df[m - 1][n - 1];
    for (i = 0; i < m; i++)
        free(df[i]);
    free(df);
    return res;
}

/*
 * @brief		Helper function for the Optimal Subsequence Bijection (OSB) distance between two series
 *
 * @param		ts1         1st time series
 * @param		ts2         2nd time series
 * @param       matx        Matrix of pre-computed weights for computing OSB
 * @param       m           Length of 1st series
 * @param       n           Length of 2nd series
 * @param		warp        Size of the warping window (default (-1) : 10% of ts1)
 * @param		queryskip	Maximal number of skips in query
 * @param       targetskip  Maximal number of skips in target
 * @param       jumpcost    Cost of performing a jump
 * @return		float       OSB distance
 *
 */
float           dist_OSB_findPath(series *ts1, series *ts2, float **matx, int m, int n, int warp, int queryskip, int targetskip, float jumpcost)
{
    float       **weight;
    float       newweight, dist = 0, minweight;
    int         **camefromcol, **camefromrow, **stepcount;
    int         stoprowjump, stopk = 0, rowjump;
    int         i, j, k;
    
    weight = malloc(m * sizeof(float *));
    camefromcol = malloc(m * sizeof(int *));
    camefromrow = malloc(m * sizeof(int *));
    stepcount = malloc(m * sizeof(int *));
    for (i = 0; i < m; i++)
    {
        weight[i] = malloc(n * sizeof(float));
        camefromcol[i] = calloc(n, sizeof(int));
        camefromrow[i] = calloc(n, sizeof(int));
        stepcount[i] = malloc(n * sizeof(int));
        for (j = 0; j < n; j++)
        {
            stepcount[i][j] = 1;
            if (m == 0)
                weight[0][j] = matx[0][j];
            else
                weight[i][j] = FLT_MAX;
        }
    }
    for (i = 0; i < m - 1; i++)
        for (j = 0; j < n - 1; j++)
            if (abs(i - j) <= warp)
            {
                stoprowjump = min(m, i + queryskip);
                for (rowjump = i + 1; rowjump < stoprowjump; rowjump++)
                {
                    stopk = min(n, j + targetskip);
                    for (k = j + 1; k < stopk; k++)
                    {
                        newweight = weight[i][j] + matx[rowjump][k] + sqrt(pow(rowjump - i - 1, 2) + pow(k - j - 1, 2)) * jumpcost;
                        if ((1 / (stepcount[i][j])) * weight[rowjump][k] >= (1 / (stepcount[i][j] + 1)) * newweight)
                        {
                            weight[rowjump][k] = newweight;
                            camefromrow[rowjump][k] = i;
                            camefromcol[rowjump][k] = j;
                            stepcount[rowjump][k] = stepcount[i][j] + 1;
                        }
                    }
                }
            }
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            weight[i][j] = weight[i][j] / stepcount[i][j];
    for (minweight = INT_MAX, j = 0; j < n; j++)
        if (weight[m - 1][j] < minweight) { minweight = weight[m - 1][j]; stopk = j; }
    rowjump = (m - 1); stopk = n - 1; k  = 0;
    while (rowjump > 0 && stopk > 0)
    {
        dist += pow(ts1->values[rowjump] - ts2->values[stopk], 2);
        stoprowjump = camefromcol[rowjump][stopk];
        rowjump = camefromrow[rowjump][stopk];
        stopk = stoprowjump;
        k++;
    }
    for (i = 0; i < m; i++)
    {
        free(weight[i]);
        free(camefromcol[i]);
        free(camefromrow[i]);
        free(stepcount[i]);
    }
    free(weight);
    free(camefromcol);
    free(camefromrow);
    free(stepcount);
    return sqrt(dist) / k;
}

/*
 * @brief		Compute the Optimal Subsequence Bijection (OSB) distance between two series
 *
 * @param		ts1         1st time series
 * @param		ts2         2nd time series
 * @param		warp        Size of the warping window (default (-1) : 10% of ts1)
 * @param		queryskip	Maximal number of skips in query
 * @param       targetskip  Maximal number of skips in target
 * @param       jumpcost    Cost of performing a jump
 * @return		float       OSB distance
 *
 */
float			dist_OSB(series *ts1, series *ts2, float warp, int queryskip, int targetskip, float jumpcost)
{
    float       **matx, *mean;
    float       dist = 0;
    int         i, j, m, n;
    
    // Set default values for arguments
    if (warp == -1) { warp = 0.1f; }
    if (queryskip == -1) { queryskip = 2; }
    if (queryskip == -1) { targetskip = 2; }
    m = ts1->length; n = ts2->length;
    warp *= ts1->length;
    matx = malloc(m * sizeof(float *));
    mean = calloc(m, sizeof(float));
    for (i = 0; i < m; i++)
    {
        matx[i] = calloc(n, sizeof(float));
        for (j = 0; j < n; j++)
        {
            matx[i][j] = sqrt(pow(ts2->values[j] - ts1->values[i], 2));
            mean[i] += matx[i][j];
        }
        mean[i] /= n;
        dist += mean[i];
    }
    if (jumpcost == -1)
    {
        dist /= m;
        for (jumpcost = 0, i = 0; i < m; i++)
            jumpcost += pow(mean[i] - dist, 2);
        jumpcost = sqrt(jumpcost / m);
    }
    dist = dist_OSB_findPath(ts1, ts2, matx, m, n, warp, queryskip, targetskip, jumpcost);
    for (i = 0; i < m; i++)
        free(matx[i]);
    free(mean);
    free(matx);
    return dist;
}

/*
 * @brief		Compute the Swale distance between two series
 *
 * @param		ts1             1st time series
 * @param		ts2             2nd time series
 * @param		m_threshold     Threshold of matching between points
 * @param		m_matchreward   Reward for matching points
 * @param       m_gappenalty    Penalty for gaps in matching series
 * @return		float           Swale distance
 *
 */
float           dist_Swale(series *ts1, series *ts2, float m_threshold, float m_matchreward, float m_gappenalty)
{
    float       ***grid;
    float       score = 0.0, gridcellsize, base, lowX, lowY, upX, upY;
    int         cellsperdim, i, j, index, m, n, max, *matches, k, l;
    
    if (m_threshold == -1) { m_threshold = 0.00809622488455851; }
    if (m_matchreward == -1) { m_matchreward = 50; }
    if (m_gappenalty == -1) { m_gappenalty = 0; }
    m = ts1->length;
    n = ts2->length;
    gridcellsize = 2 * m_threshold;
    cellsperdim = 2 * (int)ceil(1 / gridcellsize);
    base = - cellsperdim * gridcellsize / 2;
    grid = calloc(cellsperdim, sizeof(float **));
    for (i = 0; i < cellsperdim; i++)
    {
        grid[i] = calloc(cellsperdim, sizeof(float *));
        for (j = 0; j < cellsperdim; j++)
            grid[i][j] = NULL;
    }
    for (i = 0; i < m; i++)
    {
        index = m - i - 1;
        lowX = floor((((float)index / m) - base) / gridcellsize);
        lowY = floor((ts1->values[index] - base) / gridcellsize);
        upX = floor((((float)index / m) - base) / gridcellsize);
        upY = floor((ts1->values[index] - base) / gridcellsize);
        for (k = lowX; k <= upX; k++)
            for (l = lowY; l <= upY; l++)
            {
                grid[k][l] = malloc(4 * sizeof(float));
                grid[k][l][0] = index - m_threshold;
                grid[k][l][1] = ts1->values[index] - m_threshold;
                grid[k][l][2] = index + m_threshold;
                grid[k][l][3] = ts1->values[index] + m_threshold;
            }
    }
    matches = calloc(n + m + 5, sizeof(int));
    matches[0] = -1;
    for (i = 1; i < m + n; i++)
        matches[i] = m + n + 2;
    max = 0;
    for (i = 0; i < n; i++)
    {
        k =	floor((((float)i/n) - base) / gridcellsize);
        l =	floor((ts2->values[i] - base) / gridcellsize);
        if (grid[k][l] == NULL)
            continue;
        max++;
    }
    for (i = 0; i < cellsperdim; i++)
    {
        for (j = 0; j < cellsperdim; j++)
            if (grid[i][j] != NULL)
                free(grid[i][j]);
        free(grid[i]);
    }
    free(grid);
    free(matches);
    score = max * m_matchreward + (m + n - 2 * max) * m_gappenalty;
    return 1.0 - ((double)score / min(m, n));
}

/*
 * @brief       Helper function for interval distance
 */
float           interval_dist(float *a, float *b)
{
    return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

/*
 * @brief       Helper function for computing threshold-crossing (TC) sequences
 */
float           **computeTCSequence(series *tr, int *size, float m_threshold)
{
    float       **res = NULL;
	float       tstart = 0.0, tend = 0.0;
    float       origin[2] = {0.0,0.0}, ptVals[2] = {0, 0}, lastVals[2] = {0, 0};
    int         n, i, last = -1, pt = -1;
    
    *size = 0;
    n = tr->length;
    for (i = 0; i < n; i++)
    {
        last = pt;
        pt = i;
        ptVals[0] = i; ptVals[1] = tr->values[i];
        if (last != -1) { lastVals[0] = last; lastVals[1] = tr->values[last]; }
        if (last == -1)
        {
            if (interval_dist(ptVals, origin) >= m_threshold)
                tstart = i;
        }
        else if (interval_dist(lastVals, origin) < m_threshold && interval_dist(ptVals, origin) >= m_threshold)
        {
            tstart = pt;
        }
        else if (interval_dist(lastVals, origin) >= m_threshold && interval_dist(ptVals, origin) < m_threshold)
        {
            tend = last;
            *size = *size + 1;
            res = realloc(res, (*size) * sizeof(float *));
            res[(*size) - 1] = calloc(2, sizeof(float));
            res[(*size) - 1][0] = tstart;
            res[(*size) - 1][1] = tend;
        }
    }
    // handle the boundary case
    if (interval_dist(ptVals, origin) >= m_threshold)
    {
        *size = *size + 1;
        res = realloc(res, (*size) * sizeof(float *));
        res[(*size) - 1] = calloc(2, sizeof(float));
        res[(*size) - 1][0] = tstart;
        res[(*size) - 1][1] = pt;
    }
    return res;
}

/*
 * @brief		Compute the TQuEST distance between two series
 *
 * @param		ts1             1st time series
 * @param		ts2             2nd time series
 * @param		m_threshold     Threshold of matching between points
 * @param		m_maxthreshold  Maximum matching threshold
 * @param       m_minthreshold  Minimum matching threshold
 * @param       m_step          Step used for matching
 * @return		float           Swale distance
 *
 */
float           dist_TQuEST(series *ts1, series *ts2, float m_threshold, float m_maxthreshold, float m_minthreshold, float m_step)
{
    float       distsum1 = 0.0, distsum2 = 0.0;
    float       **tcseq1, **tcseq2, *itv1, *itv2;
    int         i, j, size1, size2;

    if (m_threshold == -1) { m_threshold = 0.0; };
    if (m_maxthreshold == -1) { m_maxthreshold = 0.0; };
    if (m_minthreshold == -1) { m_minthreshold = 0.0; };
    if (m_step == -1) { m_step = 0.0; }
    tcseq1 = computeTCSequence(ts1, &size1, m_threshold);
    tcseq2 = computeTCSequence(ts2, &size2, m_threshold);
    for (i = 0; i < size1; i++)
    {
        float dist = 0.0, mindist = FLT_MAX;
        itv1 = tcseq1[i];
        for (j = 0; j < size2; j++)
        {
            itv2 = tcseq2[j];
            dist = interval_dist(itv1, itv2);
            if (dist < mindist)
                mindist = dist;
        }
        distsum1 += mindist;
    }
    for (j = 0; j < size2; j++)
    {
        float dist = 0.0, mindist = FLT_MAX;
        itv2 = tcseq2[j];
        for (i = 0; i < size1; i++)
        {
            itv1 = tcseq1[i];
            dist = interval_dist(itv1, itv2);
            if (dist < mindist)
                mindist = dist;
        }
        distsum2 += mindist;
    }
    free(tcseq1);
    free(tcseq2);
    return distsum1 / size1 + distsum2 / size2;
}

/*
 * @brief		Compute the FFT with L2 (Distance of series FFT)
 *
 * @param		ts1             1st time series
 * @param		ts2             2nd time series
 * @return		float           FFT(L2) distance
 *
 */
float           dist_FFT_L2(series *ts1, series *ts2, int fftSize)
{
    series      *fft1, *fft2;
    float		res;
    int			i;
    
    if (fftSize == -1) { fftSize = 2048; }
    fft1 = transform_FFT(ts1, fftSize);
    fft2 = transform_FFT(ts2, fftSize);
	for (i = 0, res = 0; i < fftSize; i++)
		res += pow(fft1->values[i] - fft2->values[i], 2);
    free(fft1->values);
    free(fft2->values);
	free(fft1);
    free(fft2);
	return sqrt(res);
}

/*
 * @brief		Compute the FFT with DTW (Warping distance of series FFT)
 *
 * @param		ts1             1st time series
 * @param		ts2             2nd time series
 * @return		float           FFT(DTW) distance
 *
 */
float           dist_FFT_DTW(series *ts1, series *ts2, int fftSize, float warp)
{
    series      *fft1, *fft2;
	float		**df, res;
	float		min1, min2;
	int			i, j, m, n;
    
    if (fftSize == -1) { fftSize = 2048; }
    if (warp == -1) { warp = 0.1f; }
    fft1 = transform_FFT(ts1, fftSize);
    fft2 = transform_FFT(ts2, fftSize);
	m = fftSize; n = fftSize;
    warp *= fftSize;
	df = calloc(m, sizeof(float *));
	for (i = 0; i < m; i++)
		df[i] = calloc(n, sizeof(float));
	df[0][0] = sqrt(pow(fft1->values[0] - fft2->values[0], 2));
	for (i = 1; i < m; i++)
		df[i][0] = df[i - 1][0] + sqrt(pow(fft1->values[i] - fft2->values[0], 2));
	for (j = 1; j < n; j++)
		df[0][j] = df[0][j - 1] + sqrt(pow(fft1->values[0] - fft2->values[j], 2));
	for (i = 1; i < m; i++)
		for (j = 1; j < n; j++)
		{
			if ((abs(i - j) <= warp))
			{
				min1 = min(df[i][j - 1], df[i - 1][j]);
				min2 = min(min1, df[i - 1][j - 1]);
				df[i][j] = min2 + pow(fft1->values[i] - fft2->values[j], 2);
			}
			else
				df[i][j] = INT_MAX;
		}
	res = sqrt(df[m - 1][n - 1]);
	for (i = 0; i < m; i++)
		free(df[i]);
	free(df);
    free(fft1->values);
    free(fft2->values);
    free(fft1);
    free(fft2);
	return sqrt(res);
}

/*
 * @brief		Compute the ACF with L2 (Distance of series ACF)
 *
 * @param		ts1             1st time series
 * @param		ts2             2nd time series
 * @return		float           ACF(L2) distance
 *
 */
float           dist_ACF_L2(series *ts1, series *ts2)
{
    series      *acf1, *acf2;
    float		res;
    int			i;

    acf1 = transform_ACF(ts1);
    acf2 = transform_ACF(ts2);
	for (i = 0, res = 0; i < ts1->length; i++)
		res += pow(acf1->values[i] - acf2->values[i], 2);
    free(acf1->values);
    free(acf2->values);
    free(acf1);
    free(acf2);
	return sqrt(res);
}

/*
 * @brief		Compute the ACF with DTW (Warping distance of series ACF)
 *
 * @param		ts1             1st time series
 * @param		ts2             2nd time series
 * @return		float           ACF(DTW) distance
 *
 */
float           dist_ACF_DTW(series *ts1, series *ts2, float warp)
{
    series      *acf1, *acf2;
	float		**df, res;
	float		min1, min2;
	int			i, j, m, n;
    
    if (warp == -1) { warp = 0.1f; }
    acf1 = transform_ACF(ts1);
    acf2 = transform_ACF(ts2);
	m = acf1->length; n = acf2->length;
    warp *= acf1->length;
	df = calloc(m, sizeof(float *));
	for (i = 0; i < m; i++)
		df[i] = calloc(n, sizeof(float));
	df[0][0] = sqrt(pow(acf1->values[0] - acf2->values[0], 2));
	for (i = 1; i < m; i++)
		df[i][0] = df[i - 1][0] + sqrt(pow(acf1->values[i] - acf2->values[0], 2));
	for (j = 1; j < n; j++)
		df[0][j] = df[0][j - 1] + sqrt(pow(acf1->values[0] - acf2->values[j], 2));
	for (i = 1; i < m; i++)
		for (j = 1; j < n; j++)
		{
			if ((abs(i - j) <= warp))
			{
				min1 = min(df[i][j - 1], df[i - 1][j]);
				min2 = min(min1, df[i - 1][j - 1]);
				df[i][j] = min2 + pow(acf1->values[i] - acf2->values[j], 2);
			}
			else
				df[i][j] = INT_MAX;
		}
	res = sqrt(df[m - 1][n - 1]);
	for (i = 0; i < m; i++)
		free(df[i]);
	free(df);
    free(acf1->values);
    free(acf2->values);
    free(acf1);
    free(acf2);
	return sqrt(res);
}

/* Compress from file source to file dest until EOF on source. */
int                 compressSimple(const char *src, const char *dst, int level)
{
    FILE            *source, *dest;
    int             ret, flush;
    unsigned        have;
    z_stream        strm;
    unsigned char   in[CHUNK];
    unsigned char   out[CHUNK];
    
    source = fopen(src, "r");
    dest = fopen(dst, "w");
    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;
    /* compress until end of file */
    do {
        strm.avail_in = (unsigned int)fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest))
            {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        }
        while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
        /* done when last data in file processed */
    }
    while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);
    /* clean up and return */
    (void)deflateEnd(&strm);
    fclose(source);
    fclose(dest);
    return Z_OK;
}

/*
 * @brief		Compute the Compression-based Dissimilarity Measure (CDM)
 *
 * @param		ts1             1st time series
 * @param		ts2             2nd time series
 * @param       timeBins        Number of time bins to account
 * @return		float           CDM distance
 *
 */
float           dist_CDM(series *ts1, series *ts2, int timeBins)
{
    FILE        *tmpF;
    char        *str1, *str2, *fID, *cID;
    long        s1, s2, sb;
    struct stat *fstruct;
    int         tid;
    
    if (timeBins == -1) { timeBins = 16; }
    str1 = transform_SAX(ts1, 16, timeBins);
    str2 = transform_SAX(ts2, 16, timeBins);
    tid = omp_get_thread_num();
    fID = calloc(64, sizeof(char));
    cID = calloc(64, sizeof(char));
    fstruct = calloc(1, sizeof(struct stat));
    sprintf(fID, "/tmp/str1_%d.txt", tid); sprintf(cID, "/tmp/str1_%d.comp", tid);
    tmpF = fopen(fID, "w"); fprintf(tmpF, "%s", str1); fclose(tmpF);
    compressSimple(fID, cID, 5);
    stat(cID, fstruct);
    s1 = fstruct->st_size;
    sprintf(fID, "/tmp/str2_%d.txt", tid); sprintf(cID, "/tmp/str2_%d.comp", tid);
    tmpF = fopen(fID, "w"); fprintf(tmpF, "%s", str2); fclose(tmpF);
    compressSimple(fID, cID, 5);
    stat(cID, fstruct);
    s2 = fstruct->st_size;
    sprintf(fID, "/tmp/str12_%d.txt", tid); sprintf(cID, "/tmp/str12_%d.comp", tid);
    tmpF = fopen(fID, "w"); fprintf(tmpF, "%s%s", str1, str2); fclose(tmpF);
    compressSimple(fID, cID, 5);
    stat(cID, fstruct);
    sb = fstruct->st_size;
    free(str1); free(str2);
    free(fID); free(cID);
    free(fstruct);
	return (float)sb / (float)(s1 + s2);
}

//
// Wrapper functions to real distance call
//
float			dist_L1_wrap(series *ts1, series *ts2, float *args)
{
    return dist_L1(ts1, ts2);
}

//
// Wrapper functions to real distance call
//
float			dist_Euclidean_wrap(series *ts1, series *ts2, float *args)
{
    return dist_Euclidean(ts1, ts2);
}

//
// Wrapper functions to real distance call
//
float			dist_LI_wrap(series *ts1, series *ts2, float *args)
{
    return dist_LI(ts1, ts2);
}

//
// Wrapper functions to real distance call
//
float           dist_Lp_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_Lp(ts1, ts2, 10);
    return dist_Lp(ts1, ts2, args[0]);
}

//
// Wrapper functions to real distance call
//
float			dist_ERP_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_ERP(ts1, ts2, -1);
    return dist_ERP(ts1, ts2, args[0]);
}

//
// Wrapper functions to real distance call
//
float			dist_EDR_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return (dist_EDR(ts1, ts2, -1, -1, -1, -1, -1));
    return dist_EDR(ts1, ts2, args[0], args[1], args[2], args[3], args[4]);
}

//
// Wrapper functions to real distance call
//
float			dist_LCSS_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_LCSS(ts1, ts2, -1, -1);
    return dist_LCSS(ts1, ts2, args[0], args[1]);
}

//
// Wrapper functions to real distance call
//
float			dist_DTW_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_DTW(ts1, ts2, -1, NULL);
    return dist_DTW(ts1, ts2, args[0], NULL);
}

//
// Wrapper functions to real distance call
//
float			dist_DDTW_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_DDTW(ts1, ts2, -1, NULL);
    return dist_DDTW(ts1, ts2, args[0], NULL);
}

//
// Wrapper functions to real distance call
//
float			dist_WDTW_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_WDTW(ts1, ts2, -1);
    return dist_WDTW(ts1, ts2, args[0]);
}

//
// Wrapper functions to real distance call
//
float			dist_WDDTW_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_WDDTW(ts1, ts2, -1);
    return dist_WDDTW(ts1, ts2, args[0]);
}

//
// Wrapper functions to real distance call
//
float			dist_TWED_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_TWED(ts1, ts2, -1, -1);
    return dist_TWED(ts1, ts2, args[0], args[1]);
}

//
// Wrapper functions to real distance call
//
float			dist_MSM_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_MSM(ts1, ts2, -1);
    return dist_MSM(ts1, ts2, args[0]);
}

//
// Wrapper functions to real distance call
//
float			dist_OSB_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_OSB(ts1, ts2, -1, -1, -1, -1);
    return dist_OSB(ts1, ts2, args[0], (int)(args[1]), (int)(args[2]), args[3]);
}

//
// Wrapper functions to real distance call
//
float           dist_Swale_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_Swale(ts1, ts2, -1, -1, -1);
    return dist_Swale(ts1, ts2, args[0], args[1], args[2]);
}

//
// Wrapper functions to real distance call
//
float           dist_TQuEST_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_TQuEST(ts1, ts2, -1, -1, -1, -1);
    return dist_TQuEST(ts1, ts2,  args[0], args[1], args[2], args[3]);
}

//
// Wrapper functions to real distance call
//
float			dist_FFT_L2_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_FFT_L2(ts1, ts2, -1);
    return dist_FFT_L2(ts1, ts2, (int)args[0]);
}

//
// Wrapper functions to real distance call
//
float			dist_FFT_DTW_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_FFT_DTW(ts1, ts2, -1, -1);
    return dist_FFT_DTW(ts1, ts2, (int)args[0], args[1]);
}

//
// Wrapper functions to real distance call
//
float			dist_ACF_L2_wrap(series *ts1, series *ts2, float *args)
{
    return dist_ACF_L2(ts1, ts2);
}

//
// Wrapper functions to real distance call
//
float			dist_ACF_DTW_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_ACF_DTW(ts1, ts2, -1);
    return dist_ACF_DTW(ts1, ts2, args[0]);
}

//
// Wrapper functions to real distance call
//
float			dist_CDM_wrap(series *ts1, series *ts2, float *args)
{
    if (args == NULL)
        return dist_CDM(ts1, ts2, -1);
    return dist_CDM(ts1, ts2, (int)args[0]);
}

parameter       *create_parameter(char *name, int type, float mi, float ma, float st, float be)
{
    parameter   *result;
    float       iterate;
    int         i;
    
    result = calloc(1, sizeof(parameter));
    result->name = calloc(strlen(name) + 1, sizeof(char));
    sprintf(result->name, "%s", name);
    result->best = be;
    result->error = 1.0;
    result->type = type;
    result->min = mi;
    result->max = ma;
    result->step = st;
    result->opt_size = (int)(((fabsf(ma) - fabsf(mi)) / fabsf(st)) + 1);
    result->optimized = calloc(result->opt_size, sizeof(float));
    for (iterate = mi, i = 0; fabsf(iterate) <= fabsf(ma); i++, iterate += st)
        result->optimized[i] = iterate;
    return result;
}

void            add_distance(distances *ret, int id, char *name, float (*func)(series *, series *, float *), int nb_args)
{
    ret->compute[id] = 1;
    sprintf(ret->name[id], "%s", name);
    ret->functions[id] = func;
    ret->args[id] = nb_args;
    if (nb_args > 0)
        ret->params[id] = calloc(nb_args, sizeof(parameter *));
}

distances       *init_distances(int compute)
{
    distances   *ret;
    int         i;
    
    ret = malloc(sizeof(distances));
    ret->nb_distances = 21;
    ret->nb_compute = 0;
    ret->name = malloc(ret->nb_distances * sizeof(char *));
    for (i = 0; i < ret->nb_distances; i++)
        ret->name[i] = calloc(32, sizeof(char));
    ret->compute = calloc(ret->nb_distances, sizeof(int));
    for (i = 0; i < ret->nb_distances; i++)
        ret->compute[i] = compute;
    ret->resample = calloc(ret->nb_distances, sizeof(int));
    for (i = 0; i < ret->nb_distances; i++)
        ret->resample[i] = -1;
    ret->args = calloc(ret->nb_distances, sizeof(int));
    ret->params = calloc(ret->nb_distances, sizeof(parameter **));
    ret->functions = calloc(ret->nb_distances, sizeof(float (*)(series *, series *, float *)));
    add_distance(ret, 0, "L1", dist_L1_wrap, 0);
    add_distance(ret, 1, "L2", dist_Euclidean_wrap, 0);
    add_distance(ret, 2, "LI", dist_LI_wrap, 0);
    add_distance(ret, 3, "LP", dist_Lp_wrap, 1);
    ret->params[3][0] = create_parameter("p", PARAM_FLOAT, 0.5, 10, 0.5, 5);
    add_distance(ret, 4, "ERP", dist_ERP_wrap, 1);
    ret->params[4][0] = create_parameter("gap", PARAM_FLOAT, 0, 10, 1, 0.1);
    add_distance(ret, 5, "EDR", dist_EDR_wrap, 5);
    ret->params[5][0] = create_parameter("warp", PARAM_FLOAT, 0.0, 1.0, 0.05, 0.1);
    ret->params[5][1] = create_parameter("thresh", PARAM_FLOAT, 0.01, 0.04, 0.01, 0.25);
    ret->params[5][2] = create_parameter("hit", PARAM_FLOAT, 0, 2, 1, 1);
    ret->params[5][3] = create_parameter("miss", PARAM_FLOAT, -1, -2, -1, -1);
    ret->params[5][4] = create_parameter("gap", PARAM_FLOAT, -1, -2, -1, -1);
    add_distance(ret, 6, "LCSS", dist_LCSS_wrap, 2);
    ret->params[6][0] = create_parameter("warp", PARAM_FLOAT, 0.01, 0.5, 0.01, 0.1);
    ret->params[6][1] = create_parameter("thresh", PARAM_FLOAT, 0, 1.0, 0.1, 0.25);
    add_distance(ret, 7, "DTW", dist_DTW_wrap, 1);
    ret->params[7][0] = create_parameter("warp", PARAM_FLOAT, 0.01, 0.5, 0.01, 0.1);
    add_distance(ret, 8, "OSB", dist_OSB_wrap, 4);
    ret->params[8][0] = create_parameter("warp", PARAM_FLOAT, 0.025, 0.5, 0.025, 0.1);
    ret->params[8][1] = create_parameter("queryskip", PARAM_INT, 1, 3, 1, 2);
    ret->params[8][2] = create_parameter("targetskip", PARAM_INT, 1, 3, 1, 2);
    ret->params[8][3] = create_parameter("jumpcost", PARAM_INT, 1, 3, 1, 1);
    add_distance(ret, 9, "Swale", dist_Swale_wrap, 3);
    ret->params[9][0] = create_parameter("thresh", PARAM_FLOAT, 0.0, 0.5, 0.01, 0.25);
    ret->params[9][1] = create_parameter("match", PARAM_FLOAT, 0, 100, 1, 50);
    ret->params[9][2] = create_parameter("penalty", PARAM_FLOAT, 0, 10, 1, 0);
    ret->compute[9] = 0;
    add_distance(ret, 10, "TQuEST", dist_TQuEST_wrap, 4);
    ret->params[10][0] = create_parameter("thresh", PARAM_FLOAT, 0.0, 0.5, 0.01, 0.0);
    ret->params[10][1] = create_parameter("max-thresh", PARAM_FLOAT, 0, 1, 0.25, 0.0);
    ret->params[10][2] = create_parameter("min-thresh", PARAM_FLOAT, 0, 1, 0.25, 0.0);
    ret->params[10][3] = create_parameter("step", PARAM_FLOAT, 0, 1, 0.25, 0.0);
    ret->compute[10] = 0;
    add_distance(ret, 11, "FFT(L2)", dist_FFT_L2_wrap, 1);
    ret->params[11][0] = create_parameter("fftSize", PARAM_INT, 128, 1100, 128, 256);
    add_distance(ret, 12, "FFT(DTW)", dist_FFT_DTW_wrap, 2);
    ret->params[12][0] = create_parameter("fftSize", PARAM_INT, 128, 1100, 128, 256);
    ret->params[12][1] = create_parameter("warp", PARAM_FLOAT, 0.01, 0.5, 0.01, 0.1);
    add_distance(ret, 13, "ACF(L2)", dist_ACF_L2_wrap, 0);
    add_distance(ret, 14, "ACF(DTW)", dist_ACF_DTW_wrap, 1);
    ret->params[14][0] = create_parameter("warp", PARAM_FLOAT, 0.01, 0.5, 0.01, 0.1);
    add_distance(ret, 15, "CDM", dist_CDM_wrap, 1);
    ret->params[15][0] = create_parameter("bins", PARAM_FLOAT, 32, 256, 32, 64);
    add_distance(ret, 16, "DDTW", dist_DDTW_wrap, 1);
    ret->params[16][0] = create_parameter("warp", PARAM_FLOAT, 0.01, 0.5, 0.01, 0.1);
    add_distance(ret, 17, "WDTW", dist_WDTW_wrap, 1);
    ret->params[17][0] = create_parameter("penalty", PARAM_FLOAT, 0.01, 1.0, 0.01, 0.1);
    add_distance(ret, 18, "WDDTW", dist_WDDTW_wrap, 1);
    ret->params[18][0] = create_parameter("penalty", PARAM_FLOAT, 0.01, 1.0, 0.01, 0.1);
    add_distance(ret, 19, "TWED", dist_TWED_wrap, 2);
    ret->params[19][0] = create_parameter("penalty", PARAM_FLOAT, 0.05, 1.0, 0.05, 0.1);
    ret->params[19][1] = create_parameter("stiffness", PARAM_FLOAT, 0.01, 1.0, 0.01, 0.1);
    add_distance(ret, 20, "MSM", dist_MSM_wrap, 1);
    ret->params[20][0] = create_parameter("penalty", PARAM_FLOAT, 0.05, 1.0, 0.05, 0.1);
    return ret;
}

float           *compute_distances(series *ts1, series *ts2, distances *dist)
{
    float       *result;
    float       *args;
    int         i, j, n;
    
    if (dist->nb_compute == 0)
        for (i = 0; i < dist->nb_distances; i++)
            dist->nb_compute += dist->compute[i];
    result = calloc(dist->nb_compute, sizeof(float));
    for (i = 0, n = 0; i < dist->nb_distances; i++)
        if (dist->compute[i])
        {
            if (dist->args[i] == 0)
                result[n++] = dist->functions[i](ts1, ts2, NULL);
            else
            {
                args = calloc(dist->args[i], sizeof(float));
                for (j = 0; j < dist->args[i]; j++)
                    if (dist->params[i][j] != NULL)
                        args[j] = dist->params[i][j]->best;
                    else
                        args[j] = 0;
                result[n++] = dist->functions[i](ts1, ts2, args);
                free(args);
            }
        }
    return result;
}