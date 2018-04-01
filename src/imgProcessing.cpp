#include "imgProcessing.h"

Mat* imgFilter(const Mat& img, const Mat& filterMat)
{
	CV_Assert(!img.empty());

    Mat* result = new Mat();
	Point anchor = Point(-1, -1); // Center of filter matrics
	int delta = 0; // A value to be added to each pixel during the convolution
	int ddepth = -1; // The depth of dst. A negative value (such as -1) indicates that the depth is the same as the source.

    filter2D(img, *result, ddepth, filterMat, anchor, delta, BORDER_DEFAULT);
	
    return result;
}