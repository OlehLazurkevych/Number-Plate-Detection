#pragma once
#include "openCV.h"
#include "imgProcessing.h"
#include "imgAnalysis.h"
#include "imgRendering.h"

class plateCropTool
{
private:
	const int RANK_SIZE = 18;

public:
	Mat* mPotentialPlates;
	Mat& mSource;

public:
	plateCropTool(Mat& image);
	~plateCropTool();

public:
	void firstPhase();
};