#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

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
	
	Mat filter = (Mat_<double>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
	Mat edgeY;
	Point anchor = Point(-1, -1); // Means center of filter matrics
	int delta = 0; // A value to be added to each pixel during the convolution
	int ddepth = -1; // The depth of dst. A negative value (such as -1) indicates that the depth is the same as the source.
	
	filter2D(image, edgeY, ddepth, filter, anchor, delta, BORDER_DEFAULT);
	
	namedWindow(windowName, CV_WINDOW_FREERATIO); // Create a window
	imshow(windowName, edgeY); // Show our image inside the created window.
	resizeWindow(windowName, image.cols * 0.6, image.rows * 0.6);

	waitKey(0);
	destroyWindow(windowName);
	return 0;
}