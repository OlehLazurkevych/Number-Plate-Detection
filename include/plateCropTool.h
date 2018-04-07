#pragma once
#include "openCV.h"
#include "imgProcessing.h"
#include "imgAnalysis.h"
#include "imgRendering.h"

class PlateCropTool
{
public:
	class PotentialPlate
	{
	public:
		Mat* mPlate				 = nullptr;

	private:
		double mHeightHeuristics = 0; // Height of plate
		double mPeakHeuristics	 = 0; // Height of peak on vertical projection
		double mAreaHeuristics	 = 0; // Area of peak
		double mRatioHeuristics	 = 0; // Ratio of plate

		const double PLATE_RATIO = 6.8;

	public:
		PotentialPlate();
		~PotentialPlate();

	public:
		double getOveralCost();

		void setHeightHeuristics(double value);
		void setPeakHeuristics(vector<int>& data);
		void setAreaHeuristics(vector<int>& data);
		void setRatioHeuristics(double width, double height);
	};

public:
	Mat& mSource;
	PotentialPlate* mPotentialPlates;

private:
	const int VERTICAL_PEAK_QUANTITY   = 3;
	const int HORIZONTAL_PEAK_QUANTITY = 2;
	const int VERTICAL_RANK_SIZE       = 9;
	const int DERIVE_FUNC_COEF         = 4;

	const double VERTICAL_CLIP_COEF   = 0.55;
	const double HORIZONTAL_CLIP_COEF = 0.33;
	const double THIRD_PHASE_COEF     = 0.42;
	const double HORIZONTAL_RANK_COEF = 3.2;

public:
	PlateCropTool(Mat& image);
	~PlateCropTool();

public:
	void firstPhase();
	void secoundPhase();
	void thirdPhase();

private:
	pair<int, int> findPeakFoot(vector<int>* projection, const double coefficient);
};