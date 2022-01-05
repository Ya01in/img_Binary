#pragma once

#include "iterativeBinarization.h"
#include "interModesBinarization.h"
#include "liBinarization.h"
#include "meanBinarization.h"
#include "minimumBinarization.h"
#include "mutipleBinarization.h"
#include "renyEntropyBinarization.h"
#include "triangleBinarization.h"

Mat segmentation(Mat& gray,int mode,String* resultPath)
{
    cv::Mat bied;       //binarized image

    switch (mode)
    {
    case 1:
        *resultPath = "./seg_interative";
        bied = iterative(gray);
        break;

    case 2:
        *resultPath = "./seg_huang";
        bied = huang(gray);
        break;

    case 3:
        *resultPath = "./seg_interModes";
        bied = interModes(gray);
        break;

    case 4:
        *resultPath = "./seg_isoData";
        bied = isoData(gray);
        break;

    case 5:
        *resultPath = "./seg_Li";
        bied = li(gray);
        break;

    case 6:
        *resultPath = "./seg_maxEntropy";
        bied = maxEntropy(gray);
        break;

    case 7:
        *resultPath = "./seg_mean";
        bied = meanSeg(gray);
        break;

    case 8:
        *resultPath = "./seg_minError";
        bied = minError(gray);
        break;

    case 9: //BUG
        *resultPath = "./seg_minimum";
        bied = mini(gray);
        break;

    case 10:
        *resultPath = "./seg_moments";
        bied = moments(gray);
        break;

    case 11:
        *resultPath = "./seg_Otsu";
        cv::threshold(gray, bied, 0, 255, THRESH_OTSU);
        break;

    case 12:
        *resultPath = "./seg_percentile";
        bied = percentile(gray);
        break;

    case 13:
        *resultPath = "./seg_renyEntropy";
        bied = reny(gray);
        break;

    case 14:
        *resultPath = "./seg_shanbhag";
        bied = shanbhag(gray);
        break;

    case 15:
        *resultPath = "./seg_triangle";
        bied = triangle(gray);
        break;

    case 16:
        *resultPath = "./seg_yen";
        bied = Yen(gray);
        break;

    default:
        return bied;
    }
    
    return bied;
}