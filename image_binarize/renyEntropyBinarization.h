#pragma once
#include "packageEtFunctions.h"

Mat reny(Mat& gray)
{
	cv::Mat output;
	int* hist = getHist(gray);
	int threshold;
	int opt_threshold;

	int ih, it;
	int first_bin;
	int last_bin;
	int tmp_var;
	int t_star1, t_star2, t_star3;
	int beta1, beta2, beta3;
	double alpha;/* alpha parameter of the method */
	double term;
	double tot_ent;  /* total entropy */
	double max_ent;  /* max entropy */
	double ent_back; /* entropy of the background pixels at a given threshold */
	double ent_obj;  /* entropy of the object pixels at a given threshold */
	double omega;
	double norm_histo[maxIntensity + 1]; /* normalized histogram */
	double P1[maxIntensity + 1]; /* cumulative normalized histogram */
	double P2[maxIntensity + 1];

	int total = 0;
	for (ih = 0; ih <= maxIntensity; ih++)
		total += hist[ih];

	for (ih = 0; ih <= maxIntensity; ih++)
		norm_histo[ih] = (double)hist[ih] / total;

	P1[0] = norm_histo[0];
	P2[0] = 1.0 - P1[0];
	for (ih = 1; ih <= maxIntensity; ih++) {
		P1[ih] = P1[ih - 1] + norm_histo[ih];
		P2[ih] = 1.0 - P1[ih];
	}

	/* Determine the first non-zero bin */
	first_bin = 0;
	for (ih = 0; ih <= maxIntensity; ih++) {
		if (!(abs(P1[ih]) < 2.220446049250313E-16)) {
			first_bin = ih;
			break;
		}
	}

	/* Determine the last non-zero bin */
	last_bin = maxIntensity;
	for (ih = maxIntensity; ih >= first_bin; ih--) {
		if (!(abs(P2[ih]) < 2.220446049250313E-16)) {
			last_bin = ih;
			break;
		}
	}

	/* Maximum Entropy Thresholding - BEGIN */
	/* ALPHA = 1.0 */
	/* Calculate the total entropy each gray-level
	and find the threshold that maximizes it
	*/
	threshold = 0; // was MIN_INT in original code, but if an empty image is processed it gives an error later on.
	max_ent = 0.0;

	for (it = first_bin; it <= last_bin; it++) {
		/* Entropy of the background pixels */
		ent_back = 0.0;
		for (ih = 0; ih <= it; ih++) {
			if (hist[ih] != 0) {
				ent_back -= (norm_histo[ih] / P1[it]) * log(norm_histo[ih] / P1[it]);
			}
		}

		/* Entropy of the object pixels */
		ent_obj = 0.0;
		for (ih = it + 1; ih <= maxIntensity; ih++) {
			if (hist[ih] != 0) {
				ent_obj -= (norm_histo[ih] / P2[it]) * log(norm_histo[ih] / P2[it]);
			}
		}

		/* Total entropy */
		tot_ent = ent_back + ent_obj;

		// IJ.log(""+max_ent+"  "+tot_ent);

		if (max_ent < tot_ent) {
			max_ent = tot_ent;
			threshold = it;
		}
	}
	t_star2 = threshold;

	/* Maximum Entropy Thresholding - END */
	threshold = 0; //was MIN_INT in original code, but if an empty image is processed it gives an error later on.
	max_ent = 0.0;
	alpha = 0.5;
	term = 1.0 / (1.0 - alpha);
	for (it = first_bin; it <= last_bin; it++) {
		/* Entropy of the background pixels */
		ent_back = 0.0;
		for (ih = 0; ih <= it; ih++)
			ent_back += sqrt(norm_histo[ih] / P1[it]);

		/* Entropy of the object pixels */
		ent_obj = 0.0;
		for (ih = it + 1; ih <= maxIntensity; ih++)
			ent_obj += sqrt(norm_histo[ih] / P2[it]);

		/* Total entropy */
		tot_ent = term * ((ent_back * ent_obj) > 0.0 ? log(ent_back * ent_obj) : 0.0);

		if (tot_ent > max_ent) {
			max_ent = tot_ent;
			threshold = it;
		}
	}

	t_star1 = threshold;

	threshold = 0; //was MIN_INT in original code, but if an empty image is processed it gives an error later on.
	max_ent = 0.0;
	alpha = 2.0;
	term = 1.0 / (1.0 - alpha);
	for (it = first_bin; it <= last_bin; it++) {
		/* Entropy of the background pixels */
		ent_back = 0.0;
		for (ih = 0; ih <= it; ih++)
			ent_back += (norm_histo[ih] * norm_histo[ih]) / (P1[it] * P1[it]);

		/* Entropy of the object pixels */
		ent_obj = 0.0;
		for (ih = it + 1; ih <= maxIntensity; ih++)
			ent_obj += (norm_histo[ih] * norm_histo[ih]) / (P2[it] * P2[it]);

		/* Total entropy */
		tot_ent = term * ((ent_back * ent_obj) > 0.0 ? log(ent_back * ent_obj) : 0.0);

		if (tot_ent > max_ent) {
			max_ent = tot_ent;
			threshold = it;
		}
	}

	t_star3 = threshold;

	/* Sort t_star values */
	if (t_star2 < t_star1) {
		tmp_var = t_star1;
		t_star1 = t_star2;
		t_star2 = tmp_var;
	}
	if (t_star3 < t_star2) {
		tmp_var = t_star2;
		t_star2 = t_star3;
		t_star3 = tmp_var;
	}
	if (t_star2 < t_star1) {
		tmp_var = t_star1;
		t_star1 = t_star2;
		t_star2 = tmp_var;
	}

	/* Adjust beta values */
	if (abs(t_star1 - t_star2) <= 5) {
		if (abs(t_star2 - t_star3) <= 5) {
			beta1 = 1;
			beta2 = 2;
			beta3 = 1;
		}
		else {
			beta1 = 0;
			beta2 = 1;
			beta3 = 3;
		}
	}
	else {
		if (abs(t_star2 - t_star3) <= 5) {
			beta1 = 3;
			beta2 = 1;
			beta3 = 0;
		}
		else {
			beta1 = 1;
			beta2 = 2;
			beta3 = 1;
		}
	}
	//IJ.log(""+t_star1+" "+t_star2+" "+t_star3);
	/* Determine the optimal threshold value */
	omega = P1[t_star3] - P1[t_star1];
	opt_threshold = (int)(t_star1 * (P1[t_star1] + 0.25 * omega * beta1) + 0.25 * t_star2 * omega * beta2 + t_star3 * (P2[t_star3] + 0.25 * omega * beta3));


	output = binarize(gray, opt_threshold);

	return output;
}
