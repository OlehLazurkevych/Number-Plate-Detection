#pragma once
#include "openCV.h"
#include "imgProcessing.h"
#include "imgAnalysis.h"
#include "imgRendering.h"

class SegmentationTool
{
private:
	vector<Mat> mSegments;

	const double TRASH_DIF_CONSTANT = 5.5;
	const double SEGMENT_HIGHT_CONSTANT = 2.4;
	const int MIN_SEGMENTS_CONSTANT = 3;

public:
	SegmentationTool();

public:
	vector<Mat> getSegments(Mat& image);

private:
	void cutSegments(Mat& image);
	Mat extractParticle(Mat& segment, const int seedX, const int seedY, Point& globalTopLeft);
	void removeTrash();
	void deskew(Mat& image);
};