#include <iostream>
#include <cstdlib>
using namespace std;
#include "Enums.hpp"

#include <unistd.h>

#define DEBUG(x) do{ std::cout << #x << " = " << x << std::endl; }while(0)

#include "opencv2/opencv.hpp"
using namespace cv;

int hl1=0, hh1=14, hl2=160, hh2=179, sl=120, sh=255, vl=74, vh=218; // thresholds for RED, set as starting slider positions
int cl=0, ch=0; 		// Canny parameters slider positions
int dw=5; 				// dilation width slider position
int ath=8000; 			// threshold for recognising area as contour, also has a slider

string filename = "sign_set.jpg";


void sliders()
{
	namedWindow("Sliders", (240, 200));

	createTrackbar("Hue low 1", "Sliders", &hl1, 179);
	createTrackbar("Hue high 1", "Sliders", &hh1, 179);
	createTrackbar("Hue low 2", "Sliders", &hl2, 179);
	createTrackbar("Hue high 2", "Sliders", &hh2, 179);
	createTrackbar("Sat low", "Sliders", &sl, 255);
	createTrackbar("Sat high", "Sliders", &sh, 255);
	createTrackbar("Val low", "Sliders", &vl, 255);
	createTrackbar("Val high", "Sliders", &vh, 255);

	createTrackbar("Canny low", "Sliders", &cl, 199);
	createTrackbar("Canny high", "Sliders", &ch, 200);

	createTrackbar("Dilat weight", "Sliders", &dw, 15);

	createTrackbar("Contour area th", "Sliders", &ath, 32767);
}

void getCountours(Mat src, Mat imgDil, Mat img)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


	Mat maskHSV, maskHSV1, maskHSV2;
	vector<vector<Point>> contPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Mat> imgsCropped;
	string objectType;
	Mat warpMatrix, signWarped;
	Mat fg, bg;
	Mat bgmsk = Mat::zeros(src.rows, src.cols, CV_8UC1);


	for(int i = 0; i < contours.size(); i++)
	{
		cout << contours.size() << " contours found" << endl;
		int area = contourArea(contours[i]);
		//cout << area << endl;

		drawContours(img, contours, i, Scalar(112, 67, 255), 2);
		Mat signCropped, signColored, signColoredHSV, signCroppedResized;
		if(area > ath)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], contPoly[i], 0.02*peri, true);
			//cout << contPoly[i].size() << endl; // broj uglova
			boundRect[i] = boundingRect(contPoly[i]);

			// ovde kropujem, bojim, ubacujem u neki vektor
			Rect roi(boundRect[i].tl(), boundRect[i].br());

			signColored = src.clone();
			cvtColor(signColored, signColoredHSV, COLOR_BGR2HSV);


			// prodji kroz sve boje, odradi maskiranje, pa tako i farbanje
			// TAG : Obojiti segmente predefinisanim bojama

			for(int j = 0; j < ColorsThresholds.size(); j++)
			{
				inRange(signColoredHSV, Scalar(ColorsThresholds[j][0], ColorsThresholds[j][4], ColorsThresholds[j][6]), Scalar(ColorsThresholds[j][1], ColorsThresholds[j][5], ColorsThresholds[j][7]), maskHSV1);
				inRange(signColoredHSV, Scalar(ColorsThresholds[j][2], ColorsThresholds[j][4], ColorsThresholds[j][6]), Scalar(ColorsThresholds[j][3], ColorsThresholds[j][5], ColorsThresholds[j][7]), maskHSV2);
				bitwise_or(maskHSV1, maskHSV2, maskHSV);


				for(int r = 0; r < signColored.rows; r++)
				{
					for(int c = 0; c < signColored.cols; c++)
					{
						if(maskHSV.at<uchar>(r,c) == 255)
						{
							Vec3b & color = signColored.at<Vec3b>(r, c);

							color[0] = ColorsBGR[j][0];
							color[1] = ColorsBGR[j][1];
							color[2] = ColorsBGR[j][2];
						}
					}
				}
				//imshow(to_string(j)+"colors", signColored);
			}

			// TAG: Pozadina kao zasebna
			drawContours(bgmsk, contours, i, Scalar(255,255,255), FILLED);

			vector<Point> insidePoints;
			findNonZero(bgmsk, insidePoints);

			bgmsk = 255-bgmsk;
			vector<Point> outsidePoints;
			findNonZero(bgmsk, outsidePoints);
			for(int r = 0; r < signColored.rows; r++)
			{
				for(int c = 0; c < signColored.cols; c++)
				{
					if(bgmsk.at<uchar>(r,c) == 255)
					{
						Vec3b & color = signColored.at<Vec3b>(r, c);

						color[0] = ColorsBGR[SEGM_BACKGROUND][0];
						color[1] = ColorsBGR[SEGM_BACKGROUND][1];
						color[2] = ColorsBGR[SEGM_BACKGROUND][2];
					}
				}
			}


			int objCor = (int)contPoly[i].size();
			if(objCor == 3)
			{
				objectType = "Triangle";
			}
			else if(objCor == 4)
			{
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				if(aspRatio > 0.95 && aspRatio < 1.05)
					objectType = "Square";
				else
					objectType = "Rectangle";
			}
			else if(objCor == 8)
			{
				objectType = "Octagon";
			}
			else if(objCor > 8)
			{
				objectType = "Circle";
			}
			else
			{
				objectType = "None";
			}

			// TAG : iseci visak slike
			signCropped = signColored(roi);


			// TAG : skalirati znakove

			resize(signCropped, signCroppedResized, Size(), 2, 2);


			//  TAG : transformisati zakrivljene znakove

			// iz nekropovane bi islo ovako
			//Point2f srcPoints[4] = { {boundRect[i].x, boundRect[i].y}, {boundRect[i].x+boundRect[i].width, boundRect[i].y}, {boundRect[i].x, boundRect[i].y+boundRect[i].height}, {boundRect[i].x+boundRect[i].width, boundRect[i].y+boundRect[i].height} };
			// iz kropovane prosto ovako
			Point2f srcPoints[4] = { {0.0f, 0.0f}, {0.0f+boundRect[i].width, 0.0f}, {0.0f, 0.0f+boundRect[i].height}, {0.0f+boundRect[i].width, 0.0f+boundRect[i].height} };
			Point2f dstPoints[4] = { {0.0f, 0.0f}, {400, 0.0f}, {0.0f, 400}, {400, 400} };

			warpMatrix = getPerspectiveTransform(srcPoints, dstPoints);
			warpPerspective(signCropped, signWarped, warpMatrix, Point(400, 400));


			//imshow(to_string(i)+"signColored", signColored);
			//imshow(to_string(i)+"signCropped", signCropped);

			imshow(to_string(i)+"signCroppedResized", signCroppedResized);

			imshow(to_string(i)+"signWarped", signWarped);

			string outname = ("outs/" + to_string(i) + filename);
			imwrite(outname, signWarped);


			drawContours(img, contPoly, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(20, 255, 20), 4);
			putText(img, objectType, {boundRect[i].x, boundRect[i].y-3}, FONT_HERSHEY_PLAIN, 1.65, Scalar(111, 30, 52), 2.4);

		}

	}
}

