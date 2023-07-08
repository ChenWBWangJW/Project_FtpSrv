#include "XFtpRETR.h"
#include "testUtil.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <iostream>
#include <string>
using namespace std;

void XFtpRETR::Parse(string type, string msg)
{
	testout("At XFtpRETR::Parse()");
	
	//定位到文件名
	int pos = msg.rfind(" ") + 1;
	
	//拼接文件路径,substr()函数的第一个参数是起始位置，第二个参数是截取的长度
	string filename = msg.substr(pos, msg.size() - pos - 2);
	string path = cmdTask->rootDir + cmdTask->curDir + filename;
	testout("file path:[" << path << "]");

	//打开文件，'rb'表示以二进制方式打开"
	fp = fopen(path.c_str(), "rb");
	if (fp)
	{
		ConnectoPORT();			//主动连接客户端并连接数据通道
		ResCMD("150 File OK");
		bufferevent_trigger(bev, EV_WRITE, 0);		//触发写事件
	}
	else
	{
			ResCMD("451 File open failed");
	}
}

void XFtpRETR::Write(bufferevent* bev)
{
	testout("At XFtpRETR::Write()");
	if (!fp)
	{
		return;
	}
	int len = fread(buf, 1, sizeof(buf), fp);			//读取fp指向文件内容以1字节单位，写入buf中，最多写入sizeof(buf)个字节
	if (len <= 0)
	{
		ResCMD("226 Transfer complete");
		ClosePORT();
		return;
	}
	cout << buf;
	Send(buf, sizeof(buf));
}

void XFtpRETR::Event(bufferevent* bev, short events)
{
	testout("At XFtpRETR::Event()");
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		ClosePORT();
	}
	else if (events & BEV_EVENT_CONNECTED)
	{
		cout << "XFtpRETR BEV_EVENT_CONNECTED" << endl;
	}
}