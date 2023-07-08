#include <string.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/util.h>

#include <string>
using namespace std;

#include "XFtpServerCMD.h"
#include "testUtil.h"

#define BUFS 4096

void XFtpServerCMD::Reg(std::string cmd, XFtpTask* call)
{
	testout("At XftpServerCMD::Reg");
	if (!call)
	{
		cout << "XFtpServerCMD::Reg call is null" << endl;
		return;
	}

	if (cmd.empty())
	{
		cout << "XFtpServerCMD::Reg cmd is empty" << endl;
		return;
	}

	if (calls.find(cmd) != calls.end())
	{
		cout << cmd << "is already registed!" << endl;
		return;
	}

	testout(cmd << "Reg success");
	call->base = base;			//将当前线程的libevent传递给命令对象
	call->cmdTask = this;		//将当前命令对象传递给命令对象
	calls[cmd] = call;			//将命令对象存入map
	calls_del[call] = 0;		//将命令对象存入map
}

void XFtpServerCMD::Event(bufferevent* bev, short events)
{
	testout("At XFtpServerCMD::Event");
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		delete this;
	}
}

void XFtpServerCMD::Read(bufferevent* bev)
{
	cout << endl;
	testout("At XFtpServerCMD::Read");
	char buf[BUFS] = { 0 };
	while (1) 
	{
		int len = bufferevent_read(bev, buf, BUFS);
		if (len <= 0) break;
		cout << "Recv CMD(" << len << "): " << buf;
		//分发到处理对象
		//分析出类型
		string type = "";
		for (int i = 0; i < len; i++)
		{
			if (buf[i] == ' ' || buf[i] == '\r')
			{
				break;
			}
			type += buf[i];
		}

		//解析命令类型并调用命令对象进行解析
		cout << "type is [" << type << "]" << endl;
		if (calls.find(type) != calls.end())	//映射后若是有进行注册的命令类型，进入代码块
		{
			testout("begin to parse");
			XFtpTask *t = calls[type];			//指针指向该命令在map中映射的对象
			t->Parse(type, buf);				//调用该对象的解析函数Prase()
			testout("curDir: [" << curDir << "]");
		}
		else
		{
			cout << "parse object not found" << endl;
			ResCMD("200 OK\r\n");
		}
	}
}

bool XFtpServerCMD::Init()
{
	testout("At XFtpServerCMD::Init()");

	//在线程的base里添加一个缓冲区对sock的缓冲事件，这就是命令通道
	bufferevent* bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		delete this;
		return false;
	}

	//添加超时
	timeval t = { 300, 0 };
	bufferevent_set_timeouts(bev, &t, 0);

	string msg = "220 Welcome to libevent XFtpServer\r\n";
	bufferevent_write(bev, msg.c_str(), msg.size());

	this->cmdTask = this;
	this->bev = bev;

	//注册本对象实现的回调函数
	Setcb(bev);

	return true;
}

XFtpServerCMD::XFtpServerCMD() {}

XFtpServerCMD::~XFtpServerCMD() 
{
	ClosePORT();
	for (auto i : calls_del)
	{
		delete i.first;
	}
}