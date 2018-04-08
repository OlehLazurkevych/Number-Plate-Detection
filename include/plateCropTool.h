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
		double getOveralCost(); // This cost is used to compare all potential plates with each other

		void setHeightHeuristics(const double value);
		void setPeakHeuristics(const vector<int>& data);
		void setAreaHeuristics(const vector<int>& data);
		void setRatioHeuristics(const double width, const double height);
	};

public:
	Mat* mSource;
	PotentialPlate* mPotentialPlates;

private:
	const int VERTICAL_PEAK_QUANTITY   = 3;
	const int HORIZONTAL_PEAK_QUANTITY = 2;
	const int VERTICAL_RANK_SIZE       = 9;
	const int DERIVE_FUNC_COEF         = 4;

	const double VERTICAL_CLIP_COEF   = 0.55;
	const double HORIZONTAL_CLIP_COEF = 0.33;
	const double FIRST_PEAK_COEF      = 0.42;
	const double HORIZONTAL_RANK_COEF = 3.2;

public:
	PlateCropTool();
	~PlateCropTool();

public:
	Mat* getPlate(Mat& image);

private:
	void firstPhase();
	void secoundPhase();
	void thirdPhase();
	int choosePlateIndex();

private:
	pair<int, int> findPeakFoot(const vector<int>& projection, const double coefficient);
	vector<int>* findDerivative(const vector<int>& projection);
	int findFirstPeak(const vector<int>& projection, const double coefficient, const bool fromStart, const bool positive);
};