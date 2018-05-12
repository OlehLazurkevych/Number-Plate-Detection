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

private:
	vector<vector<Segment>> mChars = vector<vector<Segment>>{ vector<Segment>(), vector<Segment>(), vector<Segment>() };

	int mCurrBlock = 1;
	int mCurrChar = 0;
	int mCharsQuantity = 0;

	const int MIN_SEG_QUANTITY = 5;
	const int DES_SEG_QUANTITY = 8;

public:
	CharRecognitionTool();

public:
	void init(vector<Mat> segments);
	char next();
	void drawAll();

	int getCharsQuantity() const;

private:
	char recognizeChar(Mat& segment, bool isNumber);
	void skeletonize(Mat& segment);
};