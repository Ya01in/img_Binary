#pragma once
#include "packageEtFunctions.h"
#include "segmentation.h"
#include <string>
#include <iostream>
int start()
{
    int adaptiveFlag = 0
        ;//default is unadaptive
    cv::Mat src;        //image 
    cv::Mat gray;       //grayscaled image
    cv::Mat bied;       //binarized image
    string image_path;  //image filepath
    string resultPath;
    int mode;
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
    cv::namedWindow(grayWindow, WINDOW_NORMAL);
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
    std::cout << std::endl;
    if (mode < 0 || mode > 16)
    {
        std::cout << "please choose within the number 0 to 16\n";
        return -1;
    }
    else if (mode == 0) return 0;
    //===============================
    std::cout << "Adaptive or not? (y/n)\n";
    char ch = 'q';
    std::cin >> ch;
    if (ch == 'y')
    {
        adaptiveFlag = 1;
    }
    else if (ch == 'n')
    {
        adaptiveFlag = 0;
    }
    else
    {
        std::cout << "ERROR: enter y or n only.\n";
        return 0;
    }
    //=============================== adaptive like method by processing each row.
    if (adaptiveFlag == 1)
    {
        cv::InputArrayOfArrays;
        int layer = 3144;
        Mat subImgs[3144];
        vector<Mat> subImgs_push;
        //Mat subImgs[3200];
        int step = gray.rows / layer;
        std::cout << gray.cols << std::endl;
        std::cout << gray.rows << std::endl;

        for (int i = 0; i < layer; i++)
        {
            //int upperCol = (step * (i + 1) - 1 < gray.rows) ? step * (i + 1) - 1 : gray.rows;
            cv::Rect sub(0, i * step, gray.cols, step);
            //  std::cout << "step " << i << " range: [" << 0 << " ," << i * step << " ," << gray.cols << " ," << step << "]" << endl;

            subImgs[i] = gray(sub);
            subImgs[i] = mySegmentation(subImgs[i], mode, &resultPath);
            subImgs_push.push_back(subImgs[i]);
        }

        /* for (int i = layer; i != 1 ; i/=2)
         {
             for (int j = 0; j < i; j+=2)
             {
                 cv::vconcat(subImgs[j], subImgs[j + 1], subImgs[j/2]);
             }
         }*/


        cv::Mat result;
        cv::vconcat(subImgs_push, result);
        bied = result;
        resultPath += "_adaptive";

    }
    else
    {
        bied = mySegmentation(gray, mode, &resultPath);
        resultPath += "_normal";
    }

    //show and ask to save
    if (bied.empty())
    {
        std::cout << "ERROR: this method may not suit this image.\n";
        return -1;
    }
    const char* output = "result, press ENTER to save image or SPACEBAR to exit";
    cv::namedWindow(output, WINDOW_FULLSCREEN);
    cv::imshow(output, bied);
    std::cout << "press ENTER to save image or SPACEBAR to quit.....\n";
    if (waitKey(0) == 13)//
    {
        destroyAllWindows();
        resultPath += ".jpg";
        cv::imwrite(resultPath, bied);
    }
    else destroyAllWindows();

    return 0;

}