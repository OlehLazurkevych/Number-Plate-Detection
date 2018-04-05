#include "plateCropTool.h"

plateCropTool::plateCropTool(Mat& image) : mSource(image)
{
	mPotentialPlates = new pair<int, int>[POTENTIAL_PLATES_QUANTITY];
}

plateCropTool::~plateCropTool()
{
	if (mPotentialPlates)
	{
		delete[] mPotentialPlates;
	}
	if (mVerticalProj)
	{
		delete mVerticalProj;
	}
}

void plateCropTool::firstPhase()
{
	Mat* filtered = imgFilter(mSource, verticalDetectionMat);
	vector<int>* rVertProj = imgProjection(*filtered, false);

	mVerticalProj = vecRankFilter(*rVertProj, RANK_SIZE);

	for (int i = 0; i < POTENTIAL_PLATES_QUANTITY; i++)
	{
		mPotentialPlates[i] = findVertClip(mVerticalProj);
		fill(mVerticalProj->begin() + mPotentialPlates[i].first, mVerticalProj->begin() + mPotentialPlates[i].second, 0);
	}

	delete filtered;
	delete rVertProj;
}

pair<int, int> plateCropTool::findVertClip(vector<int>* verticalProj)
{
	pair<int, int> result;
	auto highest = max_element(verticalProj->begin(), verticalProj->end());

	for (int i = distance(verticalProj->begin(), highest); i >= 0; i--)
	{
		if (verticalProj->at(i) < (*highest) * FIRST_PHASE_COEF)
		{
			break;
		}
		result.first = i;
	}

	for (int i = distance(verticalProj->begin(), highest); i < verticalProj->size(); i++)
	{
		if (verticalProj->at(i) < (*highest) * FIRST_PHASE_COEF)
		{
			break;
		}
		result.second = i;
	}

	return result;
}
