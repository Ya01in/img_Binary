#pragma once
#include "packageEtFunctions.h"

Mat li(Mat& gray)
{
	cv::Mat output;
	int* hista = getHist(gray);
	int threshold = 999;
	int ih;
	int num_pixels;
	int sum_back; /* sum of the background pixels at a given threshold */
	int sum_obj;  /* sum of the object pixels at a given threshold */
	int num_back; /* number of background pixels at a given threshold */
	int num_obj;  /* number of object pixels at a given threshold */
	double old_thresh;
	double new_thresh;
	double mean_back; /* mean of the background pixels at a given threshold */
	double mean_obj;  /* mean of the object pixels at a given threshold */
	double mean;  /* mean gray-level in the image */
	double tolerance; /* threshold tolerance */
	double temp;

	tolerance = 0.5;
	num_pixels = 0;
	for (ih = 0; ih <= maxIntensity; ih++)
		num_pixels += hista[ih];

	/* Calculate the mean gray-level */
	mean = 0.0;
	for (ih = 0; ih < maxIntensity; ih++) //0 + 1?
		mean += ih * hista[ih];
	mean /= num_pixels;
	/* Initial estimate */
	new_thresh = mean;

	do {
		old_thresh = new_thresh;
		threshold = (int)(old_thresh + 0.5);	/* range */
		/* Calculate the means of background and object pixels */
		/* Background */
		sum_back = 0;
		num_back = 0;
		for (ih = 0; ih <= threshold; ih++) {
			sum_back += ih * hista[ih];
			num_back += hista[ih];
		}
		mean_back = (num_back == 0 ? 0.0 : (sum_back / (double)num_back));
		/* Object */
		sum_obj = 0;
		num_obj = 0;
		for (ih = threshold + 1; ih <= maxIntensity; ih++) {
			sum_obj += ih * hista[ih];
			num_obj += hista[ih];
		}
		mean_obj = (num_obj == 0 ? 0.0 : (sum_obj / (double)num_obj));

		/* Calculate the new threshold: Equation (7) in Ref. 2 */
		//new_thresh = simple_round ( ( mean_back - mean_obj ) / ( Math.log ( mean_back ) - Math.log ( mean_obj ) ) );
		//simple_round ( double x ) {
		// return ( int ) ( IS_NEG ( x ) ? x - .5 : x + .5 );
		//}
		//
		//#define IS_NEG( x ) ( ( x ) < -DBL_EPSILON ) 
		//DBL_EPSILON = 2.220446049250313E-16
		temp = (mean_back - mean_obj) / (log(mean_back) - log(mean_obj));

		if (temp < -2.220446049250313E-16)
			new_thresh = (int)(temp - 0.5);
		else
			new_thresh = (int)(temp + 0.5);
		/*  Stop the iterations when the difference between the
		new and old threshold values is less than the tolerance */
	} while (abs(new_thresh - old_thresh) > tolerance);

	output = binarize(gray, threshold);

	return output;
}
