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
int ath=5000; 			// threshold for recognising area as contour, also has a slider

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
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

	Mat maskHSV, maskHSV1, maskHSV2;
	vector<vector<Point>> contPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Mat> imgsCropped;
	string objectType;
	Mat warpMatrix, signWarped;

	for(int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		//cout << area << endl;

		Mat signCropped, signCroppedHSV, signColored, signCroppedResized, signColoredResized;
		if(area > ath)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], contPoly[i], 0.02*peri, true);
			//cout << contPoly[i].size() << endl; // broj uglova
			boundRect[i] = boundingRect(contPoly[i]);

			// ovde kropujem, bojim, ubacujem u neki vektor
			Rect roi(boundRect[i].tl(), boundRect[i].br());


			// TAG : iseci visak slike

			// znaci imas signColored koji je BGR i imas signCropped koji je HSV
			// na signCropped ces traziti boje  ( tako sto maskiras sa opsegom za neku boju )
			// na signColored ces crtati ( tako sto sve delove koji pripadaju maski obojas tom bojom )
			signColored = src(roi);
			cvtColor(signColored, signCropped, COLOR_BGR2HSV);


			// prodji kroz sve boje, odradi maskiranje, pa tako i farbanje

			cout << "DEBUG _ _ _ _ _ _ _ _ _ 1" << endl;
			/*
			// TAG: Pozadina kao zasebna

			// ako bi prvo ofarbao celu kopiju u BACKGROUND, to bi bilo lakse

			// TAG : Obojiti segmente predefinisanim bojama

			for(int i = 0; i < ColorsThresholds.size(); i++)
			{
				inRange(signCropped, Scalar(ColorsThresholds[i][0], ColorsThresholds[i][4], ColorsThresholds[i][6]), Scalar(ColorsThresholds[i][1], ColorsThresholds[i][5], ColorsThresholds[i][7]), maskHSV1);
				inRange(signCropped, Scalar(ColorsThresholds[i][2], ColorsThresholds[i][4], ColorsThresholds[i][6]), Scalar(ColorsThresholds[i][3], ColorsThresholds[i][5], ColorsThresholds[i][7]), maskHSV2);
				maskHSV = maskHSV1 + maskHSV2;
				for(int r=0; r<maskHSV.rows; r++)
				{
					for(int c=0; c<maskHSV.cols; c++)
					{
						if(maskHSV.at<uint>(r, c) != 0)
						{
							//signColored.at<double>(r, c) = ColorsBGR[i];
							cout << i << " " << maskHSV.at<uint>(r, c) << "       " << r << " - " << c << " "  << ColorsBGR[i] << endl;
							//signColored.at<Vec3b>(r,c) = Scalar(ColorsBGR[i][0], ColorsBGR[i][1], ColorsBGR[i][2]);

							// uklopi jednu po jednu boju u kopiju kropovane slike

						}
					}
				}
				cout << ColorsBGR[i] << endl;
			}
			cout << "DEBUG _ _ _ _ _ _ _ _ _ 7" << endl;

			// stavi te obojane slike u neki vektor obojanih slika
			//imgsCropped.push_back(signColored);

			*/
			cout << "DEBUG _ _ _ _ _ _ _ _ _ 8" << endl;

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


			// TAG : skalirati znakove

			resize(signCropped, signCroppedResized, Size(), 2, 2);
			resize(signColored, signColoredResized, Size(), 2, 2);


			//  TAG : transformisati zakrivljene znakove

			// iz nekropovane bi islo ovako
			//Point2f srcPoints[4] = { {boundRect[i].x, boundRect[i].y}, {boundRect[i].x+boundRect[i].width, boundRect[i].y}, {boundRect[i].x, boundRect[i].y+boundRect[i].height}, {boundRect[i].x+boundRect[i].width, boundRect[i].y+boundRect[i].height} };
			// iz kropovane prosto ovako
			Point2f srcPoints[4] = { {0.0f, 0.0f}, {0.0f+boundRect[i].width, 0.0f}, {0.0f, 0.0f+boundRect[i].height}, {0.0f+boundRect[i].width, 0.0f+boundRect[i].height} };
			Point2f dstPoints[4] = { {0.0f, 0.0f}, {400, 0.0f}, {0.0f, 400}, {400, 400} };

			warpMatrix = getPerspectiveTransform(srcPoints, dstPoints);
			warpPerspective(signColored, signWarped, warpMatrix, Point(400, 400));


			imshow(to_string(i)+"signCropped", signCropped);
			imshow(to_string(i)+"signColored", signColored);

			imshow(to_string(i)+"signCroppedResized", signCroppedResized);
			imshow(to_string(i)+"signColoredResized", signColoredResized);

			imshow(to_string(i)+"signWarped", signWarped);


			drawContours(img, contPoly, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(20, 255, 20), 4);
			putText(img, objectType, {boundRect[i].x, boundRect[i].y-3}, FONT_HERSHEY_PLAIN, 1.65, Scalar(111, 30, 52), 2.4);

		}

	}
}

int main() {
	Mat src = cv::imread("data/stop_sign.jpg");
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

		GaussianBlur(maskHSV, imgBlur, Size(3,3), 3, 0);
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
		imshow("Image Mask", maskHSV);
		imshow("Image Canny", imgCanny);
		imshow("Image Dilated", imgDil);
		imshow("Image Contours", imgCont);
		waitKey(1);
	}


	cout << "DEBUG _ _ _ _ _ _ _ _ _ end" << endl;

	return 0;
}


//// ////   GRAVEYARD   //// ////
/*
	//#define threshold 34 // TODO: replace threshold with slider values.
	Vec3b pixel(178, 207, 148); // HSV red pixel.
	// H komponenta se deli na 2 opsega  (jer je crvena u  0 i 179)

	if(pixel.val[0] < threshold) // 179-th+pval ... 179  AND   0 ... pval
	{
		hl1 = 179 - threshold + pixel.val[0];
		hh1 = 179;

		hl2 = 0;
		hh2 = pixel.val[0];
	}
	else if(pixel.val[0] > 179 - threshold) // pval ... 179  AND  0 ... pval+th-179
	{
		hl1 = pixel.val[0];
		hh1 = 179;

		hl2 = 0;
		hh2 = pixel.val[0] + threshold - 179;
	}
	else // pval-th ... pval+th
	{
		hl1 = pixel.val[0] - threshold;
		hh1 = pixel.val[0] + threshold;

		hl2 = pixel.val[0] - threshold;
		hh2 = pixel.val[0] + threshold;
	}

	// S i V komponente mogu samo da se zakucaju na min/max
	sl = (pixel.val[1] >= threshold) ? (pixel.val[1] - threshold) : 0;
	sh = (pixel.val[1] <= (255-threshold)) ? (pixel.val[1] + threshold) : 255;

	vl = (pixel.val[2] >= threshold) ? (pixel.val[2] - threshold) : 0;
	vh = (pixel.val[2] <= (255-threshold)) ? (pixel.val[2] + threshold) : 255;
*/
