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
	
	plateCropTool cropTool(image);

	cropTool.firstPhase();
	
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Done in: " << t << " sec." << endl;

	waitKey(0);
	return 0;
}