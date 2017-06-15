#pragma once

#include <cv.h>
#include <highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;

class CCommonHelper
{
public:
	CCommonHelper(void);
	~CCommonHelper(void);
	unsigned char* cvMat2RGB(cv::Mat matFrame);
	cv::Mat RGB2cvMat(char* rgbData);
	//cv::Mat IplImage2cvMat(
};

