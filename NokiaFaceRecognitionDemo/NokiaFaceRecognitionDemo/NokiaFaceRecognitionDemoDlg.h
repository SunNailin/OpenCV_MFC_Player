#ifdef NOKIADLG_H
#define NOKIADLG_H

#endif
// NokiaFaceRecognitionDemoDlg.h : 头文件
//

#pragma once
//#include "Constants.h"
#include "win_socket.h"

const static char* ipOp = "10.3.8.211";
const static int portOp = 10086;


// CNokiaFaceRecognitionDemoDlg 对话框
class CNokiaFaceRecognitionDemoDlg : public CDialogEx
{
// 构造
public:
	CNokiaFaceRecognitionDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NOKIAFACERECOGNITIONDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	rgbFrame *m_pFrameBuffer;
	rgbFrame m_CurFrame2Send;
	Param2SendFrame m_Param2SendFrame;
	cv::Mat RcvRgbFrmFromDecoderServer();
	virtual void OnFinalRelease();
	// 用于通过Socket发送当前帧进行检测的线程
	static UINT ThreadSendFrame2Detect(LPVOID pParam);
	// 用于从Socket接收检测结果
	static DWORD WINAPI ThreadReceiveRecResult(LPVOID pParam);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CvCapture* m_pCapture ;
private:
	
	rgbFrame* m_buffer;	
};
