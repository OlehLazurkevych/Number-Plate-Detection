#include "numberPlateRecognitionTool.h"

NumberPlateRecognitionTool::NumberPlateRecognitionTool(const string numbersFeaturesFilePath, const string lettersFeaturesFilePath)
{
	mPCT = new PlateCropTool();
	mST = new SegmentationTool();
	mCRT = new CharRecognitionTool(numbersFeaturesFilePath, lettersFeaturesFilePath);

	mDB = vector<CarData> {
		CarData("AA7397PA", "Andrew Mark", "X3", "4352672135"),
		CarData("BC5941HA", "Denis Lebowski", "Superb", "7863266333"),
		CarData("AA0055BP", "Luie Viton", "S500", "2223266333"),
		CarData("BC9469HC", "Den Glourin", "Peugeot", "4367547357"),
		CarData("BH4444CA", "Mery James", "LX570", "7697497452"),
		CarData("BC0644AX", "Zeus Navi", "Audi", "2223232323"),
		CarData("BX1841BM", "Nikolai Meh", "FIAT", "6357684873"),
		CarData("AH1818KE", "Nikolai Sinior", "X7", "5474352646"),
		CarData("BC9959EB", "Olexy Olexa", "Accent", "2565776764"),
		CarData("BX5222AO", "Sergey Belyi", "E250", "3724653687"),
		CarData("BC6945BK", "Oleg Lazurkevych", "Peugeot Expr", "6666666666"),
		CarData("BH8555HM", "Olexandr Voz", "BMW s", "4466543466"),
		CarData("AA7377PA", "Tony J", "X4", "7735466477"),
		CarData("BC9944CA", "Yryi Melnik", "Tesla", "5555554444"),
		CarData("BK5044CA", "Young Janix", "Porsche 911", "6554534534"),
	};
}

NumberPlateRecognitionTool::~NumberPlateRecognitionTool()
{
	delete mPCT;
	delete mST;
	delete mCRT;

	if (mSource)
	{
		delete mSource;
	}
}

void NumberPlateRecognitionTool::tryRecognize(Mat& image)
{
	vector<Mat> segments;
	
	if (mSource)
	{
		delete mSource;
		mSource = nullptr;
	}
	if (mCroppedPlate)
	{
		delete mCroppedPlate;
		mCroppedPlate = nullptr;
	}

	mTime = (double)getTickCount();

	mSource = imgGetGray(image);
	mCroppedPlate = mPCT->getPlate(*mSource);
	segments = mST->getSegments(*mCroppedPlate);

	string fLet = "(.*)";
	string numbers = "";
	string sLet = "(.*)";

	try
	{
		mCRT->setSegments(segments);
		
		numbers = mCRT->next();
		
		vector<CarData> flou;
		for (int i = 0; i < mDB.size(); i++)
		{
			if (regex_match(mDB[i].Number, std::regex(fLet + numbers + sLet)))
			{
				flou.push_back(mDB[i]);
			}
		}

		if (flou.size() > 1)
		{
			sLet = mCRT->next();
			vector<CarData> flou2;
			for (int i = 0; i < flou.size(); i++)
			{
				if (regex_match(flou[i].Number, std::regex(fLet + numbers + sLet)))
				{
					flou2.push_back(flou[i]);
				}
			}

			if (flou2.size() > 1)
			{
				fLet = mCRT->next();
				vector<CarData> flou3;
				for (int i = 0; i < flou2.size(); i++)
				{
					if (regex_match(flou2[i].Number, std::regex(fLet + numbers + sLet)))
					{
						flou3.push_back(flou2[i]);
					}
				}

				if ((fLet + numbers + sLet).size() < 8)
				{
					sLet += "(.*)";
					flou3.clear();
					for (int i = 0; i < flou2.size(); i++)
					{
						if (regex_match(flou2[i].Number, std::regex(fLet + numbers + sLet)))
						{
							flou3.push_back(flou2[i]);
						}
					}
				}
				
				for (int i = 0; i < flou3.size(); i++)
				{
					cout << flou3[i] << endl;
				}
			}
			else
			{
				for (int i = 0; i < flou2.size(); i++)
				{
					cout << flou2[i] << endl;
				}
			}
		}
		else
		{
			for (int i = 0; i < flou.size(); i++)
			{
				cout << flou[i] << endl;
			}
		}
	}
	catch (exception e)
	{
		mTime = ((double)getTickCount() - mTime) / getTickFrequency();
		throw e;
	}

	mTime = ((double)getTickCount() - mTime) / getTickFrequency();
}

void NumberPlateRecognitionTool::showCashData(bool additionalData)
{
	cout << endl << "Done in   :   " << mTime << " sec." << endl;

	//Window::Draw(*mSource);
	//Window::Draw(*mCroppedPlate);
	//mCRT->drawAll();

	if (additionalData)
	{
		Window::Draw(*mPCT->mPotentialPlates[0].mPlate);
		Window::Draw(*mPCT->mPotentialPlates[1].mPlate);
		Window::Draw(*mPCT->mPotentialPlates[2].mPlate);
	}
}