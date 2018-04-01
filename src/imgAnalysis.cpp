#include "imgAnalysis.h"

vector<int>* imgProjection(Mat & img, bool horizontal)
{
	CV_Assert(img.depth() == CV_8UC1);
	CV_Assert(!img.empty());
	
	int nRows = img.rows;
	int nCols = img.cols;

	vector<int>* projection = new vector<int>((horizontal) ? nCols : nRows);
	uchar* p;

	if (horizontal)
	{
		for (int i = 0; i < nCols; ++i)
		{
			projection->at(i) = 0;
			for (int j = 0; j < nRows; ++j)
			{
				p = img.ptr<uchar>(j);
				projection->at(i) += p[i];
			}
		}
	}
	else
	{
		for (int i = 0; i < nRows; ++i)
		{
			p = img.ptr<uchar>(i);
			projection->at(i) = 0;
			for (int j = 0; j < nCols; ++j)
			{
				projection->at(i) += p[j];
			}
		}
	}

	return projection;
}

vector<int>* vecRankFilter(vector<int>& vec, int rankSize)
{
	vector<int>* result = new vector<int>(vec.size());
	int step = rankSize / 2;
	int backVal = 0;
	int frontVal = 0;

	/*for (int i = 0; i < step; i++)
	{
		result->at(i) = vec[i];
	}*/
	
	for (int i = step; i < vec.size() - step; i++)
	{
		backVal = 0;
		frontVal = 0;

		for (int r = i; r >= i - step; r--)
		{
			backVal += vec[r];
		}
		for (int f = i + 1; f <= i + step; f++)
		{
			frontVal += vec[f];
		}

		result->at(i) = (backVal + frontVal) / rankSize;
	}

	/*for (int i = vec.size() - step; i < vec.size(); i++)
	{
		result->at(i) = vec[i];
	}*/

	return result;
}
