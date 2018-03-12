#include "filters.h"

Mat* filter(const Mat& image, const Mat& filterMat)
{
    Mat* result = new Mat();
	Point anchor = Point(-1, -1); // Center of filter matrics
	int delta = 0; // A value to be added to each pixel during the convolution
	int ddepth = -1; // The depth of dst. A negative value (such as -1) indicates that the depth is the same as the source.

    filter2D(image, *result, ddepth, filterMat, anchor, delta, BORDER_DEFAULT);
	
    return result;
}