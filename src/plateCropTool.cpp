#include "plateCropTool.h"

PlateCropTool::PlateCropTool()
{
	// Empty
}

PlateCropTool::~PlateCropTool()
{
	if (mPotentialPlates)
	{
		delete[] mPotentialPlates;
	}
}

Mat* PlateCropTool::getPlate(Mat& image)
{
	if (mPotentialPlates)
	{
		delete[] mPotentialPlates;
	}
	mPotentialPlates = new PotentialPlate[VERTICAL_PEAK_QUANTITY];

	mSource = &image;

	firstPhase();
	secoundPhase();
	thirdPhase();
	int index = choosePlateIndex();

	imwrite("plate.jpg", *mPotentialPlates[index].mPlate);

	return mPotentialPlates[index].mPlate;
}

void PlateCropTool::firstPhase()
// Clips vertically all potential plates
{
	Mat* filtered = imgFilter(*mSource, verticalDetectionMat);
	vector<int>* roughtVerticalProj = imgProjection(*filtered, false);
	vector<int>* verticalProj = vecRankFilter(*roughtVerticalProj, VERTICAL_RANK_SIZE);
	pair<int, int> clipBoundaries;

	for (int i = 0; i < VERTICAL_PEAK_QUANTITY; i++)
	{
		clipBoundaries = findPeakFoot(*verticalProj, VERTICAL_CLIP_COEF);

		if (clipBoundaries.first < clipBoundaries.second)
		{
			mPotentialPlates[i].mPlate = imgCrop(0, clipBoundaries.first, mSource->cols, clipBoundaries.second - clipBoundaries.first, *mSource);
			mPotentialPlates[i].setHeightHeuristics(clipBoundaries.second - clipBoundaries.first);

			vector<int>* subProjection = new vector<int>(verticalProj->begin() + clipBoundaries.first, verticalProj->begin() + clipBoundaries.second);
			mPotentialPlates[i].setPeakHeuristics(*subProjection);
			mPotentialPlates[i].setAreaHeuristics(*subProjection);
			if (subProjection)
			{
				delete subProjection;
			}
			
			fill(verticalProj->begin() + clipBoundaries.first, verticalProj->begin() + clipBoundaries.second, 0);
		}
		else
		{
			mPotentialPlates[i].mPlate = nullptr;
		}
	}

	if (filtered)
	{
		delete filtered;
	}
	if (roughtVerticalProj)
	{
		delete roughtVerticalProj;
	}
	if (verticalProj)
	{
		delete verticalProj;
	}
}

void PlateCropTool::secoundPhase()
// Clips horizontally on all vertically clipt plates
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
		if (mPotentialPlates[i].mPlate != nullptr)
		{
			filtered = imgFilter(*mPotentialPlates[i].mPlate, horizontalDetectionMat);

			roughtHorizontalProj = imgProjection(*filtered, true);
			horizontalProj = vecRankFilter(*roughtHorizontalProj, mPotentialPlates[i].mPlate->rows * HORIZONTAL_RANK_COEF);

			clipBndrIndx = 0;
			curBndr = 0;
			maxBndr = 0;

			for (int j = 0; j < HORIZONTAL_PEAK_QUANTITY; j++)
			{
				clipBoundaries[j] = findPeakFoot(*horizontalProj, HORIZONTAL_CLIP_COEF);
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
				if (mPotentialPlates[i].mPlate)
				{
					delete mPotentialPlates[i].mPlate;
				}
				mPotentialPlates[i].mPlate = inswapping;
			}
			if (filtered)
			{
				delete filtered;
			}
			if (roughtHorizontalProj)
			{
				delete roughtHorizontalProj;
			}
			if (horizontalProj)
			{
				delete horizontalProj;
			}
		}
	}
	if (clipBoundaries)
	{
		delete[] clipBoundaries;
	}
}

