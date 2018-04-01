#include "plateCropTool.h"

plateCropTool::plateCropTool(Mat& image) : mSource(image)
{
	mPotentialPlates = new Mat[3];
}

plateCropTool::~plateCropTool()
{
	delete[] mPotentialPlates;
}

void plateCropTool::firstPhase()
{
	Mat* filtered = imgFilter(mSource, verticalDetectionMat);
	vector<int>* vertProj = imgProjection(*filtered, false);
	vector<int>* smoothVertProj = vecRankFilter(*vertProj, RANK_SIZE);

	delete filtered;
	delete vertProj;
	delete smoothVertProj;
}
