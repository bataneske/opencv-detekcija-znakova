
#include <visualizer.hpp>

#include <iostream>
#include <cstdlib>
using namespace std;

#include <unistd.h>

#define DEBUG(x) do{ std::cout << #x << " = " << x << std::endl; }while(0)

#include "opencv2/opencv.hpp"
using namespace cv;

int main() {
	Mat src = cv::imread("data/stop_sign.jpg");
	if(src.empty()){
		throw runtime_error("Cannot open image!");
	}

	visualizer::load_cfg("data/test.visualizer.yaml");

	vector<double> x = {0, 1, 2, 3, 4, 5};
	vector<double> y1 = {1, 2, 3, 4, 5, 6};
	vector<double> y2 = {9, 8, 7, 6, 5, 4};

	visualizer::plot::clear();
	visualizer::plot::plot("TODO", 0, x, y1, "y1");
	visualizer::plot::plot("TODO", 1, x, y2, "y2");

	const uint16_t width = 10;
	const uint16_t height = 20;
	static uint8_t pix[width*height*3];
	for(uint16_t y = 0; y < height; y++){
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
	}

	// visualizer::img::show(
	// 	"/win0/upper_half/src",		//TODO implement endsWith "/src"
	// 	pix,
	// 	width,
	// 	height
	// );


	// visualizer::img::show(
	// 	"/win0/lower_half/hsv/h",	//TODO implement endsWith "/hsv/h"
	// 	src
	// );

	visualizer::img::show(
		"upper_halfasdfasdf/src",		
		pix,
		width,
		height
	);


	visualizer::img::show(
		"h",	
		src
	);

	int th_start_h0;

	while(true){
		th_start_h0 = 20;
		visualizer::slider::slider(
			"/win0/upper_half/upper_rigth_corner/th_start_h0",
			th_start_h0,
			[&](int& value){
				DEBUG(th_start_h0);
			}
		);

		for(int i = 0; i < 3; i++){
			visualizer::slider::update();
			DEBUG(th_start_h0);
			sleep(1);
		}
	}


	// <3 TODO create everything from config file and access by full or partial unique path
	/*
		visualizer::viewImg("win0/upper_half/upper_left_quater/src")

		visualizer::plot("win0/upper_half/upper_right_quater/hist", "H", TODO);
		visualizer::plot("hist", "S"); // partial unique path
		visualizer::plot("hist", "V");

		visualizer::viewImg("win0/lower_half/hsv/h", h); // Full path.
		visualizer::viewImg("hsv/s", s); // partial unique path
		visualizer::viewImg("hsv/v", v);

		visualizer::viewImg("win0/lower_half/rgb/r", r);
		visualizer::viewImg("rgb/g", g);
		visualizer::viewImg("rgb/b", b);
	*/

	//TODO usleep(1e6/25) visualizer::flush();
/*
#if 1
	cout << "Press ENTER key to exit..." << endl;
	getchar();
#else
	sleep(5);
#endif
*/

	return 0;
}
