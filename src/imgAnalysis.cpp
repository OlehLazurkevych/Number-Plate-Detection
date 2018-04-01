#include "imgAnalysis.h"

vector<int>* imgProjection(Mat & img, bool horizontal)
{
	CV_Assert(img.depth() == CV_8UC1);
	CV_Assert(!img.empty());
	
	int nRows = img.rows;
	int nCols = img.cols;

	vector<int>* projection = new vector<int>((horizontal) ? nCols : nRows);

	/*if (img.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}*/

	uchar* p;
	double projVal;

	if (horizontal)
	{
		for (int i = 0; i < nCols; ++i)
		{
			projVal = 0;
			for (int j = 0; j < nRows; ++j)
			{
				p = img.ptr<uchar>(j);
				projVal += p[i];
			}
			projection->at(i) = projVal;
		}
	}
	else
	{
		for (int i = 0; i < nRows; ++i)
		{
			p = img.ptr<uchar>(i);
			projVal = 0;
			for (int j = 0; j < nCols; ++j)
			{
				projVal += p[j];
			}
			projection->at(i) = projVal;
		}
	}

	return projection;
}
