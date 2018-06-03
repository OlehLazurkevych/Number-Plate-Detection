#include "charRecognitionTool.h"

CharRecognitionTool::CharRecognitionTool(const string numbersFeaturesFilePath, const string lettersFeaturesFilePath)
{
	fstream numberFeaturesFileStream;
	fstream letterFeaturesFileStream;

	numberFeaturesFileStream.open(numbersFeaturesFilePath, ios::in);
	if (!numberFeaturesFileStream)
	{
		numberFeaturesFileStream.close();
		letterFeaturesFileStream.close();

		throw new exception("Error   :   Could not open or find the numbers text file");
	}
	letterFeaturesFileStream.open(lettersFeaturesFilePath, ios::in);
	if (!letterFeaturesFileStream)
	{
		numberFeaturesFileStream.close();
		letterFeaturesFileStream.close();

		throw new exception("Error   :   Could not open or find the letters text file");
	}

	while (!numberFeaturesFileStream.eof())
	{
		CharFeatures curr;
		numberFeaturesFileStream >> curr;
		mNumbersFeatures.push_back(curr);
	}
	while (!letterFeaturesFileStream.eof())
	{
		CharFeatures curr;
		letterFeaturesFileStream >> curr;
		mLettersFeatures.push_back(curr);
	}

	numberFeaturesFileStream.close();
	letterFeaturesFileStream.close();
}

void CharRecognitionTool::setSegments(vector<Mat> segments)
{
	uint clearSegQuantity = 0;

	mSegments[0].clear();
	mSegments[1].clear();
	mSegments[2].clear();

	mCurrBlock = 1;
	mCurrChar = 0;
	mCharQuantity = 0;

	if (segments.size() > DES_SEG_QUANTITY)
	{
		vector<Mat> New;
		for (int i = 0; i < segments.size(); i++)
		{
			if (!segments[i].empty())
			{
				New.push_back(segments[i]);
			}
			if (New.size() == DES_SEG_QUANTITY)
			{
				break;
			}
		}
		segments = New;
	}

	for (int i = 0; i < segments.size(); i++)
	{
		if (!segments[i].empty())
		{
			++clearSegQuantity;
		}
	}

	if (segments.size() < MIN_SEG_QUANTITY)
	{
		throw exception("Plate not found   :   too less segments for a number plate");
	}
	else if (clearSegQuantity == 0)
	{
		throw exception("Plate not found   :   plate is too dirty");
	}
	else if (segments.size() == DES_SEG_QUANTITY)
	{
		mSegments[0].push_back(Segment(segments[0], Segment::Letter));
		mSegments[0].push_back(Segment(segments[1], Segment::Letter));

		mSegments[1].push_back(Segment(segments[2], Segment::Number));
		mSegments[1].push_back(Segment(segments[3], Segment::Number));
		mSegments[1].push_back(Segment(segments[4], Segment::Number));
		mSegments[1].push_back(Segment(segments[5], Segment::Number));

		mSegments[2].push_back(Segment(segments[6], Segment::Letter));
		mSegments[2].push_back(Segment(segments[7], Segment::Letter));
	}
	else if (segments.size() < DES_SEG_QUANTITY)
	{
		cout << "Warning   :   unclear number plate" << endl;

		double mean = 0;
		int block = 0;
		vector<Segment> segmentVec;
		int normSegQuantity = segments.size();
		Segment curr;

		for (int i = 0; i < segments.size(); i++)
		{
			curr.mSegment = segments[i];
			if (curr.mSegment.empty())
			{
				curr.mType = Segment::Dirt;
				--normSegQuantity;
			}
			else
			{
				mean += segments[i].cols;
			}
			
			segmentVec.push_back(curr);
		}
		mean /= normSegQuantity;

		for (int i = 0; i < segmentVec.size(); i++)
		{
			if (!segmentVec[i].mSegment.empty())
			{
				if (segmentVec[i].mSegment.cols > mean)
				{
					segmentVec[i].mType = Segment::Letter;
				}
				else
				{
					segmentVec[i].mType = Segment::Number;
				}
			}
		}

		for (int i = 0; i < segmentVec.size(); i++)
		{
			if (block == 0)
			{
				if ((segmentVec[i].mType == Segment::Letter || segmentVec[i].mType == Segment::Dirt) && mSegments[block].size() < 2)
				{
					mSegments[block].push_back(segmentVec[i]);
				}
				else
				{
					++block;
				}
			}
			if (block == 1)
			{
				if (mSegments[block].size() < 4)
				{
					if (segmentVec[i].mType == Segment::Letter || segmentVec[i].mType == Segment::Dirt)
					{
						if (segmentVec.size() - 1 - i <= 2)
						{
							++block;
						}
						else
						{
							if (segmentVec[i].mType == Segment::Letter)
							{
								segmentVec[i].mType = Segment::Number;
							}
							mSegments[block].push_back(segmentVec[i]);
						}
					}
					else
					{
						mSegments[block].push_back(segmentVec[i]);
					}
				}
				else
				{
					++block;
				}
			}
			if (block == 2)
			{
				if (segmentVec[i].mType == Segment::Number)
				{
					segmentVec[i].mType = Segment::Letter;
				}
				mSegments[block].push_back(segmentVec[i]);
			}
		}
	}
	
	for (int i = 0; i < mSegments.size(); i++)
	{
		mCharQuantity += mSegments[i].size();
	}
}

