#include "imgProcessing.h"

Mat* imgFilter(const Mat& img, const Mat& filterMat)
{
	CV_Assert(!img.empty());

    Mat* result = nullptr;

	if (img.rows != 0 && img.cols != 0)
	{
		result = new Mat();
		Point anchor = Point(-1, -1); // Center of filter matrics
		int delta = 0; // A value to be added to each pixel during the convolution
		int ddepth = -1; // The depth of dst. A negative value (such as -1) indicates that the depth is the same as the source.

		filter2D(img, *result, ddepth, filterMat, anchor, delta, BORDER_DEFAULT);
	}
	
    return result;
}

Mat * imgCrop(const int x, const int y, const int width, const int height, Mat& img)
{
	Mat* result = nullptr;

	if (img.rows != 0 && img.cols != 0 && width != 0 && height != 0)
	{
		result = new Mat(height, width, CV_8UC1);
		uchar *in, *out;

		for (int i = y, iout = 0; i < y + height; i++, iout++)
		{
			in = img.ptr<uchar>(i);
			out = result->ptr<uchar>(iout);
			for (int j = x, jout = 0; j < x + width; j++, jout++)
			{
				out[jout] = in[j];
			}
		}
	}

	return result;
}
