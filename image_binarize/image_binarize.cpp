// image_binarize.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
// this code is for image segmentation using various methods
//
#include "gateWay.h"
#include "packageEtFunctions.h"
#include "segmentation.h"
#include <opencv2/imgproc.hpp>
#include <string>
#include <iostream>

using namespace cv;


int main()
{
// manual Segmentation Selection

    //bool flag = true;
    //char a;
    //int o;
    //while (flag == true)
    //{
    //    o = start();
    //    std::cout << "Enter q to exit or Other to process another image:\n";
    //    std::cin >> a;
    //    if (a == 'q')//process another image
    //    {
    //        break;
    //    }
    //}

    cv::Mat src;        //image 
    cv::Mat gray;       //grayscaled image
    cv::Mat bied;       //binarized image
    string image_path;  //image filepath
    string resultPath;
    string target = "C:/Users/Li/Desktop/LuxVsion/diff_flare_data/";

// Segment with all methods Script

    //for (int i = 0; i < 4; i++)
    //{
    //    resultPath = target + to_string(i);
    //    src = imread(resultPath+".jpg");
    //    gray = src.clone();
    //    cvtColor(gray, gray, COLOR_BGR2GRAY);
    //    std::cout << resultPath + ".jpg \n";
    //    if (!gray.empty())
    //    {
    //        std::cout << "processing" << endl; 
    //        for (int mode = 0; mode <= 16; mode++)
    //        {
    //            bied = mySegmentation(gray, mode, &resultPath);
    //            if (!bied.empty())
    //            {
    //                resultPath += to_string(i)+"_normal.jpg";
    //                cv::imwrite(resultPath, bied);
    //                std::cout << "image write.\n";
    //            }
    //            else
    //            {
    //                std::cout << "no image create.\n";
    //            }
    //            
    //        }
    //    }
    //    else
    //    {
    //        std::cout << "cannot read file. \n";
    //    }
    //}
    
//Creating image Histogram

    //for (int i = 0; i <= 4; i++)
    //{
    //    cout << i << endl;
    //    resultPath = target + to_string(i);
    //    src = imread(resultPath + ".jpg");
    //    cvtColor(src, src, COLOR_BGR2GRAY);
    //    if (src.empty())
    //    {
    //        cout << "can't read file.\n";
    //        return -1;
    //    }
    //    int histSize = 256;
    //    float range[] = { 0, 256 }; //the upper boundary is exclusive
    //    const float* histRange[] = { range };
    //    bool uniform = true, accumulate = false;
    //    Mat intensityHist;
    //    calcHist(&src, 1, 0, Mat(), intensityHist, 1, &histSize, histRange, uniform, accumulate);
    //    int hist_w = 300, hist_h = 200;
    //    int bin_w = cvRound((double)hist_w / histSize);
    //    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
    //    normalize(intensityHist, intensityHist, 0, 100, NORM_MINMAX, -1, Mat());
    //    for (int i = 1; i < histSize; i++)
    //    {
    //        cout << i << endl;
    //        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(intensityHist.at<int>(i - 1))),
    //                        Point(bin_w * (i), hist_h - cvRound(intensityHist.at<int>(i))),
    //            Scalar(137, 255, 196), 2, 8, 0);
    //    }
    //    //imshow("Source image", src);
    //    //imshow("calcHist Demo", histImage);
    //    //waitKey(0);
    //    //destroyAllWindows();
    //    imwrite(resultPath + "hist.jpg", histImage);
    //}

//(一) src-> high-pass filter -> equalize -> threshold -> dilate-> erode-> findContours-> 針對最大的ROI
//11: 3min, 8min
//default: 5: 7sec, 17sec
    int mode = 16;
    resultPath = target + to_string(3);
    src = imread(resultPath + ".jpg");
    cvtColor(src, src, COLOR_BGR2GRAY);
    Mat src1 = src.clone();
    Mat HPimg;
    if (src1.empty())
    {
        cout << "can't read file.\n";
        return -1;
    }
    
    //using Laplacian function
    int kernel_size = 3;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    Laplacian(src, HPimg, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(HPimg, HPimg);
    equalizeHist(HPimg, HPimg);
    //Display the original image
    //namedWindow("Display Image");
    //imshow("Display Image", src);
    
    //Display the High Pass Filtered Image image
    namedWindow("High Pass Filtered Image");
    imshow("High Pass Filtered Image", HPimg);
    cv::imwrite("./3HP_3.jpg", HPimg);
    //thresholding High Pass Image
    bied = mySegmentation(HPimg, mode, &resultPath);
    namedWindow("Thresholded High Pass Filtered Image");
    imshow("Thresholded High Pass Filtered Image", bied);
    cv::imwrite("./3HP_TH.jpg", bied);
    //Morphology
    //closing
    Mat close;
    Mat kernelcl = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
    morphologyEx(bied, close, MORPH_CLOSE, kernelcl);
    namedWindow("closing");
    imshow("closing", close);
    cv::imwrite("./3closing.jpg", close);
    //opening
    Mat open;
    Mat kernelop = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
    morphologyEx(close, open, MORPH_OPEN, kernelop);
    namedWindow("opening");
    imshow("opening", open);
    cv::imwrite("./3opening.jpg", open);

    //Connected Componnent Labeling
    Mat labelImage(src.size(), CV_32S);
    Mat labels, stats, centroids;
    int nLabels = connectedComponentsWithStats(open, labels, stats, centroids);
    cout << "num of labels: " << nLabels << endl;
    int max_size = -1;
    int max_index = -1;
    for (int i = 1; i < nLabels; i++)
    {
        if (stats.at<int>(i, CC_STAT_AREA)>max_size)
        {   
            max_index = i;
            max_size = stats.at<int>(i, CC_STAT_AREA);
        }
    }
    cout << stats.at<int>(max_index, CC_STAT_AREA) << endl;
    cout << centroids.at<double>(max_index, 0)<< "," << centroids.at<double>(max_index, 1) << endl;
    //Segment Out the Largest Componnent
    int x = stats.at<int>(max_index, CC_STAT_LEFT);
    int y = stats.at<int>(max_index, CC_STAT_TOP);
    int width = stats.at<int>(max_index, CC_STAT_WIDTH);
    int height = stats.at<int>(max_index, CC_STAT_HEIGHT);
    Mat largestComp = HPimg(Range(y, height+y), Range(x, width + x));
    //Mat withoutLargest = HPimg.clone();
    //for (int i = 0; i < HPimg.rows; i++)
    //{
    //    for (int j = 0; j < HPimg.cols; j++)
    //    {
    //        if (j>x and j< (width+x))
    //        {
    //            if (i> y and i < (height+y))
    //            {
    //                withoutLargest.at<int>(i, j) = 0;
    //            }
    //            
    //        }
    //    }
    //}
    //thresholding the Largest Componnent
    largestComp = mySegmentation(largestComp, mode, &resultPath);
    //withoutLargest = mySegmentation(withoutLargest, mode, &resultPath);
    namedWindow("largestComp", WINDOW_FULLSCREEN);
    imshow("largestComp", largestComp);
    cv::imwrite("./3largestComp.jpg", largestComp);
    //namedWindow("withoutLargest");
    //imshow("withoutLargest", withoutLargest);
    waitKey(0);

    
    return 0;
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
