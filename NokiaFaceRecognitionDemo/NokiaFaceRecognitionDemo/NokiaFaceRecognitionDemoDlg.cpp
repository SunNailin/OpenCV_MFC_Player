
// NokiaFaceRecognitionDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NokiaFaceRecognitionDemo.h"
#include "NokiaFaceRecognitionDemoDlg.h"
#include "afxdialogex.h"
#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include <queue>
#include "CommonHelper.h"

#include "UserMessageDef.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const int MAX_BUFFERFRAMES = 110;	//缓冲区中最大帧数
char* ipAddr = "10.3.8.211";
const int portNo = 10086;
volatile int nCurFrameNum = 0;		//视频帧计数器
int nBufferedFrames = 0;			//缓冲区中帧数
int nCurBufferPointer = 0;			//当前缓冲指针
int nCurPlayPointer = 0;			//当前播放指针
BYTE curFrame[IMAGE_WIDTH][IMAGE_HEIGHT][3];

HANDLE hdlReceiveRecResult = NULL;		//读网络接收缓冲区所需要的线程的 句柄和ID
DWORD dwThreadIDReceiveRecResult = 0;
CEvent ceRecResultReceived;				//在读网络接收缓冲区线程中，如果接收到了完整的检测结果，触发此事件
//一帧视频数据发送后，一致等待此事件，当检测到事件后处理检测结果，显示


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNokiaFaceRecognitionDemoDlg 对话框




CNokiaFaceRecognitionDemoDlg::CNokiaFaceRecognitionDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNokiaFaceRecognitionDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNokiaFaceRecognitionDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNokiaFaceRecognitionDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CNokiaFaceRecognitionDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CNokiaFaceRecognitionDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CNokiaFaceRecognitionDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_TESTINTFACE, &CNokiaFaceRecognitionDemoDlg::OnBnClickedTestintface)
END_MESSAGE_MAP()


// CNokiaFaceRecognitionDemoDlg 消息处理程序

BOOL CNokiaFaceRecognitionDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	WinClient client(ipAddr,portNo);
	m_pFrameBuffer = (rgbFrame*)malloc(MAX_BUFFERFRAMES*(12+MAX_LEN));
	m_pCapture= cvCreateCameraCapture(0);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNokiaFaceRecognitionDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNokiaFaceRecognitionDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNokiaFaceRecognitionDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//显示原始视频流，并向检测端发送数据，维护缓冲区
void CNokiaFaceRecognitionDemoDlg::OnBnClickedOk()
{

	cvNamedWindow("originVideo", 1);
	while(true){		
		cv::Mat matFrame = CNokiaFaceRecognitionDemoDlg::RcvRgbFrmFromDecoderServer();
		if(matFrame.cols == 0) break;
		imshow("originVideo",matFrame); 
		char c = cv::waitKey(45);
		if(27 == c) break;
	}
	cvDestroyWindow("originVideo"); 



}

void CNokiaFaceRecognitionDemoDlg::OnFinalRelease()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (NULL != m_pFrameBuffer)
		delete[] m_pFrameBuffer;
	m_pFrameBuffer = NULL;
	CDialogEx::OnFinalRelease();
}

//模拟从解码服务器端获得图像数据
cv::Mat CNokiaFaceRecognitionDemoDlg::RcvRgbFrmFromDecoderServer()
{
	//    // TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	//	//比如这个函数是接收视频数据
	//	/*需要定义一个全局计数器
	//	  接收到视频数据后，因为要求两帧检测一次，所以可以考虑不保存偶数(奇数)帧
	//	*/
	//	rgbFrame rgbFrm;
	//	nCurFrameNum++;
	//	if (1 != (nCurFrameNum % 2))
	//	{//偶数帧不保存
	////		return;
	//	}
	//	//保存此帧数据到缓冲区
	//	nBufferedFrames++;
	//	nCurBufferPointer++;
	//	//bufferFrame [] = 
	//	if (nBufferedFrames > MAX_BUFFERFRAMES)
	//		nBufferedFrames = MAX_BUFFERFRAMES;
	//	nCurBufferPointer %= MAX_BUFFERFRAMES;//求余是为了对图像缓冲区进行循环利用
	//	//SaveFrame(nCurBufferPointer);假设这个是将当前帧保存到缓冲区的函数
	//	m_CurFrame2Send.frameNo = nCurBufferPointer;
	//	memcpy((BYTE*)&m_CurFrame2Send + OFFSET_RGBDATAINFRAME, curFrame, MAX_FRAME_LEN);
	//	m_Param2SendFrame.pDlg = this;//传递此指针是为了在线程中可以对对话框进行操作，比如显示图像
	//	m_Param2SendFrame.pImgFrame = &m_CurFrame2Send;
	//	AfxBeginThread(CNokiaFaceRecognitionDemoDlg::ThreadSendFrame2Detect, (LPVOID)&m_Param2SendFrame);
	//	return rgbFrm;

	//声明IplImage指针  
	IplImage* pFrame = NULL;  

	//获取摄像头  
	//CvCapture* pCapture = cvCreateCameraCapture(0);
	cv::Mat matFrame = (cv::Mat)cvQueryFrame( m_pCapture );
	//cvReleaseCapture(&m_pCapture); 
	//cv::waitKey(45);
	return matFrame;
	//cvDestroyWindow("video"); 
	//创建窗口  
	//cvNamedWindow("video", 1);  

	//显示视频  
	//while(1)  
	//{  
	//pFrame=cvQueryFrame( pCapture1 );  

	//if(!pFrame)break;  
	//cvShowImage("video",pFrame1);

	//char* rgbData = 

	//rgbFrame frame2send = new rgbFrame();

	//		CNokiaFaceRecognitionDemoDlg::ThreadSendFrame2Detect();

	//char c=cvWaitKey(33);  
	//if(c==27)break;  
	//}  
	/*cvReleaseCapture(&pCapture1);  
	cvDestroyWindow("video");  */
}

