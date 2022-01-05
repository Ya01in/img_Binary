#pragma once
#include "packageEtFunctions.h"

Mat interative(Mat& gray)
{
	cv::Mat output;
	int* hist = getHist(gray);

	//// Original IJ implementation for compatibility.
	int level;
	double result, sum1, sum2, sum3, sum4;
	int min = 0;
	int max = maxIntensity;

	while ((hist[min] == 0) && (min < maxIntensity+1))min++;
	while ((hist[max] == 0) && (max > 0))max--;
	if (min >= max) {
		level = maxIntensity / 2;
	}
	else
	{
		int movingIndex = min;
		int inc = std::max(max / 40, 1);
		do {
			sum1 = sum2 = sum3 = sum4 = 0.0;
			for (int i = min; i <= movingIndex; i++) {
				sum1 += i * hist[i];
				sum2 += hist[i];
			}
			for (int i = (movingIndex + 1); i <= max; i++) {
				sum3 += i * hist[i];
				sum4 += hist[i];
			}
			result = (sum1 / sum2 + sum3 / sum4) / 2.0;
			movingIndex++;
		} while ((movingIndex + 1) <= result && movingIndex < maxIntensity);

		//.showProgress(1.0);
		level = (int)round(result);
	}
	std::cout << "level value:" << level << std::endl;

	output = binarize(gray, level);

	return output;
}
