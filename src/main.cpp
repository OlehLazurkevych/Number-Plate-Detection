#include "openCV.h"
#include "plateCropTool.h"

int main(int argc, char** argv)
{
	Mat image = imread("E:/Projects/Number-Plate-Recognition/data/photo-of-cars/ez-5.jpg");

	if (image.empty())
	{
		cout << "<Error>  :  Could not open or find the image!" << endl;
		system("pause");
		return -1;
	}

	Mat* res;

	double t = (double)getTickCount();
	
	PlateCropTool cropTool;

	image = *imgGetGray(image);
	res = cropTool.getPlate(image);

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Done in: " << t << " sec." << endl;
	
	Window::Draw(*res);
	Window::Draw(image);

	Window::Draw(*cropTool.mPotentialPlates[0].mPlate);
	Window::Draw(*cropTool.mPotentialPlates[1].mPlate);
	Window::Draw(*cropTool.mPotentialPlates[2].mPlate);

	waitKey(0);
	system("pause");
	return 0;
}