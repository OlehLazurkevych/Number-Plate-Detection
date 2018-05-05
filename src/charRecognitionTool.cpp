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
	else if (segments.size() > DES_SEG_QUANTITY)
	{
		throw exception("Plate not found   :   too many segments for a number plate");
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
	return 0;
}

void CharRecognitionTool::drawAll() 
{
	Mat blunk(20, 20, CV_8UC1, Scalar(255));
	for (int i = 0; i < mChars.size(); i++)
	{
		for (int j = 0; j < mChars[i].size(); j++)
		{
			Window::Draw((mChars[i][j].mSegment.empty()) ? blunk : mChars[i][j].mSegment);
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