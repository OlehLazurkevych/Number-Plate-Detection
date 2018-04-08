#include "imgRendering.h"

string Window::mCounter = "A";

void Window::Draw(Mat& img, int width, int height)
{
	if (img.cols != 0 && img.rows != 0)
	{
		string windowName = "Step #" + mCounter;
		++mCounter[0];

		namedWindow(windowName, CV_WINDOW_FREERATIO); // Create a window
		imshow(windowName, img);
		resizeWindow(windowName, (width > 0) ? width : img.cols * 0.6, (height > 0) ? height : img.rows * 0.6);
	}
}

void Window::Draw(vector<int>& vec, bool horizontal)
// Draws a histogram of a vector
{
	int width = *max_element(vec.begin(), vec.end());
	int lowest = *min_element(vec.begin(), vec.end());
	if (lowest < 0)
	{
		lowest = abs(lowest);
		width += lowest;
	}
	else
	{
		lowest = 0;
	}
	int height = vec.size();
	
	if (width != 0 && height != 0)
	{
		Mat histogram(height, width, CV_8UC1, Scalar(230));

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				histogram.at<uchar>(Point(j, i)) = 25;
				if (j >= vec[i] + lowest)
				{
					break;
				}
				if (j == lowest)
				{
					histogram.at<uchar>(Point(j, i)) = 230;
				}
			}
		}

		if (horizontal)
		{
			rotate(histogram, histogram, ROTATE_90_CLOCKWISE);
			Mat res;
			flip(histogram, res, 1);
			Draw(res, 0, 200);
		}
		else
		{
			Draw(histogram, 200, 0);
		}
	}
}