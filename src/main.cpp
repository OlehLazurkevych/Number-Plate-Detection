#include "openCV.h"
#include "numberPlateRecognitionTool.h"



vector<Point2f> Train(string path, int quantity)
{
	CharRecognitionTool test("", "");
	vector<Point2f> res;
	char n = '0';
	char sn = '0' - 1;
	string postf = ".jpg";

	for (int i = 0; i < quantity; i++)
	{
		string dir;
		if (n == '9' || sn != '0' - 1)
		{
			n = '0';
			dir = path + (++n) + (++sn) + postf;
		}
		else
		{
			dir = path + (++n) + postf;
		}
		Mat image = imread(dir, CV_LOAD_IMAGE_GRAYSCALE);
		if (!image.empty())
		{
			image = *imgLocalThresh(image, 0, 0);

			//vector<Point2f> temp = test.getLEndsVec(image);
			vector<Point2f> temp = test.getJunctionsVec(image);

			if (res.empty())
			{
				res = temp;
				for (int i = 0; i < res.size(); i++)
				{
					cout << res[i].x << "  " << res[i].y << endl;
				}
				cout << "======================================================" << endl;
			}
			else
			{
				list<Point2f> lis;

				for (int j = 0; j < temp.size(); j++)
				{
					lis.push_back(temp[j]);
				}

				list<Point2f>::iterator it = lis.begin();
				list<Point2f>::iterator itmin = it;

				float min;

				for (int j = 0; j < res.size(); j++)
				{
					if (!lis.empty())
					{
						it = lis.begin();
						itmin = it;
						min = abs(res[j].x - it->x) + abs(res[j].y - it->y);

						for (; it != lis.end(); ++it)
						{
							if (abs(res[j].x - it->x) + abs(res[j].y - it->y) < min)
							{
								min = abs(res[j].x - it->x) + abs(res[j].y - it->y);
								itmin = it;
							}
						}

						res[j].x += itmin->x;
						res[j].x /= 2;
						res[j].y += itmin->y;
						res[j].y /= 2;

						lis.remove(*itmin);
					}
					else
					{
						Window::Draw(image);
						cout << "Train() ! WARNING   :   list is shorter than actual result vector" << endl;
						break;
					}
				}
			}

			cout << dir << endl;
		}
		else
		{
			cout << "Train() ! FILE NOT FOUND   :   " << dir << endl;
		}
	}

	return res;
}



int main(int argc, char** argv)
{
	Mat image = imread("E:/Projects/Number-Plate-Recognition/data/photo-of-cars/1.jpg");
	NumberPlateRecognitionTool NPRT("E:/Projects/Number-Plate-Recognition/data/char-features/f-numbers.txt",
									"E:/Projects/Number-Plate-Recognition/data/char-features/f-letters.txt");
	

	/*vector<Point2f> res = Train("E:/Projects/Number-Plate-Recognition/data/training-chars/H/", 4);
	for (int i = 0; i < res.size(); i++)
	{
		cout << res[i].x << "  " << res[i].y << endl;
	}*/


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