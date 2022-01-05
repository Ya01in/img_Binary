#pragma once
#include "packageEtFunctions.h"

Mat seg_interative(Mat& gray)
{
	cv::Mat output;
	int* hist = getHist(gray);
	int thresholdValue = 999;



	output = binarize(gray, thresholdValue);

	return output;
}