char CharRecognitionTool::next()
{
	cout << " ";
	for (int j = 0; j < mSegments[0].size(); j++)
	{
		cout << recognizeChar(mSegments[0][j].mSegment, false);
	}
	cout << " ";
	for (int j = 0; j < mSegments[1].size(); j++)
	{
		cout << recognizeChar(mSegments[1][j].mSegment, true);
	}
	cout << " ";
	for (int j = 0; j < mSegments[2].size(); j++)
	{
		cout << recognizeChar(mSegments[2][j].mSegment, false);
	}

	return 0;
}

void CharRecognitionTool::drawAll()
{
//	char n = '1';
//	string f = "E:/Projects/Number-Plate-Recognition/data/training-chars/t";
//	string s = ".png";/////////////////////////////////////////////////////

	Mat blunk(20, 20, CV_8UC1, Scalar(255));
	for (int i = 0; i < mSegments.size(); i++)
	{
		for (int j = 0; j < mSegments[i].size(); j++)
		{
			//Window::Draw((mSegments[i][j].mSegment.empty()) ? blunk : mSegments[i][j].mSegment);

			//imwrite( f + n++ + s , mSegments[i][j].mSegment);//////////////////////////////////////////

			switch (mSegments[i][j].mType)
			{
				case Segment::Undefined:
				{
					cout << "[" << i << "][Undefined] ";
					break;
				}
				case Segment::Dirt:
				{
					cout << "[" << i << "][ Dirt ] ";
					break;
				}
				case Segment::Number:
				{
					cout << "[" << i << "][Number] ";
					break;
				}
				case Segment::Letter:
				{
					cout << "[" << i << "][Letter] ";
					break;
				}
			}
		}
	}
}

int CharRecognitionTool::getCharQuantity() const
{
	return mCharQuantity;
}

