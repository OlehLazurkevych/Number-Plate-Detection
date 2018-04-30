#include "segmentationTool.h"

SegmentationTool::SegmentationTool()
{
	// Empty
}

SegmentationTool::~SegmentationTool()
{
	// Empty
}

vector<Mat> SegmentationTool::getSegments(Mat& image)
{
	mSegments.clear();
	cutSegments(image);
	extractCharacters();
	removeTrash();
	return mSegments;
}

void SegmentationTool::cutSegments(Mat& image)
{
	Mat* source = imgLocalThresh(image, 3, 2);
	Mat* segment = nullptr;
	vector<int>* projection = imgProjection(*source, true);
	int threshHold = (*max_element(projection->begin(), projection->end())) * CUT_PEEK_CONSTANT;
	vector<pair<int, int>> edges = firstPeekAndFlat(*projection, threshHold);

	for (int i = 0; i < edges.size() - 1; i++)
	{
		if (segment)
		{
			delete segment;
		}
		segment = imgCrop(edges[i].second, 0, edges[i + 1].first - edges[i].second, source->rows, *source);
		if (segment)
		{
			mSegments.push_back(*segment);
		}
	}
	delete source;
	delete projection;
}

vector<pair<int, int>> SegmentationTool::firstPeekAndFlat(vector<int>& projection, const int threshHold)
{
	vector<pair<int, int>> result{ pair<int, int>(0,0) };
	bool found = false;

	for (int i = 0; i < projection.size(); i++)
	{
		if (!found && projection[i] >= threshHold)
		{
			result.push_back(pair<int, int>(i,0));
			found = true;
		}
		else if (found && projection[i] <= threshHold)
		{
			result[result.size() - 1].second = i;
			found = false;
		}
	}

	result.push_back(pair<int, int>(projection.size(), 0));

	return result;
}

void SegmentationTool::extractCharacters()
{
	list<pair<Mat, int>> particles;
	pair<Mat, int>* curr = nullptr;
	uchar* segmentPtr;

	for (int i = 0; i < mSegments.size(); i++)
	{
		for (int y = 0; y < mSegments[i].rows; y++)
		{
			segmentPtr = mSegments[i].ptr<uchar>(y);
			for (int x = 0; x < mSegments[i].cols; x++)
			{
				if (segmentPtr[x] == 0)
				{
					curr = extractParticle(mSegments[i], x, y);
					if (curr)
					{
						particles.push_back(*curr);
						delete curr;
					}
				}
			}
		}
		mSegments[i] = max_element(particles.begin(), particles.end(), 
								[](pair<Mat, int> first, pair<Mat, int> second)
								{ 
									return first.second < second.second;
								})->first;
		particles.clear();
	}
}

pair<Mat, int>* SegmentationTool::extractParticle(Mat& segment, const int seedX, const int seedY)
{
	const uchar BGCOLOR = 255;
	const uchar CHCOLOR = 0;

	pair<Mat, int>* result = new pair<Mat, int>(Mat(segment.rows, segment.cols, CV_8UC1, Scalar(BGCOLOR)), 0);

	stack<Point> points;
	Point point;
	points.push(Point(seedX, seedY));

	int minX = segment.cols;
	int maxX = 0;
	int minY = segment.rows;
	int maxY = 0;
	int whiteQuantity = 0;

	while (!points.empty())
	{
		point = points.top();
		points.pop();
		segment.at<uchar>(point) = BGCOLOR;
		result->first.at<uchar>(point) = CHCOLOR;
		
		if (point.x < minX)
		{
			minX = point.x;
		}
		if (point.x > maxX)
		{
			maxX = point.x;
		}
		if (point.y < minY)
		{
			minY = point.y;
		}
		if (point.y > maxY)
		{
			maxY = point.y;
		}

		if (point.y - 1 >= 0)
		{
			if (segment.at<uchar>(Point(point.x, point.y - 1)) == CHCOLOR)
			{
				points.push(Point(point.x, point.y - 1));
			}
		}
		if (point.x + 1 < segment.cols)
		{
			if (segment.at<uchar>(Point(point.x + 1, point.y)) == CHCOLOR)
			{
				points.push(Point(point.x + 1, point.y));
			}
		}
		if (point.y + 1 < segment.rows)
		{
			if (segment.at<uchar>(Point(point.x, point.y + 1)) == CHCOLOR)
			{
				points.push(Point(point.x, point.y + 1));
			}
		}
		if (point.x - 1 >= 0)
		{
			if (segment.at<uchar>(Point(point.x - 1, point.y)) == CHCOLOR)
			{
				points.push(Point(point.x - 1, point.y));
			}
		}
	}
		
	Mat* cropped = imgCrop(minX, minY, maxX - minX + 1, maxY - minY + 1, result->first);
	if (cropped)
	{
		for (int y = 0; y < cropped->rows; y++)
		{
			for (int x = 0; x < cropped->cols; x++)
			{
				if (cropped->at<uchar>(y, x) == BGCOLOR)
				{
					++whiteQuantity;
				}
			}
		}

		result->first = *cropped;
		result->second = whiteQuantity;
		delete cropped;
	}
	else
	{
		delete result;
		result = nullptr;
	}
	return result;
}

void SegmentationTool::removeTrash()
{
	if (mSegments.size() >= MIN_SEGMENTS_CONSTANT)
	{
		Mat* blunk = new Mat;		
		pair<double, double> mean(0, 0);

		for (int i = 0; i < mSegments.size(); i++)
		{
			mean.first += mSegments[i].cols;
			mean.second += mSegments[i].rows;
		}

		mean.first /= mSegments.size();
		mean.second /= mSegments.size();

		for (int i = 0; i < mSegments.size(); i++)
		{
			if (mSegments[i].cols + mSegments[i].rows < (mean.first + mean.second) / 3)
			{
				mSegments[i] = *blunk;
			}
		}

		mean = pair<double, double>(0, 0);

		mean.first += mSegments[1].cols;
		mean.second += mSegments[1].rows;

		mean.first += mSegments[2].cols;
		mean.second += mSegments[2].rows;

		mean.first += mSegments[mSegments.size() - 3].cols;
		mean.second += mSegments[mSegments.size() - 3].rows;

		mean.first += mSegments[mSegments.size() - 2].cols;
		mean.second += mSegments[mSegments.size() - 2].rows;

		mean.first /= 4;
		mean.second /= 4;

		if (abs(mean.first - mSegments[0].cols) + abs(mean.second - mSegments[0].rows) > TRASH_DIF_CONSTANT)
		{
			mSegments[0] = *blunk;
		}
		if (abs(mean.first - mSegments[mSegments.size() - 1].cols) + abs(mean.second - mSegments[mSegments.size() - 1].rows) > TRASH_DIF_CONSTANT)
		{
			mSegments[mSegments.size() - 1] = *blunk;
		}

		vector<Mat> result;
		for (int i = 0; i < mSegments.size(); i++)
		{
			if (!mSegments[i].empty())
			{
				result.push_back(mSegments[i]);
			}
		}
		mSegments = result;
		delete blunk;
	}
	else
	{
		mSegments.clear();
	}
}