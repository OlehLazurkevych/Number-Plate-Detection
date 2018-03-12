#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

#include "filters.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat image = imread("E:/Projects/Number-Plate-Recognition/data/photo-of-cars/ez-1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		system("pause");
		return -1;
	}
	
	String windowName = "Source image";
	
	Mat* res = nullptr;
	res = filter(image, verticalDetectionMat);
	
	namedWindow(windowName, CV_WINDOW_FREERATIO); // Create a window
	imshow(windowName, *res); // Show our image inside the created window.
	resizeWindow(windowName, image.cols * 0.6, image.rows * 0.6);

	waitKey(0);
	delete res;
	destroyWindow(windowName);
	return 0;
}