// 用于通过Socket发送当前帧进行检测
UINT CNokiaFaceRecognitionDemoDlg::ThreadSendFrame2Detect(LPVOID pParam)
{
	if (NULL == pParam)
		return 1;
	Param2SendFrame* pParamSent = (Param2SendFrame*)pParam;
	if (NULL == pParamSent)
		return 2;
	CNokiaFaceRecognitionDemoDlg* pThisDlg = (CNokiaFaceRecognitionDemoDlg*)pParamSent->pDlg;
	rgbFrame* pCurFrame = pParamSent->pImgFrame;
	if (NULL == pCurFrame)
		return 3;
	//在这里利用Socket的Send函数将此帧数据发送出去
	//SendData(pCurFrame);
	//传递消息，准备接收检测结果
	DWORD dwWaitResut = 0;
	ceRecResultReceived.ResetEvent();
	if (NULL == hdlReceiveRecResult)
		return 4;	//如果没有建立读网络接收缓冲区线程，则不接收结果，也不进行后续处理
	PostThreadMessage(dwThreadIDReceiveRecResult, WM_USERRECEIVERECRESULT, 0, 0);
	dwWaitResut = WaitForSingleObject(ceRecResultReceived.m_hObject, INFINITE);//其中INFINITE为等待时间，可以改为一个比较小的时间
	if (WAIT_OBJECT_0 == dwWaitResut)
	{
		//接收到了检测结果，进行显示
		//因为这已经是一个独立线程，所以显示可以不要在一个独立线程里

		//如果希望用独立线程，则参考static UINT ThreadReceiveRecResult(LPVOID pParam)定义函数
		//然后可以利用两种方式调用该函数
		//    1.利用AfxBeginThread函数调用；
		//    2.在OnInitDialog中利用CreateThread建立线程，并利用PostThreadMessage传递消息
		//注意：第一种方式中，该函数返回类型为UINT；第二种方式，函数的返回类型应该为DWORD
		//实际上还有一种方法，就是为窗口定义自定义的消息处理函数，也有两种方式，这里不列举了
		//包括现在定义的两个线程也可以用窗口消息处理函数的方式

	}
	return 0;
}


// 用于从Socket接收检测结果  并且在内存中的缓冲图像帧中画框
DWORD CNokiaFaceRecognitionDemoDlg::ThreadReceiveRecResult(LPVOID pParam)
{
	MSG msg;
	BOOL hasMsg = FALSE;
	while(TRUE)
	{
		WaitMessage();
		hasMsg = PeekMessage(&msg, (HWND)-1, WM_USERRECEIVERECRESULT, WM_USERRECEIVERECRESULT, PM_REMOVE);
		if (!hasMsg)
			continue;
		if (WM_USERRECEIVERECRESULT == msg.message)
		{
			//在这里利用Socket接收检测结果，可以是循环，直到接收到，并且接收完整为止

		}
		else if (WM_QUIT == msg.message)
			return 0;
	}
	return 0;
}

