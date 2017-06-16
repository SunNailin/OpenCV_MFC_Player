#pragma once

#include <cv.h>
#include <highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "Constants.h"
using namespace cv;

class CCommonHelper
{
public:
	CCommonHelper(void);
	~CCommonHelper(void);
	static unsigned char* cvMat2RGB(cv::Mat matFrame);
	static cv::Mat RGB2cvMat(unsigned char* rgbData);
	//cv::Mat IplImage2cvMat(
};

