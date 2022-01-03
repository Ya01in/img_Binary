#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#define maxIntensity 255

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

Mat seg_interative(Mat& gray)
{
	cv::Mat result;
	int* hist = getHist(gray);
	std::cout << hist[100];

	return result;
}