void CNokiaFaceRecognitionDemoDlg::OnBnClickedButton1()
{
	rgbFrame rgbFrm;
	int frameNo = 0;
	WinClient m_Client("10.103.110.250",10086);
	Imagehead imgHd;
	Size dsize = Size(IMAGE_WIDTH,IMAGE_HEIGHT);
	HEAD hd[MAX_HEAD_LEN];
	cvNamedWindow("testInterface", 1);  

	while(1) {
		// 从服务器端获取解码出的rgbFrame（这里用cv::Mat模拟从服务器端得到的数据）
		cv::Mat matFrame = CNokiaFaceRecognitionDemoDlg::RcvRgbFrmFromDecoderServer();
		cv::Mat tmpMat = Mat(dsize,CV_32S);
		cv::resize(matFrame,tmpMat,dsize);
		//将mat数据封装到rgbFrame中		

		unsigned char* tmpRgbData =	CCommonHelper::cvMat2RGB(tmpMat);
		cv::Mat tmpMat2Show = CCommonHelper::RGB2cvMat(tmpRgbData);
		//imshow("testInterface",tmpMat2Show);
		cv::waitKey(0);
		rgbFrm.frameNo = frameNo;
		rgbFrm.frameLen = MAX_LEN;
		rgbFrm.rgbData = tmpRgbData;
		int headIndex = 0;


		// 将从服务器端得到的视频帧传输给检测服务器
		if(m_Client.sendRGBFrame(rgbFrm)) {
			printf("发送成功！视频帧号：%d",frameNo);
			frameNo++;
		}
		else {
			printf("发送失败！视频帧号：%d",frameNo);
			exit(1);
		}

		// 从检测服务器取得检测出来的数据
		char c = cv::waitKey(500);
		if(27 == c) break;
		m_Client.recvImagehead(imgHd);
		m_Client.recvHead(hd);

		// 将检测数据画到视频帧中
		CvFont font;  
		cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.5f, 1.5f, 0, 2, CV_AA);//设置显示的字体
		IplImage imgTmp = tmpMat;
		IplImage *input = cvCloneImage(&imgTmp);
		if(headIndex<imgHd.Head_num)
		{
			CvPoint P1,P2;
			P1.x = hd[headIndex].x;
			P1.y = hd[headIndex].y;
			P2.x = hd[headIndex].x + hd[headIndex].width;
			P2.y = hd[headIndex].y + hd[headIndex].height;
			int fcID = hd[headIndex].facename;
			char* strID = (char*)&fcID;
			//itoa(fcID,strID,10);
			cvRectangle(input,P1 ,P2, CV_RGB(0, 255, 0), 2); 
			cvPutText(input,strID, cvPoint(P1.x, P1.y-10), &font, CV_RGB(255, 0, 0));
		}
		// 显示
		cvShowImage("testInterface",input);  
	}
}


void CNokiaFaceRecognitionDemoDlg::OnBnClickedButton2()
{
	int rgbFrameSize = sizeof(rgbFrame);
	return;
}


void CNokiaFaceRecognitionDemoDlg::OnBnClickedTestintface()
{
	// TODO: 接口测试

	rgbFrame rgbFrm;
	int frameNo = 0;
	WinClient m_Client("10.103.222.250",10086);
	Imagehead imgHd;
	HEAD hd[MAX_HEAD_LEN];
	cvNamedWindow("testInterface", 1);  

	while(1) {
		// 从服务器端获取解码出的rgbFrame（这里用cv::Mat模拟从服务器端得到的数据）
		cv::Mat matFrame = CNokiaFaceRecognitionDemoDlg::RcvRgbFrmFromDecoderServer();

		//将mat数据封装到rgbFrame中	
		unsigned char* tmpRgbData =	CCommonHelper::cvMat2RGB(matFrame);
		rgbFrm.frameNo = frameNo;
		rgbFrm.frameLen = MAX_LEN;
		rgbFrm.rgbData = tmpRgbData;
		int headIndex = 0;


		// 将从服务器端得到的视频帧传输给检测服务器
		//if(m_Client.sendRGBFrame(rgbFrm)) {
		//	printf("发送成功！视频帧号：%d",frameNo);
		//	frameNo++;
		//}
		//else {
		//	printf("发送失败！视频帧号：%d",frameNo);
		//	exit(1);
		//}

		//// 从检测服务器取得检测出来的数据
		//cv::waitKey(500);
		//m_Client.recvImagehead(imgHd);
		//m_Client.recvHead(hd);

		// 将检测数据画到视频帧中
		CvFont font;  
		cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.5f, 1.5f, 0, 2, CV_AA);//设置显示的字体
		IplImage imgTmp = matFrame;
		IplImage *input = cvCloneImage(&imgTmp);
		if(headIndex<imgHd.Head_num)
		{
			CvPoint P1,P2;
			P1.x = hd[headIndex].x;
			P1.y = hd[headIndex].y;
			P2.x = hd[headIndex].x + hd[headIndex].width;
			P2.y = hd[headIndex].y + hd[headIndex].height;
			int fcID = hd[headIndex].facename;
			char* strID ;
			itoa(fcID,strID,10);
			cvRectangle(input,P1 ,P2, CV_RGB(0, 255, 0), 2); 
			cvPutText(input,strID, cvPoint(P1.x, P1.y-10), &font, CV_RGB(255, 0, 0));
		}
		// 显示
		cvShowImage("testInterface",input);  
	}


}
