#pragma once

#ifndef SOCKET_H
#define SOCKET_H
#include "Constants.h"

#endif

class WinServer{
public:
	WinServer(char* lisIp, int lisPort){
		this->lisIp = lisIp;
		this->lisPort = lisPort;
		init();
	}
	void lisConn(){
		//Listen监听端
		listen(sockServer,5);//5为等待连接数目
		printf("Server监听链接...\n");
		int len=sizeof(SOCKADDR);
		//会阻塞进程，直到有客户端连接上来为止
		sockClient=accept(sockServer,(SOCKADDR*)&addrClient,&len);
		printf("Server连接已建立\n");
	}
	int sendMsg(char* filename){
		FILE *fp = fopen(filename, "rb");		//以二进制方式打开文件
		if(fp == NULL){
			printf("Cannot open file, press any key to exit!\n");
			system("pause");
			exit(0);
		}
		fseek(fp, 0L, SEEK_END );		//发送4字节的文件长度
		int length = ftell(fp);
		lenBuf[0]  = length >> 24;     
		lenBuf[1] = length >> 16;
		lenBuf[2] = length >> 8;
		lenBuf[3] = length;
		int sendRet = send(sockClient,lenBuf,LEN_BUF_SIZE,0);
		memset(lenBuf,0,LEN_BUF_SIZE);

		fseek(fp,0L,SEEK_SET);			//循环发送文件
		int nCount;
		while( (nCount = fread(sendBuf, 1, BUF_SIZE, fp)) > 0 ){
			send(sockClient, sendBuf, nCount, 0);
		}
		printf("发送完毕\n");
		memset(sendBuf,0,BUF_SIZE);	

		return 1;
	}

	int recvMsg(char* filename){
		FILE *fp = fopen(filename, "wb");		//以二进制方式打开文件
		if(fp == NULL){
			printf("Cannot open file, press any key to exit!\n");
			system("pause");
			exit(0);
		}

		int recvHeaderSize = recv(sockClient,lenBuf,4,0);		//接收文件4字节长度
		int fileSize = ((unsigned char)lenBuf[0]<<24)|((unsigned char)lenBuf[1]<<16)|((unsigned char)lenBuf[2]<<8)|(unsigned char)lenBuf[3];
		memset(lenBuf,0,LEN_BUF_SIZE);

		int ackCount=0;
		int nCount;		//循环接收文件
		while( (nCount = recv(sockClient, recvBuf, BUF_SIZE, 0)) > 0 ){
			fwrite(recvBuf, nCount, 1, fp);
			ackCount+=nCount;
			if(ackCount==fileSize){
				break;
			}
		}
		printf("接收完毕\n");

		memset(recvBuf,0,BUF_SIZE);

		return 1;
	}

	void release(){
		//关闭socket
		closesocket(sockClient);
		closesocket(sockServer);
		WSACleanup();
	}

private:
	void init(){
		memset(sendBuf,0,BUF_SIZE);
		memset(recvBuf,0,BUF_SIZE);
		memset(lenBuf,0,LEN_BUF_SIZE);
		WSAStartup(MAKEWORD(2,2),&wsaData);
		sockServer=socket(AF_INET,SOCK_STREAM,0);
		addrServer.sin_addr.S_un.S_addr=inet_addr(lisIp);
		addrServer.sin_family=AF_INET;
		addrServer.sin_port=htons(lisPort);
		bind(sockServer,(SOCKADDR*)&addrServer,sizeof(SOCKADDR));

		lisConn();
	}


	char* lisIp;
	int lisPort;

	WSADATA wsaData;
	SOCKET sockServer;
	SOCKADDR_IN addrServer;
	SOCKET sockClient;
	SOCKADDR_IN addrClient;
	char sendBuf[BUF_SIZE];
	char recvBuf[BUF_SIZE];
	char lenBuf[LEN_BUF_SIZE];
};

