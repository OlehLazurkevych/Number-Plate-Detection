#pragma once
#include "openCV.h"
#include "imgProcessing.h"
#include "imgAnalysis.h"
#include "imgRendering.h"

class SegmentationTool
{
private:
	vector<Mat> mSegments;

	const double CUT_PEEK_CONSTANT = 0.89;
	const double TRASH_DIF_CONSTANT = 5.0;
	const int MIN_SEGMENTS_CONSTANT = 5;

public:
	SegmentationTool();
	~SegmentationTool();

public:
	vector<Mat> getSegments(Mat& image);

private:
	void cutSegments(Mat& image);
	vector<pair<int, int>> firstPeekAndFlat(vector<int>& projection, const int threshHold);
	void extractCharacters();
	pair<Mat, int>* extractParticle(Mat& segment, const int seedX, const int seedY);
	void removeTrash();
};