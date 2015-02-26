/*
 *  distances.h
 *  HV-MOTS Classification
 *
 *	This file contains the header information for time series distances. Currently implemented :
 *		- L1-Norm
 *		- Euclidean
 *		- Lp-Norms
 *		- LI-Norm
 *		- Longest Common SubSequence (LCSS)
 *		- Edit Distance on Real values (EDR)
 *		- Edit with Real Penalty (ERP)
 *
 *  Created by Philippe ESLING on 08/01/14.
 *  Copyleft 2014 IRCAM.
 *
 */

#ifndef DISTANCES_H_
# define DISTANCES_H_

# include "types.h"

float			dist_L1(series *ts1, series *ts2);
float			dist_Euclidean(series *ts1, series *ts2);
float			dist_LI(series *ts1, series *ts2);
float			dist_Lp(series *ts1, series *ts2, float p);
float			dist_ERP(series *ts1, series *ts2, float gap);
float			dist_EDR(series *ts1, series *ts2, float warp, float thresh, float hit, float miss, float gap);
float			dist_LCSS(series *ts1, series *ts2, float warp, float thresh);
float			dist_DTW(series *ts1, series *ts2, float warp, int **window);
float			dist_DDTW(series *ts1, series *ts2, float warp, int **window);
float			dist_WDTW(series *ts1, series *ts2, float penalty);
float			dist_WDDTW(series *ts1, series *ts2, float penalty);
float			dist_TWED(series *ts1, series *ts2, float penalty, float stiff);
float			dist_MSM(series *ts1, series *ts2, float penalty);
float			dist_OSB(series *ts1, series *ts2, float warp, int queryskip, int targetskip, float jumpcost);
float           dist_Swale(series *ts1, series *ts2, float m_threshold, float m_matchreward, float m_gappenalty);
float           dist_TQuEST(series *ts1, series *ts2, float m_threshold, float m_maxthreshold, float m_minthreshold, float m_step);
float           dist_FFT_L2(series *ts1, series *ts2, int fftSize);
float           dist_FFT_DTW(series *ts1, series *ts2, int fftSize, float warp);
float           dist_ACF_L2(series *ts1, series *ts2);
float           dist_ACF_DTW(series *ts1, series *ts2, float warp);
float           dist_CDM(series *ts1, series *ts2, int timeBins);
float			dist_L1_wrap(series *ts1, series *ts2, float *args);
float			dist_Euclidean_wrap(series *ts1, series *ts2, float *args);
float			dist_Lp_wrap(series *ts1, series *ts2, float *args);
float			dist_LI_wrap(series *ts1, series *ts2, float *args);
float			dist_ERP_wrap(series *ts1, series *ts2, float *args);
float			dist_EDR_wrap(series *ts1, series *ts2, float *args);
float			dist_LCSS_wrap(series *ts1, series *ts2, float *args);
float			dist_DTW_wrap(series *ts1, series *ts2, float *args);
float			dist_DDTW_wrap(series *ts1, series *ts2, float *args);
float			dist_WDTW_wrap(series *ts1, series *ts2, float *args);
float			dist_WDDTW_wrap(series *ts1, series *ts2, float *args);
float			dist_TWED_wrap(series *ts1, series *ts2, float *args);
float			dist_MSM_wrap(series *ts1, series *ts2, float *args);
float			dist_OSB_wrap(series *ts1, series *ts2, float *args);
float           dist_Swale_wrap(series *ts1, series *ts2, float *args);
float           dist_TQuEST_wrap(series *ts1, series *ts2, float *args);
float           dist_FFT_L2_wrap(series *ts1, series *ts2, float *args);
float           dist_FFT_DTW_wrap(series *ts1, series *ts2, float *args);
float           dist_ACF_L2_wrap(series *ts1, series *ts2, float *args);
float           dist_ACF_DTW_wrap(series *ts1, series *ts2, float *args);
float           dist_CDM_wrap(series *ts1, series *ts2, float *args);
distances       *init_distances(int compute);
float           *compute_distances(series *ts1, series *ts2, distances *dist);

#endif