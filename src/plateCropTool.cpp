#include "plateCropTool.h"

plateCropTool::plateCropTool(Mat& image) : mSource(image)
{
	mPotentialPlates = new Mat*[POTENTIAL_PLATES_QUANTITY];
}

plateCropTool::~plateCropTool()
{
	if (mPotentialPlates)
	{
		for (int i = 0; i < POTENTIAL_PLATES_QUANTITY; i++)
		{
			if (mPotentialPlates[i])
			{
				delete mPotentialPlates[i];
			}
		}
		delete[] mPotentialPlates;
	}
}

void plateCropTool::firstPhase()
{
	Mat* filtered = imgFilter(mSource, verticalDetectionMat);
	vector<int>* roughtVerticalProj = imgProjection(*filtered, false);
	vector<int>* verticalProj = vecRankFilter(*roughtVerticalProj, VERTICAL_RANK_SIZE);
	pair<int, int> clipBoundaries;

	for (int i = 0; i < POTENTIAL_PLATES_QUANTITY; i++)
	{
		clipBoundaries = findPeekFoot(verticalProj, FIRST_PHASE_COEF);
		mPotentialPlates[i] = imgCrop(0, clipBoundaries.first, mSource.cols, clipBoundaries.second - clipBoundaries.first, mSource);
		fill(verticalProj->begin() + clipBoundaries.first, verticalProj->begin() + clipBoundaries.second, 0);
	}

	delete filtered;
	delete roughtVerticalProj;
	delete verticalProj;
}

pair<int, int> plateCropTool::findPeekFoot(vector<int>* projection, const double coefficient)
{
	pair<int, int> result;
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
