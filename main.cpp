
#include "Enums.hpp"

#include <visualizer.hpp>

#include <iostream>
#include <cstdlib>
using namespace std;

#include <unistd.h>

#define DEBUG(x) do{ std::cout << #x << " = " << x << std::endl; }while(0)
#define threshold 30

#include "opencv2/opencv.hpp"
using namespace cv;

void hsv_to_panels(Mat3b hsv_img)
{/*
	Mat3b H(hsv_img.size(), hsv_img.type()), S(hsv_img.size(), hsv_img.type()), V(hsv_img.size(), hsv_img.type());
	for(uint16_t i = 0; i < hsv_img.rows; i++)
	{
		for(uint16_t j = 0; j < hsv_img.cols; j++)
		{
			DEBUG(i);
			DEBUG(j);
			Vec3b pixel = hsv_img.at<Vec3b>(i, j);
			H.at<uchar>(i, j) = pixel.val[0];
			S.at<uchar>(i, j) = pixel.val[1];
			V.at<uchar>(i, j) = pixel.val[2];
		}
	}
		visualizer::img::show(
			"h",
			H
		);

		visualizer::img::show(
			"s",
			S
		);

		visualizer::img::show(
			"v",
			V
		);
*/}

int main() {
	Mat src = cv::imread("data/stop_sign.jpg");
	Mat src2 = cv::imread("data/1l.jpg");
	if(src.empty()){
		throw runtime_error("Cannot open image!");
	}
	cout << "DEBUG _ _ _ _ _ _ _ _ _ 1" << endl;
	visualizer::load_cfg("data/main.visualizer.yaml");

	cout << "DEBUG _ _ _ _ _ _ _ _ _ 2" << endl;
	const uint16_t width = 10;
	const uint16_t height = 20;
	static uint8_t pix[width*height*3];

	// BGR to HSV:
	Mat3b src2hsv;
	cvtColor(src2, src2hsv, COLOR_BGR2HSV);

	cout << "DEBUG _ _ _ _ _ _ _ _ _ 3" << endl;
	//HSV to H, S and V displays:
	hsv_to_panels(src2hsv);

	cout << "DEBUG _ _ _ _ _ _ _ _ _ 4" << endl;
	/*visualizer::img::show(
		"src",
		src
	);

	visualizer::img::show(
		"h",
		src
	);

	visualizer::img::show(
		"r",
		src2
	);*/


	int th_start_h0;

	cout << "DEBUG _ _ _ _ _ _ _ _ _ 5" << endl;
	while(true){

		cout << "DEBUG _ _ _ _ _ _ _ _ _ 6" << endl;
		th_start_h0 = 20;
		visualizer::slider::slider(
			"/win0/upper_half/upper_rigth_corner/th_start_h0",
			th_start_h0,
			[&](int& value){
				DEBUG(th_start_h0);
			}
		);

		cout << "DEBUG _ _ _ _ _ _ _ _ _ 7" << endl;
		for(int i = 0; i < 3; i++){
			visualizer::slider::update();
			DEBUG(th_start_h0);
			sleep(1);
		}


		cout << "DEBUG _ _ _ _ _ _ _ _ _ 8" << endl;
	}

	cout << "DEBUG _ _ _ _ _ _ _ _ _ 9" << endl;

	return 0;
}


/*for(uint16_t y = 0; y < height; y++){
	for(uint16_t x = 0; x < width; x++){
		uint32_t i = (y*width + x)*3;
		// Red.
		pix[i+0] = 0;
		pix[i+1] = 0;
		pix[i+2] = 255;
	}
}

for(uint16_t y = 3; y < height-3; y++){
	for(uint16_t x = 3; x < width-3; x++){
		uint32_t i = (y*width + x)*3;
		// Blue.
		pix[i+0] = 255;
		pix[i+1] = 0;
		pix[i+2] = 0;
	}
}*/
