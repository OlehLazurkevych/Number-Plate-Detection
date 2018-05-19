#pragma once
#include "openCV.h"
#include "imgRendering.h"
#include "plateCropTool.h"
#include "segmentationTool.h"
#include "charRecognitionTool.h"

class NumberPlateRecognitionTool
{
private:
	PlateCropTool*         mPCT;
	SegmentationTool*      mST;
	CharRecognitionTool*   mCRT;

	/* Cash data */
	Mat* mSource;
	Mat* mCroppedPlate;
	double mTime;

public:
	NumberPlateRecognitionTool(const string numbersFeaturesFilePath, const string lettersFeaturesFilePath);
	~NumberPlateRecognitionTool();

public:
	void tryRecognize(Mat& image);
	void showCashData(bool additionalData = false);
};