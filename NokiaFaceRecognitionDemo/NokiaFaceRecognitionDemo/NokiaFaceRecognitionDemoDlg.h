#ifdef NOKIADLG_H
#define NOKIADLG_H

#endif
// NokiaFaceRecognitionDemoDlg.h : ͷ�ļ�
//

#pragma once
//#include "Constants.h"
#include "win_socket.h"

const static char* ipOp = "10.3.8.211";
const static int portOp = 10086;


// CNokiaFaceRecognitionDemoDlg �Ի���
class CNokiaFaceRecognitionDemoDlg : public CDialogEx
{
// ����
public:
	CNokiaFaceRecognitionDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NOKIAFACERECOGNITIONDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	// ����ͨ��Socket���͵�ǰ֡���м����߳�
	static UINT ThreadSendFrame2Detect(LPVOID pParam);
	// ���ڴ�Socket���ռ����
	static DWORD WINAPI ThreadReceiveRecResult(LPVOID pParam);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CvCapture* m_pCapture ;
private:
	
	rgbFrame* m_buffer;	
};
