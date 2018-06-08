#pragma once
#include "openCV.h"
#include "imgRendering.h"
#include "plateCropTool.h"
#include "segmentationTool.h"
#include "charRecognitionTool.h"

class NumberPlateRecognitionTool
{
private: 
class CarData
{
public:
	string Number = "";
	string Owner = "";
	string Mark = "";
	string Id = "";

	CarData() {}
	CarData(string number, string owner, string mark, string id): Number(number), Owner(owner), Mark(mark), Id(id) {}

	friend ostream& operator<<(ostream& out, CarData& obj)
	{
		return out << "Number: " << obj.Number << " Owner: " << obj.Owner << " Mark: " << obj.Mark << " Id: " << obj.Id;
	}
};
private:
	PlateCropTool*         mPCT;
	SegmentationTool*      mST;
	CharRecognitionTool*   mCRT;

	vector<CarData> mDB;

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