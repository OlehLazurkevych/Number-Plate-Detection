#include "openCV.h"
#include "plateCropTool.h"
#include "segmentationTool.h"

int main(int argc, char** argv)
{
	Mat image = imread("E:/Projects/Number-Plate-Recognition/data/photo-of-cars/1.jpg");

	if (image.empty())
	{
		cout << "<Error>  :  Could not open or find the image!" << endl;
		system("pause");
		return -1;
	}
	
	Mat* croppedPlate;
	vector<Mat> segments;

	double t = (double)getTickCount();//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	
	PlateCropTool cropTool;
	SegmentationTool segmentationTool;

	image = *imgGetGray(image);
	croppedPlate = cropTool.getPlate(image);
	segments = segmentationTool.getSegments(*croppedPlate);
	if (segments.size() == 0)
	{
		cout << "Plate not found" << endl;
	}

	t = ((double)getTickCount() - t) / getTickFrequency();//|||||||||||||||||||||||||||||||||||||||||||||
	cout << endl << "Done in: " << t << " sec." << endl;
	
	Window::Draw(*croppedPlate);

	for (int i = 0; i < segments.size(); i++)
	{
		Window::Draw(segments[i]);
	}

	//Window::Draw(image);

	//Window::Draw(*cropTool.mPotentialPlates[0].mPlate);
	//Window::Draw(*cropTool.mPotentialPlates[1].mPlate);
	//Window::Draw(*cropTool.mPotentialPlates[2].mPlate);

	waitKey(0);
	system("pause");
	return 0;
}