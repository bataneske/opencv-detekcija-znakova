
#pragma once

#include "opencv2/opencv.hpp"
#include <string>

enum HSV_Segmented_Sign_Enum {
	SEGM_BACKGROUND,
	SEGM_RED,
	SEGM_GREEN,
	SEGM_BLUE,
	SEGM_YELLOW,
	SEGM_BLACK,
	SEGM_WHITE
};


static cv::Scalar segm_to_color[] = {
	cv::Scalar(161, 111, 245),
	cv::Scalar(0, 0, 255),
	cv::Scalar(0, 255, 0),
	cv::Scalar(255, 0, 0),
	cv::Scalar(0, 255, 255),
	cv::Scalar(0, 0, 0),
	cv::Scalar(255, 255, 255)
};


static std::string sign_types[] = {
	"Stop",
	"Priority",
	"Crosswalk",
	"Parking",
	"Proceed straight",
	"One-way",
	"Danger",
	"No parking",
	"No priority",
	"Pedestrian zone",
	"Highway Entry",
	"Highway Exit",
	"Black spot"
};
