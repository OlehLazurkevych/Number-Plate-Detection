#include "plateCropTool.h"

PlateCropTool::PlateCropTool(Mat& image) : mSource(image)
{
	mPotentialPlates = new PotentialPlate[VERTICAL_PEAK_QUANTITY];
}

PlateCropTool::~PlateCropTool()
{
	if (mPotentialPlates)
	{
		delete[] mPotentialPlates;
	}
}

void PlateCropTool::firstPhase()
{
	Mat* filtered = imgFilter(mSource, verticalDetectionMat);
	vector<int>* roughtVerticalProj = imgProjection(*filtered, false);
	vector<int>* verticalProj = vecRankFilter(*roughtVerticalProj, VERTICAL_RANK_SIZE);
	pair<int, int> clipBoundaries;

	for (int i = 0; i < VERTICAL_PEAK_QUANTITY; i++)
	{
		clipBoundaries = findPeakFoot(verticalProj, VERTICAL_CLIP_COEF);

		mPotentialPlates[i].mPlate = imgCrop(0, clipBoundaries.first, mSource.cols, clipBoundaries.second - clipBoundaries.first, mSource);
		mPotentialPlates[i].setHeightHeuristics(clipBoundaries.second - clipBoundaries.first);

		vector<int>* subProjection = new vector<int>(verticalProj->begin() + clipBoundaries.first, verticalProj->begin() + clipBoundaries.second);
		mPotentialPlates[i].setPeakHeuristics(*subProjection);
		mPotentialPlates[i].setAreaHeuristics(*subProjection);
		delete subProjection;

		fill(verticalProj->begin() + clipBoundaries.first, verticalProj->begin() + clipBoundaries.second, 0);
	}

	delete filtered;
	delete roughtVerticalProj;
	delete verticalProj;
}

void PlateCropTool::secoundPhase()
{
	Mat* filtered = nullptr;
	Mat* inswapping = nullptr;
	vector<int>* roughtHorizontalProj = nullptr;
	vector<int>* horizontalProj = nullptr;
	pair<int, int>* clipBoundaries = new pair<int, int>[HORIZONTAL_PEAK_QUANTITY];

	int clipBndrIndx = 0;
	int curBndr = 0;
	int maxBndr = 0;

	for (int i = 0; i < VERTICAL_PEAK_QUANTITY; i++)
	{
		filtered = imgFilter(*mPotentialPlates[i].mPlate, horizontalDetectionMat);
		roughtHorizontalProj = imgProjection(*filtered, true);
		horizontalProj = vecRankFilter(*roughtHorizontalProj, mPotentialPlates[i].mPlate->rows * HORIZONTAL_RANK_COEF);

		clipBndrIndx = 0;
		curBndr = 0;
		maxBndr = 0;
		
		for (int j = 0; j < HORIZONTAL_PEAK_QUANTITY; j++)
		{
			clipBoundaries[j] = findPeakFoot(horizontalProj, HORIZONTAL_CLIP_COEF);
			fill(horizontalProj->begin() + clipBoundaries[j].first, horizontalProj->begin() + clipBoundaries[j].second, 0);

			curBndr = clipBoundaries[j].second - clipBoundaries[j].first;
			if (curBndr > maxBndr)
			{
				maxBndr = curBndr;
				clipBndrIndx = j;
			}
		}

		inswapping = imgCrop(clipBoundaries[clipBndrIndx].first, 0, clipBoundaries[clipBndrIndx].second - clipBoundaries[clipBndrIndx].first, mPotentialPlates[i].mPlate->rows, *mPotentialPlates[i].mPlate);
		if (inswapping)
		{
			delete mPotentialPlates[i].mPlate;
			mPotentialPlates[i].mPlate = inswapping;
		}

		delete filtered;
		delete roughtHorizontalProj;
		delete horizontalProj;
	}
	delete[] clipBoundaries;
}

void PlateCropTool::thirdPhase()
{

}

pair<int, int> PlateCropTool::findPeakFoot(vector<int>* projection, const double coefficient)
{
	pair<int, int> result(0,0);
	auto highest = max_element(projection->begin(), projection->end());

	for (int i = distance(projection->begin(), highest); i >= 0; i--)
	{
		if (projection->at(i) < (*highest) * coefficient)
		{
			break;
		}
		result.first = i;
	}

	for (int i = distance(projection->begin(), highest); i < projection->size(); i++)
	{
		if (projection->at(i) < (*highest) * coefficient)
		{
			break;
		}
		result.second = i;
	}

	return result;
}

PlateCropTool::PotentialPlate::PotentialPlate()
{
	// Empty
}

PlateCropTool::PotentialPlate::~PotentialPlate()
{
	if (mPlate)
	{
		delete mPlate;
	}
}

double PlateCropTool::PotentialPlate::getOveralCost()
{
	return (0.15 * mHeightHeuristics + 0.25 * mPeakHeuristics + 0.4 * mAreaHeuristics + 0.4 * mRatioHeuristics);
}

void PlateCropTool::PotentialPlate::setHeightHeuristics(double value)
{
	mHeightHeuristics = value;
}

void PlateCropTool::PotentialPlate::setPeakHeuristics(vector<int>& data)
{
	mPeakHeuristics = 1 / (*std::max_element(data.begin(), data.end()));
}

void PlateCropTool::PotentialPlate::setAreaHeuristics(vector<int>& data)
{
	mAreaHeuristics = 1 / std::accumulate(data.begin(), data.end(), 0);
}

void PlateCropTool::PotentialPlate::setRatioHeuristics(double width, double height)
{
	mRatioHeuristics = abs(abs(width / height) - PLATE_RATIO);
}
