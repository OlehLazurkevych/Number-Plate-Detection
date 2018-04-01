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