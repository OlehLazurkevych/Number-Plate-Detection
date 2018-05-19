#include "numberPlateRecognitionTool.h"

NumberPlateRecognitionTool::NumberPlateRecognitionTool(const string numbersFeaturesFilePath, const string lettersFeaturesFilePath)
{
	mPCT = new PlateCropTool();
	mST = new SegmentationTool();
	mCRT = new CharRecognitionTool(numbersFeaturesFilePath, lettersFeaturesFilePath);
}

NumberPlateRecognitionTool::~NumberPlateRecognitionTool()
{
	delete mPCT;
	delete mST;
	delete mCRT;

	if (mSource)
	{
		delete mSource;
	}
}

void NumberPlateRecognitionTool::tryRecognize(Mat& image)
{
	vector<Mat> segments;
	
	if (mSource)
	{
		delete mSource;
		mSource = nullptr;
	}
	if (mCroppedPlate)
	{
		delete mCroppedPlate;
		mCroppedPlate = nullptr;
	}

	mTime = (double)getTickCount();

	mSource = imgGetGray(image);
	mCroppedPlate = mPCT->getPlate(*mSource);
	segments = mST->getSegments(*mCroppedPlate);

	try
	{
		mCRT->setSegments(segments);
		mCRT->next();
	}
	catch (exception e)
	{
		mTime = ((double)getTickCount() - mTime) / getTickFrequency();
		throw e;
	}

	mTime = ((double)getTickCount() - mTime) / getTickFrequency();
}

void NumberPlateRecognitionTool::showCashData(bool additionalData)
{
	cout << endl << "Done in   :   " << mTime << " sec." << endl;

	Window::Draw(*mSource);
	Window::Draw(*mCroppedPlate);
	mCRT->drawAll();

	if (additionalData)
	{
		Window::Draw(*mPCT->mPotentialPlates[0].mPlate);
		Window::Draw(*mPCT->mPotentialPlates[1].mPlate);
		Window::Draw(*mPCT->mPotentialPlates[2].mPlate);
	}
}