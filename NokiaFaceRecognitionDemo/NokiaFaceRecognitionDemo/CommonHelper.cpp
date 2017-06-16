#include "StdAfx.h"
#include "CommonHelper.h"


CCommonHelper::CCommonHelper(void)
{
}


CCommonHelper::~CCommonHelper(void)
{
}


 unsigned char* CCommonHelper::cvMat2RGB(cv::Mat matFrame)
{
	unsigned char *arrayData=new unsigned char[matFrame.rows*matFrame.cols+1];
	if (matFrame.isContinuous())
		arrayData = matFrame.data;
	return arrayData;
}

 cv::Mat CCommonHelper::RGB2cvMat(unsigned char* rgbData)
{
	Mat myimg(IMAGE_HEIGHT,IMAGE_WIDTH, CV_8UC3,rgbData);
	return myimg;
}


