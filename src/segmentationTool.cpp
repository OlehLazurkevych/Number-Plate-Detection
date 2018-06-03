#include "segmentationTool.h"

SegmentationTool::SegmentationTool()
{
	// Empty
}

vector<Mat> SegmentationTool::getSegments(Mat& image)
{
	mSegments.clear();
	//deskew(image);
	cutSegments(image);
	removeTrash();
	return mSegments;
}

void SegmentationTool::cutSegments(Mat& image)
{
	Mat* source = imgLocalThresh(image, 3, 2);
	Mat curr;
	Point glPosP;

	for (int x = 0; x < source->cols; x++)
	{
		for (int y = 0; y < source->rows; y++)
		{
			if (source->at<uchar>(Point(x, y)) == 0)
			{
				curr = extractParticle(*source, x, y, glPosP);
				if (!curr.empty())
				{
					if (curr.rows * SEGMENT_HIGHT_CONSTANT > source->rows)
					{
						if (curr.cols > curr.rows)
						{
							mSegments.push_back(Mat()); // Probably dirt
						}
						else
						{
							mSegments.push_back(curr); // Potential character
						}
					}
				}
			}
		}
	}

	if (source)
	{
		delete source;
	}
}

Mat SegmentationTool::extractParticle(Mat& segment, const int seedX, const int seedY, Point& globalTopLeft)
{
	const uchar BGCOLOR = 255;
	const uchar CHCOLOR = 0;

	Mat result = Mat(segment.rows, segment.cols, CV_8UC1, Scalar(BGCOLOR));

	stack<Point> points;
	Point point;
	points.push(Point(seedX, seedY));

	int minX = segment.cols;
	int maxX = 0;
	int minY = segment.rows;
	int maxY = 0;

	while (!points.empty())
	{
		point = points.top();
		points.pop();
		segment.at<uchar>(point) = BGCOLOR;
		result.at<uchar>(point) = CHCOLOR;
		
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
	
	globalTopLeft = Point(minX, minY);

	Mat* cropped = imgCrop(minX, minY, maxX - minX + 1, maxY - minY + 1, result);
	if (cropped)
	{
		result = *cropped;
		delete cropped;
	}
	else
	{
		result = Mat();
	}
	return result;
}

void SegmentationTool::removeTrash()
{
	if (mSegments.size() >= MIN_SEGMENTS_CONSTANT)
	{
		int start = 0;
		int end = mSegments.size();
		int diver = 0;
		pair<double, double> mean(0, 0);

		mean = pair<double, double>(0, 0);

		if (!mSegments[1].empty())
		{
			mean.first += mSegments[1].cols;
			mean.second += mSegments[1].rows;
			++diver;
		}

		if (!mSegments[2].empty())
		{
			mean.first += mSegments[2].cols;
			mean.second += mSegments[2].rows;
			++diver;
		}

		if (!mSegments[mSegments.size() - 3].empty())
		{
			mean.first += mSegments[mSegments.size() - 3].cols;
			mean.second += mSegments[mSegments.size() - 3].rows;
			++diver;
		}

		if (!mSegments[mSegments.size() - 2].empty())
		{
			mean.first += mSegments[mSegments.size() - 2].cols;
			mean.second += mSegments[mSegments.size() - 2].rows;
			++diver;
		}

		if (diver >= 2)
		{
			mean.first /= diver;
			mean.second /= diver;

			if (!mSegments[0].empty() && abs(mean.first - mSegments[0].cols) + abs(mean.second - mSegments[0].rows) > TRASH_DIF_CONSTANT)
			{
				++start;
			}
			if (!mSegments[mSegments.size() - 1].empty() && abs(mean.first - mSegments[mSegments.size() - 1].cols) + abs(mean.second - mSegments[mSegments.size() - 1].rows) > TRASH_DIF_CONSTANT)
			{
				--end;
			}
		}

		vector<Mat> result;
		for (int i = start; i < end; i++)
		{
			result.push_back(mSegments[i]);
		}
		mSegments = result;
	}
}

void SegmentationTool::deskew(Mat& image)
{
	Mat* localThresh = imgLocalThresh(image, 3, 2);
	pair<Point, Point> points;
	int charHeight = 0;
	Mat curr;
	int count = 0;
	uchar CHCOLOR = 0; // Black pixel of posible character
	Point glPosPoint;

	for (int x = 0; x < localThresh->cols; x++)
	{
		for (int y = 0; y < localThresh->rows; y++)
		{
			if (localThresh->at<uchar>(Point(x, y)) == CHCOLOR)
			{
				curr = extractParticle(*localThresh, x, y, glPosPoint);
				if (!curr.empty())
				{
					if (curr.rows * SEGMENT_HIGHT_CONSTANT > localThresh->rows && curr.cols < curr.rows && curr.cols * 2 > curr.rows)
					{
						Window::Draw(curr);
						if (count == 1)
						{
							points.first = glPosPoint;
							charHeight = curr.rows + 1;
						}
						else if(count != 0 && count < 6)
						{
							points.second = glPosPoint;
						}
						count++;
					}
				}
			}
		}
	}

	if (charHeight > 0)
	{
		Window::Draw(image);
		image = *imgDeskew(image, points.first, points.second, charHeight);
	}

	if (localThresh)
	{
		delete localThresh;
	}
}