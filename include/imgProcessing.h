#pragma once
#include "openCV.h"

const Mat verticalDetectionMat = (Mat_<int>(3, 3) << 
		-1, 0, 1,
		-1, 0, 1, 
		-1, 0, 1);
const Mat horizontalDetectionMat = (Mat_<int>(3, 3) << 
        -1, -1, -1,
        0, 0, 0, 
        1, 1, 1);

Mat* imgFilter(const Mat& img, const Mat& filterMat);
Mat* imgCrop(const int x, const int y, const int width, const int height, Mat& img);
Mat* imgGetGray(Mat& img);
Mat* imgLocalThresh(Mat& img, const int xSegments, const int ySegments);
Mat* imgDeskew(Mat& img, Point left, Point right, int height);