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

	if (img.rows != 0 && img.cols != 0 && width > 0 && height > 0)
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

Mat* imgGetGray(Mat& img)
{
	Mat* result = new Mat(img.rows, img.cols, CV_8UC1);
	uchar* in, *out;

	for (int i = 0; i < img.rows; i++)
	{
		in = img.ptr<uchar>(i);
		out = result->ptr<uchar>(i);

		for (int jin = 1, jout = 0; jout < result->cols; jout++)
		{
			out[jout] = (in[jin - 1] + in[jin] + abs( in[jin + 1] - 70)) / 3;

			jin += 3;
		}
	}

	return result;
}

Mat* imgLocalThresh(Mat& img, const int widthSegments, const int heightSegments)
{
	Mat* result = new Mat(img.rows, img.cols, CV_8UC1, Scalar(0));
	uchar *in, *out;

	vector<int> xSegments{ 0 };
	vector<int> ySegments{ 0 };

	int widthStep = img.cols / (widthSegments + 1);
	int heightStep = img.rows / (heightSegments + 1);

	for (int i = 1; i <= widthSegments; i++)
	{
		xSegments.push_back(xSegments[i - 1] + widthStep);
	}
	for (int i = 1; i <= heightSegments; i++)
	{
		ySegments.push_back(ySegments[i - 1] + heightStep);
	}

	xSegments.push_back(img.cols);
	ySegments.push_back(img.rows);

	int xEdgeIndx = 0;
	int yEdgeIndx = 0;

	int sum = 0;
	int quantity = 0;
	int mean = 0;

	for (int yChunk = 0; yChunk <= heightSegments; yChunk++)
	{
		for (int xChunk = 0; xChunk <= widthSegments; xChunk++)
		{
			for (int i = ySegments[yEdgeIndx]; i < ySegments[yEdgeIndx + 1]; i++)
			{
				in = img.ptr<uchar>(i);
				for (int j = xSegments[xEdgeIndx]; j < xSegments[xEdgeIndx + 1]; j++)
				{
					sum += in[j];
					++quantity;
				}
			}

			mean = sum / quantity;

			for (int i = ySegments[yEdgeIndx]; i < ySegments[yEdgeIndx + 1]; i++)
			{
				in = img.ptr<uchar>(i);
				out = result->ptr<uchar>(i);
				for (int j = xSegments[xEdgeIndx]; j < xSegments[xEdgeIndx + 1]; j++)
				{
					if (in[j] > mean)
					{
						out[j] = 255;
					}
				}
			}

			++xEdgeIndx;
		}
		xEdgeIndx = 0;
		++yEdgeIndx;
	}

	return result;
}

Mat* imgDeskew(Mat& img, Point left, Point right, int height)
{
	Mat* result = new Mat(height, img.cols, CV_8UC1, Scalar(255));
	double m = ((double)right.y - (double)left.y) / ((double)right.x - (double)left.x);
	double x2 = (double)right.x;
	double y2 = (double)right.y;
	int ay;

	for (int x = 0; x < img.cols; x++)
	{
		ay = m * ((double)x - x2) + y2;

		if (ay < img.rows && ay >= 0)
		{
			for (int y = ay, j = 0; y < ay + height && y < img.rows; y++, j++)
			{
				result->at<uchar>(Point(x, j)) = img.at<uchar>(Point(x, y));
			}
		}
	}

	return result;
}