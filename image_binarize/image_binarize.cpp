// image_binarize.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
// this code is for image segmentation using various methods
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include "thresholdind.h"

using namespace cv;


int main()
{
    cv::Mat src;        //image 
    cv::Mat gray;       //grayscaled image
    cv::Mat bied;       //binarized image
    String image_path;  //image filepath
    String resultPath;
    uint mode;
    //entrance interface
    std::cout << "================================\n";
    std::cout << "Image Binarization......\n";
    std::cout << "================================\n";
    //get target file path
    while (src.empty())
    {
        std::cout << "Please enter your target image path: ";
        std::cin >> image_path;
        src = imread(image_path);
        if (src.empty())
        {
            std::cout << "Could not open or find the image!" << std::endl;
            std::cout << "Please retry or exit the program.";
        }
        else
        {
            break;
        }
    }
    const char* sourceWindow = "source, PRESS ANY KEY TO CONTINUE.";
    cv::namedWindow(sourceWindow, WINDOW_FULLSCREEN);
    cv::imshow(sourceWindow, src);
    //destroyWindow(sourceWindow);
    //grayscaled source image
    gray = src.clone();
    const char* grayWindow = "grayscaled image, PRESS ANY KEY TO CONTINUE.";
    cvtColor(gray, gray, COLOR_BGR2GRAY);
    cv::namedWindow(grayWindow, WINDOW_FULLSCREEN);
    cv::imshow(grayWindow, gray);
    waitKey(0);
    destroyAllWindows();
    //choose mode
    std::cout << "\n================================\n";
    std::cout << "Image Binarization Methods:\n";
    std::cout << "================================\n";
    std::cout << "(1)interative    (2)huang         (3)interModes    (4)isoData\n\n";
    std::cout << "(5)Li            (6)maxEntropy    (7)mean          (8)minError\n\n";
    std::cout << "(9)minimum       (10)moments      (11)Otsu         (12)percentile\n\n";
    std::cout << "(13)renyEntropy  (14)shanbhag     (15)triangle     (16)yen\n\n";
    std::cout << "Choose Binarization Method, or ENTER 0 to exit:";
    std::cin >> mode;
    std::cout<< std::endl;
    if (mode < 0 || mode > 16)
    {
        std::cout << "please choose within the number 0 to 16\n";
        return -1;
    }
    switch (mode)
    {
    case 1:
        resultPath = "./seg_interative.jpg";
        bied = seg_interative(gray);
        break;
        
    case 2:
        resultPath = "./seg_huang.jpg";
        break;

    case 3:
        resultPath = "./seg_interModes.jpg";
        break;

    case 4:
        resultPath = "./seg_isoData.jpg";
        break;

    case 5:
        resultPath = "./seg_Li.jpg";
        break;

    case 6:
        resultPath = "./seg_maxEntropy.jpg";
        break;

    case 7:
        resultPath = "./seg_mean.jpg";
        break;

    case 8:
        resultPath = "./seg_minError.jpg";
        break;

    case 9:
        resultPath = "./seg_minimum.jpg";
        break;

    case 10:
        resultPath = "./seg_moments.jpg";
        break;

    case 11:
        resultPath = "./seg_Otsu.jpg";
        cv::threshold(gray, bied, 0, 255, THRESH_OTSU);
        break;

    case 12:
        resultPath = "./seg_percentile.jpg";
        break;

    case 13:
        resultPath = "./seg_renyEntropy.jpg";
        break;

    case 14:
        resultPath = "./seg_shanbhag.jpg";
        break;

    case 15:
        resultPath = "./seg_triangle.jpg";
        break;

    case 16:
        resultPath = "./seg_yen.jpg";
        break;

    default:
        return 0;
    }
    //show and ask to save
    const char* output = "result, press ENTER to save image or SPACEBAR to exit";
    cv::namedWindow(output, WINDOW_FULLSCREEN);
    cv::imshow(output, bied);
    std::cout << "press ENTER to save image or SPACEBAR to exit.....\n";
    if (waitKey(0) == 13)
    {
        cv::imwrite(resultPath, bied);
    }
    
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