void PlateCropTool::thirdPhase()
// Clips horizontally all potential plates more precisely
{
	vector<int>* horizontalProj = nullptr;
	vector<int>* deriviateProj = nullptr;
	vector<int>* firstHalf = nullptr;
	vector<int>* secoundHalf = nullptr;
	Mat* inswapping;
	pair<int, int> clipBoundaries;

	for (int i = 0; i < VERTICAL_PEAK_QUANTITY; i++)
	{
		if (mPotentialPlates[i].mPlate != nullptr)
		{
			horizontalProj = imgProjection(*mPotentialPlates[i].mPlate, true);
			deriviateProj = findDerivative(*horizontalProj);

			firstHalf = new vector<int>(deriviateProj->begin(), deriviateProj->begin() + (deriviateProj->size() / 2));
			secoundHalf = new vector<int>(deriviateProj->begin() + (deriviateProj->size() / 2), deriviateProj->end());

			clipBoundaries.first = findFirstPeak(*firstHalf, FIRST_PEAK_COEF, true, true);
			clipBoundaries.second = (deriviateProj->size() / 2) + findFirstPeak(*secoundHalf, FIRST_PEAK_COEF, false, false);

			mPotentialPlates[i].setRatioHeuristics(clipBoundaries.second - clipBoundaries.first, mPotentialPlates[i].mPlate->rows);

			inswapping = imgCrop(clipBoundaries.first, 0, clipBoundaries.second - clipBoundaries.first, mPotentialPlates[i].mPlate->rows, *mPotentialPlates[i].mPlate);
			if (mPotentialPlates[i].mPlate)
			{
				delete mPotentialPlates[i].mPlate;
			}
			mPotentialPlates[i].mPlate = inswapping;

			if (firstHalf)
			{
				delete firstHalf;
			}
			if (secoundHalf)
			{
				delete secoundHalf;
			}
			if (horizontalProj)
			{
				delete horizontalProj;
			}
			if (deriviateProj)
			{
				delete deriviateProj;
			}
		}
	}
}

int PlateCropTool::choosePlateIndex()
{
	double min = mPotentialPlates[0].getOveralCost();
	int index = 0;

	for (int i = 1; i < VERTICAL_PEAK_QUANTITY; i++)
	{
		if (mPotentialPlates[i].mPlate != nullptr)
		{
			if (mPotentialPlates[i].getOveralCost() < min)
			{
				min = mPotentialPlates[i].getOveralCost();
				index = i;
			}
		}
	}

	return index;
}

pair<int, int> PlateCropTool::findPeakFoot(const vector<int>& projection, const double coefficient)
{
	pair<int, int> result(0,0);
	auto highest = max_element(projection.begin(), projection.end());

	for (int i = distance(projection.begin(), highest); i >= 0; i--)
	{
		if (projection[i] < (*highest) * coefficient)
		{
			break;
		}
		result.first = i;
	}

	for (int i = distance(projection.begin(), highest); i < projection.size(); i++)
	{
		if (projection[i] < (*highest) * coefficient)
		{
			break;
		}
		result.second = i;
	}

	return result;
}

vector<int>* PlateCropTool::findDerivative(const vector<int>& projection)
{
	vector<int>* result = new vector<int>(projection.size());

	fill(result->begin(), result->begin() + DERIVE_FUNC_COEF, 0);

	for (int i = DERIVE_FUNC_COEF; i < result->size(); i++)
	{
		result->at(i) = (projection[i] - projection[i - DERIVE_FUNC_COEF]) / DERIVE_FUNC_COEF;
	}

	return result;
}

int PlateCropTool::findFirstPeak(const vector<int>& projection, const double coefficient, const bool fromStart, const bool positive)
{
	int result;

	if (positive)
	{
		int max = 0;
		double checker = (*max_element(projection.begin(), projection.end())) * coefficient;
		if (fromStart)
		{
			for (int i = 0; i < projection.size(); i++)
			{
				if (projection[i] > checker)
				{
					if (projection[i] >= max)
					{
						result = i;
						max = projection[i];
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			for (int i = projection.size() - 1; i >= 0; i--)
			{
				if (projection[i] > checker)
				{
					if (projection[i] >= max)
					{
						result = i;
						max = projection[i];
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	else
	{
		int min = 0;
		double checker = (*min_element(projection.begin(), projection.end())) * coefficient;

		if (fromStart)
		{
			for (int i = 0; i < projection.size(); i++)
			{
				if (projection[i] < checker)
				{
					if (projection[i] <= min)
					{
						result = i;
						min = projection[i];
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			for (int i = projection.size() - 1; i >= 0; i--)
			{
				if (projection[i] < checker)
				{
					if (projection[i] <= min)
					{
						result = i;
						min = projection[i];
					}
					else
					{
						break;
					}
				}
			}
		}
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

void PlateCropTool::PotentialPlate::setHeightHeuristics(const double value)
{
	mHeightHeuristics = value;
}

void PlateCropTool::PotentialPlate::setPeakHeuristics(const vector<int>& data)
{
	mPeakHeuristics = 1 / (*std::max_element(data.begin(), data.end()));
}

void PlateCropTool::PotentialPlate::setAreaHeuristics(const vector<int>& data)
{
	mAreaHeuristics = 1 / std::accumulate(data.begin(), data.end(), 0);
}

void PlateCropTool::PotentialPlate::setRatioHeuristics(const double width, const double height)
{
	mRatioHeuristics = abs(abs(width / height) - PLATE_RATIO);
}
