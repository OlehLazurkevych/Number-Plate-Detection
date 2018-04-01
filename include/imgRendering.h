#pragma once
#include "openCV.h"

class Window
{
private:
	static string mCounter;
	Window();

public:
	static void Draw(Mat& img, int width = 0, int height = 0);
	static void Draw(vector<int>& vec, bool horizontal);
};