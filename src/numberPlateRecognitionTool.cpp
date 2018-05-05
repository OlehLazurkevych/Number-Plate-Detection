#include "numberPlateRecognitionTool.h"

NumberPlateRecognitionTool::NumberPlateRecognitionTool()
{
	mPCT = new PlateCropTool();
	mST = new SegmentationTool();
	mCRT = new CharRecognitionTool();
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
	if (mCroppedPlate)
	{
		delete mCroppedPlate;
	}
}

void NumberPlateRecognitionTool::tryRecognize(Mat& image)
{
	vector<Mat> segments;
	
	if (mSource)
	{
		delete mSource;
	}
	if (mCroppedPlate)
	{
		delete mCroppedPlate;
	}

	mTime = (double)getTickCount();

	mSource = imgGetGray(image);
	mCroppedPlate = mPCT->getPlate(*mSource);
	segments = mST->getSegments(*mCroppedPlate);
	try
	{
		mCRT->init(segments);
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
	cout << "Done in   :   " << mTime << " sec." << endl;

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