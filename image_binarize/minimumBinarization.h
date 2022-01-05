#pragma once
#include "packageEtFunctions.h"

Mat mini(Mat& gray)
{
	int iter = 0;
	int threshold = -1;
	int max = -1;
	double iHisto[maxIntensity+1];
	cv::Mat output;
	int* hist = getHist(gray);
	

	for (int i = 0; i <= maxIntensity; i++) {
		iHisto[i] = (double)hist[i];
		if (hist[i] > 0) max = i;
	}
	double tHisto[maxIntensity+1]; // Instead of double[] tHisto = iHisto ;
	while (!bimodalTest(iHisto)) {
		//smooth with a 3 point running mean filter
		for (int i = 1; i < maxIntensity; i++)
			tHisto[i] = (iHisto[i - 1] + iHisto[i] + iHisto[i + 1]) / 3;
		tHisto[0] = (iHisto[0] + iHisto[1]) / 3; //0 outside
		tHisto[maxIntensity] = (iHisto[maxIntensity - 1] + iHisto[maxIntensity]) / 3; //0 outside
		std::memcpy(tHisto, iHisto, maxIntensity+1); //Instead of iHisto = tHisto ;
		iter++;
		if (iter > 10000) {
			threshold = -1;
			std::cout<< "ERROR: Minimum Threshold not found after 10000 iterations.\n";
			Mat empty;
			return empty;
		}
	}
	// The threshold is the minimum between the two peaks. modified for 16 bits

	for (int i = 1; i < max; i++) {
		//IJ.log(" "+i+"  "+iHisto[i]);
		if (iHisto[i - 1] > iHisto[i] && iHisto[i + 1] >= iHisto[i]) {
			threshold = i;
			break;
		}
	}

	output = binarize(gray, threshold);

	return output;
}
