
#pragma once

#include "opencv2/opencv.hpp"
#include <string>

using namespace std;
using namespace cv;

static string SignTypes[] = {
	"Stop",
	"Priority",
	"Crosswalk",
	"Parking",
	"Proceed straight",
	"One-way",
	"Danger",
	"No parking",
	"No priority", // == yield
	"Pedestrian zone",
	"Highway Entry",
	"Highway Exit",
	"Black spot"
};
enum ColorsEnum{
	SEGM_RED,
	SEGM_GREEN,
	SEGM_BLUE,
	SEGM_YELLOW,
	SEGM_BLACK,
	SEGM_WHITE,
	SEGM_BACKGROUND
};
// hl1, hh1, hl2, hh2, sl, sh, vl, vh
// thresholdi se eksperimentalno pronalaze, i zatim beleze
// fora je da opsezi (low-high) budu minimalni, a da i dalje nadje ceo znak i nista od pozadine

vector<vector<int>> ColorsThresholds{
	{0, 14, 160, 179, 120, 255, 74, 218}, //RED
	{81, 92, 81, 92, 95, 255, 74, 134}, //GREEN
	{104, 113, 104, 113, 183, 255, 117, 244}, //BLUE
	{20, 28, 20, 28, 89, 255, 116, 255}, //YELLOW
	{0, 179, 0, 179, 0, 130, 0, 99}, //BLACK
	{0, 179, 0, 179, 0, 45, 162, 255} //WHITE
};

//   u  BGR  !!
vector<Scalar> ColorsBGR{
	{0, 0, 255}, //RED
	{0, 255, 0}, //GREEN
	{255, 0, 0}, //BLUE
	{0, 255, 255}, //YELLOW
	{0, 0, 0}, //BLACK
	{255, 255, 255}, //WHITE
	{161, 111, 245} //BACKGROUND  (HSV -> 168, 140, 244)
};
