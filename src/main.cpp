#include "imgProcessing.h"
#include "imgAnalysis.h"
#include "imgRendering.h"

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
	
	Mat* res = imgFilter(image, verticalDetectionMat);
	vector<int>* verticalProj = imgProjection(*res, false);
	vector<int>* horizontalProj = imgProjection(*res, true);
	
	t = ((double)getTickCount() - t) / getTickFrequency();
	cout << "Done in: " << t << " sec." << endl;

	Window::Draw(*res);
	Window::Draw(*verticalProj, false);
	Window::Draw(*horizontalProj, true);
	
	waitKey(0);
	delete res;
	delete verticalProj;
	delete horizontalProj;
	return 0;
}