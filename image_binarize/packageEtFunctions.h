#pragma once
#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#define maxIntensity 255
#define Black 0
#define White 255

using namespace cv;

//function for getting histogram of the image.
//int* hist = getHist(gray);
int* getHist(Mat& img) 
{
	int *hist = new int [maxIntensity+1]();
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			int intensity =  (int)img.at<uchar>(i, j);
			hist[intensity] += 1;
		}
	}
	
	return hist;
}

//function for binarizing image using given thresholdValue.
Mat binarize(Mat& img, int th_value)
{
	Mat bied = img.clone();
	int value = 0;
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			int intensity = (int)img.at<uchar>(i, j);
			if (intensity >= th_value) value = White;
			else value = Black;
			bied.at<uchar>(i, j) = value;
		}
	}

	return bied;
}

bool bimodalTest(double* histogram)
{
	//int len = sizeof(histogram) / sizeof(histogram[0]);
	int mode = 0;
	for (int i = 1; i <= maxIntensity; i++)
	{
		if (histogram[i - 1] < histogram[i] && histogram[i + 1] < histogram[i])
		{
			mode++;
			if (mode > 2)
			{
				//std::cout << "mode over 2\n";
				std::cout << "number of mode: " << mode << std::endl;
				return false;
			}
		}
	}
	if (mode == 2)
	{
		std::cout << "Bimodel image created.\n";
		return true;
	}
}
