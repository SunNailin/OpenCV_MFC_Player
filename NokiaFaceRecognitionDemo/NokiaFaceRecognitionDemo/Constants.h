#pragma once

#ifndef __CONTANTS_H__
#define __CONTANTS_H__

#include <iostream>
#include <stdio.h>  
#include <windows.h>  //一定要包含该头文件
#include <cv.h>
#include <highgui.h>

using namespace cv;
   
#pragma comment(lib, "WS2_32.lib") 
#define BUF_SIZE 1024
#define LEN_BUF_SIZE 4


#define MAX_HEAD_LEN    6			//最大的头像数据个数
#define FACE_SIZE (224*224*3)		//
#define RECV_IMAGEHEAD_SIZE 8		//
#define HEAD_SIZE (36+FACE_SIZE)					//
#define MAX_RECV_HEAD_SIZE (MAX_HEAD_LEN*HEAD_SIZE)				//
#define IMAGE_WIDTH			1920	//图像的宽度
#define IMAGE_HEIGHT		1080	//图像的高度
#define MAX_FRAME_LEN		(IMAGE_WIDTH * IMAGE_HEIGHT * 3)	//图像帧数据大小
#define MAX_LEN (MAX_FRAME_LEN+1)									//图像帧数据大小+1冗余字节
#endif




typedef struct rgbFrame
{
	int frameNo;			//帧号（0-109循环）
	int frameLen;			//帧数据长度，即rgbData长度
	//BYTE rgbData[MAX_FRAME_LEN];
	unsigned char* rgbData;			//rgb数据内容
	//rgbFrame(int frmNo, int frmLen, char* rgbD):frameNo(frmNo),frameLen(frmLen),rgbData(rgbD){}
};
//*pRGBFram;
#define OFFSET_RGBDATAINFRAME	4

struct Imagehead{
	int Frame_No;
	int Head_num;
};
//*pImageHead;

struct HEAD{
	int Frame_No;
	int HeadIndex;
	int x;
	int y;
	int width;
	int height;
	int facename;
	int Pre_Frame_No;
	int PrevHeadIndex;
	unsigned char Face[FACE_SIZE];
};

typedef struct Param2SendFrame
{
	CDialog* pDlg;
	rgbFrame* pImgFrame;
};
//*pParam2SendFrame;

