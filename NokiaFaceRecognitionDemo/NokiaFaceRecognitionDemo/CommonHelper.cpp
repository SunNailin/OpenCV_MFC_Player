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
	unsigned char *arrayData=new unsigned char[matFrame.rows*matFrame.cols];
    if (matFrame.isContinuous())
        arrayData = matFrame.data;
    Mat myimg(1920,1080, CV_8UC3,arrayData);
    for (int i = 0; i < matFrame.rows * matFrame.cols * 3; i++) {
       myimg.at<unsigned char>(i) = (unsigned char)arrayData[i]; 
    }
	return arrayData;
}