int main() {
	Mat src = cv::imread("data/"+filename);
	if(src.empty()){
		throw runtime_error("Cannot open image!");
	}
	cout << "DEBUG _ _ _ _ _ _ _ _ _ start" << endl;

	Mat imgHSV, imgBlur, imgCanny, imgDil, imgCont;
	Mat resultHSV, resultHSV1, resultHSV2, maskHSV, maskHSV1, maskHSV2;

	cvtColor(src, imgHSV, COLOR_BGR2HSV);

	sliders();


	while(true)
	{
		inRange(imgHSV, Scalar(hl1, sl, vl), Scalar(hh1, sh, vh), maskHSV1);
		inRange(imgHSV, Scalar(hl2, sl, vl), Scalar(hh2, sh, vh), maskHSV2);
		maskHSV = maskHSV1 + maskHSV2;

		GaussianBlur(maskHSV, imgBlur, Size(5,5), 3, 0);
		Canny(imgBlur, imgCanny, cl, ch);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(dw, dw));
		dilate(imgCanny, imgDil, kernel);

		imgCont = src.clone();

		getCountours(src, imgDil, imgCont);




		//bitwise_and(imgHSV, imgHSV, resultHSV1, maskHSV1);
		//bitwise_and(imgHSV, imgHSV, resultHSV2, maskHSV2);
		//resultHSV = resultHSV1 + resultHSV2;
		//cvtColor(resultHSV, src, COLOR_HSV2BGR);

		imshow("Src", src);
		//imshow("Image Mask", maskHSV);
		//imshow("Image Canny", imgCanny);
		//imshow("Image Dilated", imgDil);
		imshow("Image Contours", imgCont);
		waitKey(1);
	}


	cout << "DEBUG _ _ _ _ _ _ _ _ _ end" << endl;

	return 0;
}
