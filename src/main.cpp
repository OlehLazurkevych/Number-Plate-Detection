#include "openCV.h"
#include "numberPlateRecognitionTool.h"

int main(int argc, char** argv)
{
	Mat image = imread("E:/Projects/Number-Plate-Recognition/data/photo-of-cars/1.jpg");
	NumberPlateRecognitionTool NPRT;

	if (image.empty())
	{
		cout << "Error   :   Could not open or find the image file" << endl;
	}
	else
	{
		try
		{
			NPRT.tryRecognize(image);
		}
		catch (exception e)
		{
			cout << e.what() << endl;
		}

		NPRT.showCashData();
	}
	
	waitKey(0);
	system("pause");
	return 0;
}