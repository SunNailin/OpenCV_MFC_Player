
// NokiaFaceRecognitionDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNokiaFaceRecognitionDemoApp:
// �йش����ʵ�֣������ NokiaFaceRecognitionDemo.cpp
//

class CNokiaFaceRecognitionDemoApp : public CWinApp
{
public:
	CNokiaFaceRecognitionDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNokiaFaceRecognitionDemoApp theApp;