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
	int hist[maxIntensity+1] = { 0 };
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			int intensity =  (int)img.at<uchar>(i, j);
			hist[intensity] += 1;
		}
	}
	
	return &hist[0];
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