char CharRecognitionTool::recognizeChar(Mat& segment, bool isNumber)
{
	if (!segment.empty())
	{
		/* Segment sceletonization before feature extraction */
		skeletonize(segment);

		/* Feature extraction */
		vector<Point2f> lineEnds = getLEndsVec(segment);
		vector<Point2f> junctions = getJunctionsVec(segment);
		int loopQuantity = getLoopQuantity(segment);
		vector<CharFeatures> charset;
		
		/* Confident selection (by loop and line-ends quantity) */
		if (isNumber)
		{
			for (int i = 0; i < mNumbersFeatures.size(); i++)
			{
				if (loopQuantity == mNumbersFeatures[i].mLoopQuantity && lineEnds.size() == mNumbersFeatures[i].mLineEnds.size())
				{
					charset.push_back(mNumbersFeatures[i]);
				}
			}
		}
		else
		{
			for (int i = 0; i < mLettersFeatures.size(); i++)
			{
				if (loopQuantity == mLettersFeatures[i].mLoopQuantity && lineEnds.size() == mLettersFeatures[i].mLineEnds.size())
				{
					charset.push_back(mLettersFeatures[i]);
				}
			}
		}

		if (charset.size() == 1)
		{
			return charset[0].mChar; // Confident result
		}
		else if (charset.size() > 1)
		{
			char result;
			if (!isNumber && lineEnds.size() == 4)
			{
				/* Complex analysis of junctions */

				result = charset[0].mChar;
				float min = getAllPointsDifference(charset[0].mJunctions, junctions);
				float curr;

				for (int i = 1; i < charset.size(); i++)
				{
					curr = getAllPointsDifference(charset[i].mJunctions, junctions);
					if (curr < min)
					{
						min = curr;
						result = charset[i].mChar;
					}
				}
			}
			else
			{
				/* Complex analysis of line-ends */

				result = charset[0].mChar;
				float min = getAllPointsDifference(charset[0].mLineEnds, lineEnds);
				float curr;

				for (int i = 1; i < charset.size(); i++)
				{
					curr = getAllPointsDifference(charset[i].mLineEnds, lineEnds);
					if (curr < min)
					{
						min = curr;
						result = charset[i].mChar;
					}
				}
			}

			return result; // The closest choise
		}
	}
	return '*'; // Segment is empty
}

void CharRecognitionTool::skeletonize(Mat& segment)
{
	const uchar BGCOLOR = 255;
	const uchar CHCOLOR = 0;
	const uchar MARKER = 100;

	const vector<Point> neighbourhood8Pos = vector<Point>{
		Point(0, -1),  /*  Top            */
		Point(1, -1),  /*  Top Right      */
		Point(1, 0),   /*  Right          */
		Point(1, 1),   /*  Bottom Right   */
		Point(0, 1),   /*  Bottom         */
		Point(-1, 1),  /*  Bottom Left    */
		Point(-1, 0),  /*  Left           */
		Point(-1, -1)  /*  Top Left       */
	};
	vector<bool> neighbourhood8Vals;
	Point curr;

	int blackQuantity = 0;

	bool iterate = true;
	bool highlightThis = false;

	while (iterate)
	{
		iterate = false;

		for (int y = 0; y < segment.rows; y++)
		{
			for (int x = 0; x < segment.cols; x++)
			{
				if (segment.at<uchar>(Point(x, y)) == CHCOLOR)
				{
					neighbourhood8Vals = vector<bool>{ true, true, true, true, true, true, true, true }; // True means white pixel
					highlightThis = false;
					blackQuantity = 0;

					for (int i = 0; i < neighbourhood8Pos.size(); i++)
					{
						curr = Point(x + neighbourhood8Pos[i].x, y + neighbourhood8Pos[i].y);
						if (curr.x >= 0 && curr.x < segment.cols && curr.y >= 0 && curr.y < segment.rows)
						{
							if (segment.at<uchar>(curr) != BGCOLOR)
							{
								neighbourhood8Vals[i] = false;
								++blackQuantity;
							}
						}
					}

					if (neighbourhood8Vals[0] || neighbourhood8Vals[2] || neighbourhood8Vals[4])
					{
						if (neighbourhood8Vals[2] || neighbourhood8Vals[4] || neighbourhood8Vals[6])
						{
							if (blackQuantity >= 2 && blackQuantity <= 6)
							{
								int q = 0;
								for (int i = 0; i < neighbourhood8Vals.size() - 1; i++)
								{
									if (!neighbourhood8Vals[i] && neighbourhood8Vals[i + 1])
									{
										++q;
									}
								}
								if (!neighbourhood8Vals[neighbourhood8Vals.size() - 1] && neighbourhood8Vals[0])
								{
									++q;
								}
								if (q == 1)
								{
									segment.at<uchar>(Point(x, y)) = MARKER;
								}
							}
						}
					}
				}
			}
		}

		for (int y = 0; y < segment.rows; y++)
		{
			for (int x = 0; x < segment.cols; x++)
			{
				if (segment.at<uchar>(Point(x, y)) == MARKER)
				{
					segment.at<uchar>(Point(x, y)) = BGCOLOR;
					iterate = true;
				}
			}
		}
	}

	/* Push black boundary pixels from boundaries by one unit */
	for (int x = 0; x < segment.cols; x++) // Top boundary
	{
		if (segment.at<uchar>(Point(x, 0)) == CHCOLOR)
		{
			segment.at<uchar>(Point(x, 0)) = BGCOLOR;
			segment.at<uchar>(Point(x, 1)) = CHCOLOR;
		}
	}
	for (int y = 0; y < segment.rows; y++) // Right boundary
	{
		if (segment.at<uchar>(Point(segment.cols - 1, y)) == CHCOLOR)
		{
			segment.at<uchar>(Point(segment.cols - 1, y)) = BGCOLOR;
			segment.at<uchar>(Point(segment.cols - 2, y)) = CHCOLOR;
		}
	}
	for (int x = 0; x < segment.cols; x++) // Bottom boundary
	{
		if (segment.at<uchar>(Point(x, segment.rows - 1)) == CHCOLOR)
		{
			segment.at<uchar>(Point(x, segment.rows - 1)) = BGCOLOR;
			segment.at<uchar>(Point(x, segment.rows - 2)) = CHCOLOR;
		}
	}
	for (int y = 0; y < segment.rows; y++) // Left boundary
	{
		if (segment.at<uchar>(Point(0, y)) == CHCOLOR)
		{
			segment.at<uchar>(Point(0, y)) = BGCOLOR;
			segment.at<uchar>(Point(1, y)) = CHCOLOR;
		}
	}
}

