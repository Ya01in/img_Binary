#pragma once
#include "packageEtFunctions.h"

Mat triangle(Mat& gray)
{
	cv::Mat output;
	int* hist = getHist(gray);
	int thresholdValue = 999;


	// find min and max
	int min = 0, dmax = 0, max = 0, min2 = 0;
	for (int i = 0; i <= maxIntensity; i++) {
		if (hist[i] > 0) {
			min = i;
			break;
		}
	}
	if (min > 0) min--; // line to the (p==0) point, not to data[min]

	// The Triangle algorithm cannot tell whether the data is skewed to one side or another.
	// This causes a problem as there are 2 possible thresholds between the max and the 2 extremes
	// of the histogram.
	// Here I propose to find out to which side of the max point the data is furthest, and use that as
	//  the other extreme. Note that this is not done in the original method. GL
	for (int i = maxIntensity; i > 0; i--) {
		if (hist[i] > 0) {
			min2 = i;
			break;
		}
	}
	if (min2 < maxIntensity) min2++; // line to the (p==0) point, not to data[min]

	for (int i = 0; i < maxIntensity+1; i++) {
		if (hist[i] > dmax) {
			max = i;
			dmax = hist[i];
		}
	}
	// find which is the furthest side
	//IJ.log(""+min+" "+max+" "+min2);
	bool inverted = false;
	if ((max - min) < (min2 - max)) {
		// reverse the histogram
		//IJ.log("Reversing histogram.");
		inverted = true;
		int left = 0;          // index of leftmost element
		int right = maxIntensity; // index of rightmost element
		while (left < right) {
			// exchange the left and right elements
			int temp = hist[left];
			hist[left] = hist[right];
			hist[right] = temp;
			// move the bounds toward the center
			left++;
			right--;
		}
		min = maxIntensity - min2;
		max = maxIntensity - max;
	}

	if (min == max) {
		//IJ.log("Triangle:  min == max.");
		thresholdValue =  min;
		output = binarize(gray, thresholdValue);
		return output;

	}

	// describe line by nx * x + ny * y - d = 0
	double nx, ny, d;
	// nx is just the max frequency as the other point has freq=0
	nx = hist[max];   //-min; // data[min]; //  lowest value bmin = (p=0)% in the image
	ny = min - max;
	d = sqrt(nx * nx + ny * ny);
	nx /= d;
	ny /= d;
	d = nx * min + ny * hist[min];

	// find split point
	int split = min;
	double splitDistance = 0;
	for (int i = min + 1; i <= max; i++) {
		double newDistance = nx * i + ny * hist[i] - d;
		if (newDistance > splitDistance) {
			split = i;
			splitDistance = newDistance;
		}
	}
	split--;

	if (inverted) {
		// The histogram might be used for something else, so let's reverse it back
		int left = 0;
		int right = maxIntensity;
		while (left < right) {
			int temp = hist[left];
			hist[left] = hist[right];
			hist[right] = temp;
			left++;
			right--;
		}
		thresholdValue = (maxIntensity - split);
		output = binarize(gray, thresholdValue);
		return output;

	}
	else thresholdValue = split;

	output = binarize(gray, thresholdValue);
	return output;
}