class WinClient{
public:
	WinClient(char* ipOp, int portOp){
		this->ipOp = ipOp;
		this->portOp = portOp;
		init();
	}
	void estConn(){
		//连接到服务端
		printf("Client连接...\n");
		if(connect(sockClient,(SOCKADDR*)&addrServer,sizeof(SOCKADDR))<0){
			printf("Client建立连接失败\n");
			exit(1);
		}
		printf("Client连接已建立\n");
	}
	int sendRGBFrame (rgbFrame& frame){
		//int sendCount=0;
		int sendRet;
		int2byte(frame.frameNo);		//发送结构体
		sendRet = send(sockClient,sendLenBuf,LEN_BUF_SIZE,0);
		//sendCount+=sendRet;
		int2byte(frame.frameLen);
		sendRet = send(sockClient,sendLenBuf,LEN_BUF_SIZE,0);
		//sendCount+=sendRet;
		int fullTime = MAX_LEN/BUF_SIZE;
		int restSize = MAX_LEN-fullTime*BUF_SIZE;
		int i;
		for(i=0;i<fullTime;i++){
			memcpy(sendBuf, frame.rgbData+i*BUF_SIZE, BUF_SIZE);
			sendRet = send(sockClient, sendBuf, BUF_SIZE, 0);
			//sendCount+=sendRet;
		}
		memcpy(sendBuf,frame.rgbData+i*BUF_SIZE,restSize);
		sendRet = send(sockClient, sendBuf, restSize, 0);
		//sendCount+=sendRet;

		printf("发送完一个rgbFrame\n");
		return 1;
	}

	int recvImagehead(Imagehead& imagehead){
		int ackCount=0;
		int nCount=0;
		if(overflowCount>=RECV_IMAGEHEAD_SIZE){
			memcpy(recvImageheadByte, recvOverflowBuf, RECV_IMAGEHEAD_SIZE);
			memcpy(swapBuf, recvOverflowBuf+RECV_IMAGEHEAD_SIZE, overflowCount-RECV_IMAGEHEAD_SIZE);
			overflowCount-=RECV_IMAGEHEAD_SIZE;
			memcpy(recvOverflowBuf, swapBuf, overflowCount);

			memcpy(recvLenBuf, recvImageheadByte, 4);
			imagehead.Frame_No=byte2int();
			memcpy(recvLenBuf, recvImageheadByte+4, 4);
			imagehead.Head_num = byte2int();
			headNum = imagehead.Head_num;
			return 1;
		}
		if(overflowCount>0 && overflowCount<RECV_IMAGEHEAD_SIZE){
			memcpy(recvImageheadByte, recvOverflowBuf, overflowCount);
			ackCount+=overflowCount;
		}
		while( (nCount = recv(sockClient, recvBuf, BUF_SIZE, 0)) > 0){
			ackCount+=nCount;
			if(ackCount>=RECV_IMAGEHEAD_SIZE){
				overflowCount=ackCount-RECV_IMAGEHEAD_SIZE;
				int tailCount = nCount-overflowCount;
				memcpy(recvImageheadByte+ackCount-nCount, recvBuf, tailCount);
				memcpy(recvOverflowBuf, recvBuf+tailCount, overflowCount);
				break;
			}else{
				memcpy(recvImageheadByte+ackCount-nCount, recvBuf, nCount);
			}
		}
		if(nCount==0){
			return -1;
		}
		memcpy(recvLenBuf, recvImageheadByte, 4);
		imagehead.Frame_No=byte2int();
		memcpy(recvLenBuf, recvImageheadByte+4, 4);
		imagehead.Head_num = byte2int();
		headNum = imagehead.Head_num;

		return 1;
	}
	int recvHead(HEAD* heads){
		
		int ackCount=0;
		int nCount=0;
		if(overflowCount>0){
			memcpy(recvHeadByte, recvOverflowBuf, overflowCount);
			ackCount+=overflowCount;
		}
		while((nCount=recv(sockClient, recvBuf, BUF_SIZE, 0))>0){
			ackCount+=nCount;
			if(ackCount>=headNum*HEAD_SIZE){
				overflowCount=ackCount-headNum*HEAD_SIZE;
				int tailCount = nCount-overflowCount;
				memcpy(recvHeadByte+ackCount-nCount, recvBuf, tailCount);
				memcpy(recvOverflowBuf, recvBuf+tailCount, overflowCount);
				break;
			}else{
				memcpy(recvHeadByte+ackCount-nCount,recvBuf, nCount);
			}
		}
		if(nCount==0){
			return -1;
		}
		for(int i=0;i<headNum;i++){
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE,4);
			heads[i].Frame_No = byte2int();
			memcpy(&(heads[i].HeadIndex), recvHeadByte+i*HEAD_SIZE+4,1);
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE+5,4);
			heads[i].x = byte2int();
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE+9,4);
			heads[i].y = byte2int();
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE+13,4);
			heads[i].width = byte2int();
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE+17,4);
			heads[i].height = byte2int();
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE+21,4);
			heads[i].facename = byte2int();
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE+25,4);
			heads[i].Pre_Frame_No = byte2int();
			memcpy(recvLenBuf, recvHeadByte+i*HEAD_SIZE+29,4);
			heads[i].PrevHeadIndex = byte2int();
			memcpy(heads[i].Face, recvHeadByte+i*HEAD_SIZE+33, FACE_SIZE);
		}
		return 1;
	}

	void release(){
		//关闭socket
		closesocket(sockClient);
		WSACleanup();

		free(sendBuf);
		free(recvBuf);
		free(sendLenBuf);
		free(recvLenBuf);
		free(recvOverflowBuf);
		free(recvImageheadByte);
		free(recvHeadByte);
	}
