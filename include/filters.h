#pragma once

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

const Mat verticalDetectionMat = (Mat_<int>(3, 3) << 
		-1, 0, 1,
		-1, 0, 1, 
		-1, 0, 1);
const Mat horizontalDetectionMat = (Mat_<int>(3, 3) << 
        -1, -1, -1,
        0, 0, 0, 
        1, 1, 1);

Mat* filter(const Mat& image, const Mat& filterMat);