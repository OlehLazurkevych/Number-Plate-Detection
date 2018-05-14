#include "openCV.h"
#include "numberPlateRecognitionTool.h"

int main(int argc, char** argv)
{
	Mat image = imread("E:/Projects/Number-Plate-Recognition/data/training-chars/K/1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	image = *imgLocalThresh(image, 0, 0);
	NumberPlateRecognitionTool NPRT;
	CharRecognitionTool test;

	Window::Draw(image);

	

	vector<Point2f> res = test.getLEndsVec(image);

	for (int i = 0; i < res.size(); i++)
	{
		cout << res[i].x << "  " << res[i].y << endl;
	}

	cout << test.getLoopQuantity(image) << endl;

	/*if (image.empty())
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
	}*/
	
	waitKey(0);
	system("pause");
	return 0;
}


void 