private:
	void init(){
		overflowCount=0;
		sendBuf = (char*)malloc(BUF_SIZE);
		recvBuf = (char*)malloc(BUF_SIZE);
		sendLenBuf = (char*)malloc(LEN_BUF_SIZE);
		recvLenBuf = (char*)malloc(LEN_BUF_SIZE);;
		recvOverflowBuf = (char*)malloc(BUF_SIZE);
		recvImageheadByte = (char*)malloc(RECV_IMAGEHEAD_SIZE);
		recvHeadByte = (char*)malloc(MAX_RECV_HEAD_SIZE);
		swapBuf = (char*)malloc(BUF_SIZE);
		/*
		memset(recvOverflowBuf,0,BUF_SIZE);
		memset(sendBuf,0,BUF_SIZE);
		memset(recvBuf,0,BUF_SIZE);
		memset(sendLenBuf,0,LEN_BUF_SIZE);
		memset(recvLenBuf,0,LEN_BUF_SIZE);
		memset(recvImageheadByte,0,RECV_IMAGEHEAD_SIZE);
		memset(recvHeadByte,0,MAX_RECV_HEAD_SIZE);
		memset(swapBuf,0,BUF_SIZE);
		*/
		WSAStartup(MAKEWORD(2,2),&wsaData);
		//新建客户端socket
		sockClient=socket(AF_INET,SOCK_STREAM,0);
		//定义要连接的服务端地址 
		addrServer.sin_addr.S_un.S_addr=inet_addr(ipOp);//目标IP
		addrServer.sin_family=AF_INET;
		addrServer.sin_port=htons(portOp);//连接端口

		estConn();
	}

	void int2byte(int length){
		sendLenBuf[0]  = length >> 24;     
		sendLenBuf[1] = length >> 16;
		sendLenBuf[2] = length >> 8;
		sendLenBuf[3] = length;
	}
	int byte2int(){
		return ((unsigned char)recvLenBuf[0]<<24)|((unsigned char)recvLenBuf[1]<<16)|((unsigned char)recvLenBuf[2]<<8)|(unsigned char)recvLenBuf[3];
	}

	int headNum;
	char* ipOp;
	int portOp;

	WSADATA wsaData;
	SOCKET sockClient;
	SOCKADDR_IN addrServer;
	int overflowCount;
	char* sendBuf;
	char* recvBuf;
	char* sendLenBuf;
	char* recvLenBuf;
	char* recvOverflowBuf;
	char* recvImageheadByte;
	char* recvHeadByte;
	char* swapBuf;

/*
	char sendBuf[BUF_SIZE];
	char recvBuf[BUF_SIZE];
	char sendLenBuf[LEN_BUF_SIZE];
	char recvLenBuf[LEN_BUF_SIZE];	
	char recvOverflowBuf[BUF_SIZE];
	char recvImageheadByte[RECV_IMAGEHEAD_SIZE];
	char recvHeadByte[MAX_RECV_HEAD_SIZE];
	char swapBuf[BUF_SIZE];
*/
};
