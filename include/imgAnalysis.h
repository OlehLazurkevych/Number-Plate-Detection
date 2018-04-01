#pragma once
#include "openCV.h"

vector<int>* imgProjection(Mat& img, bool horizontal);
vector<int>* vecRankFilter(vector<int>& vec, int rankSize);