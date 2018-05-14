#include "charRecognitionTool.h"

CharRecognitionTool::CharRecognitionTool()
{
	// Empty
}

void CharRecognitionTool::init(vector<Mat> segments)
{
	uint clearSegQuantity = 0;

	mChars[0].clear();
	mChars[1].clear();
	mChars[2].clear();

	mCurrBlock = 1;
	mCurrChar = 0;
	mCharsQuantity = 0;

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
		mChars[0].push_back(Segment(segments[0], Segment::Letter));
		mChars[0].push_back(Segment(segments[1], Segment::Letter));

		mChars[1].push_back(Segment(segments[2], Segment::Number));
		mChars[1].push_back(Segment(segments[3], Segment::Number));
		mChars[1].push_back(Segment(segments[4], Segment::Number));
		mChars[1].push_back(Segment(segments[5], Segment::Number));

		mChars[2].push_back(Segment(segments[6], Segment::Letter));
		mChars[2].push_back(Segment(segments[7], Segment::Letter));
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
				if ((segmentVec[i].mType == Segment::Letter || segmentVec[i].mType == Segment::Dirt) && mChars[block].size() < 2)
				{
					mChars[block].push_back(segmentVec[i]);
				}
				else
				{
					++block;
				}
			}
			if (block == 1)
			{
				if (mChars[block].size() < 4)
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
							mChars[block].push_back(segmentVec[i]);
						}
					}
					else
					{
						mChars[block].push_back(segmentVec[i]);
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
				mChars[block].push_back(segmentVec[i]);
			}
		}
	}
	
	for (int i = 0; i < mChars.size(); i++)
	{
		mCharsQuantity += mChars[i].size();
	}
}

char CharRecognitionTool::next()
{
	for (int i = 0; i < mChars.size(); i++)
	{
		for (int j = 0; j < mChars[i].size(); j++)
		{
			skeletonize(mChars[i][j].mSegment);
		}
	}

	return 0;
}

void CharRecognitionTool::drawAll() 
{
//	char n = '1';
//	string f = "E:/Projects/Number-Plate-Recognition/data/training-chars/t";
//	string s = ".png";/////////////////////////////////////////////////////

	Mat blunk(20, 20, CV_8UC1, Scalar(255));
	for (int i = 0; i < mChars.size(); i++)
	{
		for (int j = 0; j < mChars[i].size(); j++)
		{
			Window::Draw((mChars[i][j].mSegment.empty()) ? blunk : mChars[i][j].mSegment);

			//imwrite( f + n++ + s , mChars[i][j].mSegment);//////////////////////////////////////////

			switch (mChars[i][j].mType)
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

int CharRecognitionTool::getCharsQuantity() const
{
	return mCharsQuantity;
}

char CharRecognitionTool::recognizeChar(Mat & segment, bool isNumber)
{
	return 0;
}

void CharRecognitionTool::skeletonize(Mat & segment)
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