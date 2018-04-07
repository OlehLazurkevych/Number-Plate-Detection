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

	double t = (double)getTickCount();
	
	PlateCropTool cropTool(image);
	
	cropTool.firstPhase();
	cropTool.secoundPhase();
	cropTool.thirdPhase();

	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Done in: " << t << " sec." << endl;

	for (int i = 0; i < 3; i++)
	{
		Window::Draw(*cropTool.mPotentialPlates[i].mPlate);
	}

	waitKey(0);
	system("pause");
	return 0;
}