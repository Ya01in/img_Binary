#pragma once
#include "packageEtFunctions.h"

// The threshold is the mean of the greyscale data
Mat meanSeg(Mat& gray)
{
	cv::Mat output;
	int* hista = getHist(gray);
	int threshold = 999;

	long tot = 0, sum = 0;
	for (int i = 0; i <= maxIntensity; i++) {
		tot += hista[i];
		sum += ((long)i * (long)hista[i]);
	}
	threshold = (int)floor(sum / tot);


	output = binarize(gray, threshold);

	return output;
}
