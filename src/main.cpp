#include "openCV.h"
#include "plateCropTool.h"

int main(int argc, char** argv)
{
	Mat image = imread("E:/Projects/Number-Plate-Recognition/data/photo-of-cars/ez-1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	
	if (image.empty())
	{
		cout << "<Error>  :  Could not open or find the image!" << endl;
		system("pause");
		return -1;
	}

	Mat* res;

	double t = (double)getTickCount();
	
	PlateCropTool cropTool;

	res = cropTool.getPlate(image);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Done in: " << t << " sec." << endl;
	
	Window::Draw(*res);

	waitKey(0);
	system("pause");
	return 0;
}