int CharRecognitionTool::getLoopQuantity(Mat segment)
{
	const uchar CHCOLOR = 0;

	const vector<Point> neighbourhood4Pos = vector<Point>{
		Point(0, -1),  /*  Top     */
		Point(1, 0),   /*  Right   */
		Point(0, 1),   /*  Bottom  */
		Point(-1, 0)   /*  Left    */
	};
	Point curr;
	Point currPeek;

	int loopQuantity = 0;
	stack<Point> points;

	for (int y = 0; y < segment.rows; y++)
	{
		for (int x = 0; x < segment.cols; x++)
		{
			curr = Point(x, y);
			if (segment.at<uchar>(curr) != CHCOLOR)
			{
				points.push(curr);
				while (!points.empty())
				{
					curr = points.top();
					points.pop();
					segment.at<uchar>(curr) = CHCOLOR;

					for (int i = 0; i < neighbourhood4Pos.size(); i++)
					{
						currPeek = Point(curr.x + neighbourhood4Pos[i].x, curr.y + neighbourhood4Pos[i].y);
						if (currPeek.x >= 0 && currPeek.x < segment.cols && currPeek.y >= 0 && currPeek.y < segment.rows)
						{
							if (segment.at<uchar>(currPeek) != CHCOLOR)
							{
								points.push(currPeek);
							}
						}
					}
				}
				++loopQuantity;
			}
		}
	}

	return loopQuantity - 1;
}

vector<Point2f> CharRecognitionTool::getLEndsVec(Mat segment)
{
	const uchar CHCOLOR = 0;

	const vector<Point> neighbourhood8Pos = vector<Point>{
		Point(0, -1),  /*  Top            */
		Point(1, -1),  /*  Top Right      */
		Point(1, 0),   /*  Right          */
		Point(1, 1),   /*  Bottom Right   */
		Point(0, 1),   /*  Bottom         */
		Point(-1, 1),  /*  Bottom Left    */
		Point(-1, 0),  /*  Left           */
		Point(-1, -1)  /*  Top Left       */
	};
	Point curr;
	Point currPeek;

	vector<Point2f> result;

	int blackQuantity = 0;

	for (int y = 0; y < segment.rows; y++)
	{
		for (int x = 0; x < segment.cols; x++)
		{
			curr = Point(x, y);
			if (segment.at<uchar>(curr) == CHCOLOR)
			{
				blackQuantity = 0;
				for (int i = 0; i < neighbourhood8Pos.size(); i++)
				{
					currPeek = Point(curr.x + neighbourhood8Pos[i].x, curr.y + neighbourhood8Pos[i].y);
					if (currPeek.x >= 0 && currPeek.x < segment.cols && currPeek.y >= 0 && currPeek.y < segment.rows)
					{
						if (segment.at<uchar>(currPeek) == CHCOLOR)
						{
							++blackQuantity;
						}
					}
				}

				if (blackQuantity == 1)
				{
					result.push_back(Point2f((float)x / (float)segment.cols, (float)y / (float)segment.rows));
				}
			}
		}
	}

	return result;
}

