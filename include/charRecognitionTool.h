#pragma once
#include "openCV.h"
#include "imgProcessing.h"
#include "imgAnalysis.h"
#include "imgRendering.h"

class CharRecognitionTool
{
private:
	class Segment
	{
	public:
		enum SegType { Undefined, Dirt, Number, Letter };

	public:
		Mat mSegment;
		int mType;

	public:
		Segment();
		Segment(Mat segment);
		Segment(int type);
		Segment(Mat segment, int type);
	};
	
	class CharFeatures
	{
	public:
		char mChar;
		int mLoopQuantity;
		vector<Point2f> mLineEnds;
		vector<Point2f> mJunctions;

	public:
		CharFeatures();

		friend istream& operator>>(istream& stream, CharFeatures& data)
		{
			int sizeLA;
			int sizeJA;
			stream >> data.mChar >> data.mLoopQuantity >> sizeLA;
			for (int i = 0; i < sizeLA; i++)
			{
				Point2f lineEnd;
				stream >> lineEnd.x >> lineEnd.y;
				data.mLineEnds.push_back(lineEnd);
			}
			stream >> sizeJA;
			for (int i = 0; i < sizeJA; i++)
			{
				Point2f junction;
				stream >> junction.x >> junction.y;
				data.mJunctions.push_back(junction);
			}
			return stream;
		}
		friend ostream& operator<<(ostream& stream, const CharFeatures& data)
		{
			stream << data.mChar << endl << data.mLoopQuantity << endl << data.mLineEnds.size();
			for (int i = 0; i < data.mLineEnds.size(); i++)
			{
				stream << data.mLineEnds[i].x << ' ' << data.mLineEnds[i].y << endl;
			}
			stream << data.mJunctions.size() << endl;
			for (int i = 0; i < data.mJunctions.size(); i++)
			{
				stream << data.mJunctions[i].x << ' ' << data.mJunctions[i].y << endl;
			}
			return stream;
		}
	};

private:
	vector<vector<Segment>> mSegments = vector<vector<Segment>>{ vector<Segment>(), vector<Segment>(), vector<Segment>() };
	vector<CharFeatures> mNumbersFeatures;
	vector<CharFeatures> mLettersFeatures;

	int mCurrBlock = 1;
	int mCurrChar = 0;
	int mCharQuantity = 0;

	const int MIN_SEG_QUANTITY = 4;// 5 danger value
	const int DES_SEG_QUANTITY = 8;

public:
	CharRecognitionTool(const string numbersFeaturesFilePath, const string lettersFeaturesFilePath);

public:
	void setSegments(vector<Mat> segments);
	string next();
	void drawAll();

	int getCharQuantity() const;

private:
	string recognizeChar(Mat& segment, bool isNumber);
	void skeletonize(Mat& segment);

public://////////////////////////////////////////////////////////////////////
	int getLoopQuantity(Mat segment);
	vector<Point2f> getLEndsVec(Mat segment);
	vector<Point2f> getJunctionsVec(Mat segment);
	float getAllPointsDifference(vector<Point2f> first, vector<Point2f> secound);
};