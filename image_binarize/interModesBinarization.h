#pragma once
#include "packageEtFunctions.h"

//have bug inside this concept
Mat interModes(Mat& gray)
{
	cv::Mat output;
	int* hist = getHist(gray);
	int thresholdValue = 999;
	double iHisto[maxIntensity+1];
	int iter = 0;
	for (int i = 0; i <= maxIntensity; i++)
		iHisto[i] = (double)hist[i];

	while (!bimodalTest(iHisto)) {
		//smooth with a 3 point running mean filter
		double previous = 0, current = 0, next = iHisto[0];
		for (int i = 0; i < maxIntensity; i++) {
			previous = current;
			current = next;
			next = iHisto[i + 1];
			iHisto[i] = (previous + current + next) / 3;
		}
		iHisto[maxIntensity - 1] = (current + next) / 3;
		iter++;
		//std::cout << iter << std::endl;
		if (iter > 10000) {
			thresholdValue = -1;
			std::cout<< "ERROR: Intermodes Threshold not found after 10000 iterations.\n";
			Mat empty;
			return empty;
		}
	}

	// The threshold is the mean between the two peaks.
	int tt = 0;
	for (int i = 1; i < maxIntensity; i++) {
		if (iHisto[i - 1] < iHisto[i] && iHisto[i + 1] < iHisto[i]) {
			tt += i;
		}
	}
	thresholdValue = (int)floor(tt / 2.0);


	output = binarize(gray, thresholdValue);

	return output;
}