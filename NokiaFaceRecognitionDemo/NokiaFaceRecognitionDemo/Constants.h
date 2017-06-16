#pragma once

#ifndef __CONTANTS_H__
#define __CONTANTS_H__

#include <iostream>
#include <stdio.h>  
#include <windows.h>  //һ��Ҫ������ͷ�ļ�
#include <cv.h>
#include <highgui.h>

using namespace cv;
   
#pragma comment(lib, "WS2_32.lib") 
#define BUF_SIZE 1024
#define LEN_BUF_SIZE 4


#define MAX_HEAD_LEN    6			//����ͷ�����ݸ���
#define FACE_SIZE (224*224*3)		//
#define RECV_IMAGEHEAD_SIZE 8		//
#define HEAD_SIZE (36+FACE_SIZE)					//
#define MAX_RECV_HEAD_SIZE (MAX_HEAD_LEN*HEAD_SIZE)				//
#define IMAGE_WIDTH			1920	//ͼ��Ŀ��
#define IMAGE_HEIGHT		1080	//ͼ��ĸ߶�
#define MAX_FRAME_LEN		(IMAGE_WIDTH * IMAGE_HEIGHT * 3)	//ͼ��֡���ݴ�С
#define MAX_LEN (MAX_FRAME_LEN+1)									//ͼ��֡���ݴ�С+1�����ֽ�
#endif




typedef struct rgbFrame
{
	int frameNo;			//֡�ţ�0-109ѭ����
	int frameLen;			//֡���ݳ��ȣ���rgbData����
	//BYTE rgbData[MAX_FRAME_LEN];
	unsigned char* rgbData;			//rgb��������
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

