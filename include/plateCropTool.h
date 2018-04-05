#pragma once
#include "openCV.h"
#include "imgProcessing.h"
#include "imgAnalysis.h"
#include "imgRendering.h"

class plateCropTool
{
public:
	Mat& mSource;
	pair<int, int>* mPotentialPlates;
	vector<int>* mVerticalProj;

private:
	const int POTENTIAL_PLATES_QUANTITY = 3;
	const int RANK_SIZE = 18;

	const double FIRST_PHASE_COEF = 0.55;
	const double SECOUND_PHASE_COEF = 0.42;

public:
	plateCropTool(Mat& image);
	~plateCropTool();

public:
	void firstPhase();

private:
	pair<int, int> findVertClip(vector<int>* verticalProj);
};