vector<Point2f> CharRecognitionTool::getJunctionsVec(Mat segment)
{
	const uchar CHCOLOR = 0;
	const uchar BGCOLOR = 255;
	
	const vector<Point> neighbourhood8Pos = vector<Point>{
		Point(0, -1),  /*  Top            */
		Point(1, -1),  /*  Top Right      */
		Point(1, 0),   /*  Right          */
		Point(1, 1),   /*  Bottom Right   */
		Point(0, 1),   /*  Bottom         */
		Point(-1, 1),  /*  Bottom Left    */
		Point(-1, 0),  /*  Left           */
		Point(-1, -1)  /*  Top Left       */
	};
	Point curr;
	Point currPeek;

	vector<Point2f> result;
	uchar first, previous;

	int whiteToBlack = 0;

	for (int y = 0; y < segment.rows; y++)
	{
		for (int x = 0; x < segment.cols; x++)
		{
			curr = Point(x, y);
			if (segment.at<uchar>(curr) == CHCOLOR)
			{
				whiteToBlack = 0;

				first = segment.at<uchar>(Point(curr.x + neighbourhood8Pos[0].x, curr.y + neighbourhood8Pos[0].y));
				previous = first;

				for (int i = 1; i < neighbourhood8Pos.size(); i++)
				{
					currPeek = Point(curr.x + neighbourhood8Pos[i].x, curr.y + neighbourhood8Pos[i].y);
					if (previous == BGCOLOR && segment.at<uchar>(currPeek) == CHCOLOR)
					{
						++whiteToBlack;
					}
					previous = segment.at<uchar>(currPeek);
				}

				if (previous == BGCOLOR && first == CHCOLOR)
				{
					++whiteToBlack;
				}

				if (whiteToBlack > 2)
				{
					result.push_back(Point2f((float)x / (float)segment.cols, (float)y / (float)segment.rows));
				}
			}
		}
	}

	return result;
}

float CharRecognitionTool::getAllPointsDifference(vector<Point2f> first, vector<Point2f> second)
{
	if (first.size() != second.size())
	{
		throw new exception("getAllPointsDifference()   :   vectors are not the same size");
	}

	float result = 0;
	float min = 0;
	float curr;
	list<Point2f> container;

	for (int i = 0; i < second.size(); i++)
	{
		container.push_back(second[i]);
	}

	list<Point2f>::iterator it = container.begin();
	list<Point2f>::iterator minit = it;

	for (int i = 0; i < first.size(); i++)
	{
		it = container.begin();
		minit = it;
		min = abs(first[i].x - it->x) + abs(first[i].y - it->y);

		while (++it != container.end())
		{
			curr = abs(first[i].x - it->x) + abs(first[i].y - it->y);
			if (curr < min)
			{
				min = curr;
				minit = it;
			}
		}

		result += min;
		container.remove(*minit);
	}

	return result;
}

CharRecognitionTool::Segment::Segment() :
	mSegment(Mat()),
	mType(SegType::Undefined)
{
}

CharRecognitionTool::Segment::Segment(Mat segment) :
	mSegment(segment),
	mType(SegType::Undefined)
{
}

CharRecognitionTool::Segment::Segment(int type) :
	mSegment(Mat()),
	mType(type)
{
}

CharRecognitionTool::Segment::Segment(Mat segment, int type) :
	mSegment(segment),
	mType(type)
{
}

CharRecognitionTool::CharFeatures::CharFeatures()
{
	// Empty
}