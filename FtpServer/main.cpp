#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;
#include <event2/event.h>
#include <stdlib.h>

#include <signal.h>

#define errmsg(msg) do{cout << msg << endl;exit(1);}while(0)

/********************* basic include ********************/
//字符串处理头文件
#include <string>
#include <string.h>
using std::string;
#include <fstream>

//libevent相关头文件
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <event2/http.h>

//线程池相关头文件
#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"
#include "testUtil.h"
#include "XFtpFactory.h"

#define SPORT 21		//FTP端口号
#define BUFS 1024

#define XThreadPoolGet XThreadPool::Get()

/*********************************************
* @说明：
*		1.创建线程池任务
*		2.将新连接的套接字传递给任务
*		3.将任务分发给线程池
* 
* @目的：
*		把等待连接的套接字传递给线程池，让线程
*		池去处理
* 
* @备注：
*		XFtpFactory是任务类XTask的子类，提供一
*		个创建任务的接口，这样可以在创建任务时，
*		根据不同的任务类型，创建不同的任务类。
* 
*		Dispatch是线程池的分发函数，将任务分发
*		线程池，线程池自动调用任务类的Run函数。
* 
*		task成员变量sock保存已经连接的套接字，
*		之后处理任务则通过这个套接字和客户端进
*		行交互。
**********************************************/
void listen_cb(struct evconnlistener* ev, evutil_socket_t s, struct sockaddr * addr, int socklen, void* arg)
{
	testout("main thread listen_cb");
	sockaddr_in *sin = (sockaddr_in*)addr;

	/*
	* 线程池任务类
	*/
	XTask *task = XFtpFactory::Get()->CreateTask();		//创建线程池任务，指针指向该函式返回的对象
	task->sock = s;										//将新连接的套接字传递给任务
	XThreadPoolGet->Dispatch(task);						//将任务分发给线程池
}

/*********************************************
* @说明：
*		1.初始化线程池
*		2.初始化libevent
*		3.创建监听套接字
*
* @目的：
*		    将监听函数listen_cb与libevent绑定，
*		用户连接时，libevent会通过库函数传递回
*		调函数，在回调函数里处理已经连接的套接
*		字。
**********************************************/
int main() 
{
	/*
	* 0.
	* 避免进程在写入已关闭的管道或套接字时被终止
	* 通过忽略 SIGPIPE 信号，可以在写入失败时手动处理错误，而不是让进程默认终止
	*/
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		errmsg("signal(SIGPIPE, SIG_IGN) error!");
	
	/*
	* 1.
	* 初始化线程池
	*/
	XThreadPoolGet->Init(10);

	/*
	* 2.
	* 初始化libevent，创建一个指向libevent事件循环对象的指针base
	*/
	event_base * base = event_base_new();
	if(!base)
		errmsg("main thread event_base_new failed!");

	/*
	* 3.
	* 初始化地址结构体
	*/
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	/*
	* 4.
	* 通过libevent库函数创建一个TCP监听器（evconnlistener 类型结构体指针ev）
	* 通过evconnlistener_new_bind函数进行listen_cb回调函数的注册
	*/
	evconnlistener* ev = evconnlistener_new_bind (
		base,											//libevent事件循环对象指针
		listen_cb,										//回调函数
		base,											//回调函数参数
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,		//标志位
		10,												//连接队列大小，对应listen函数的backlog参数
		(sockaddr*)&sin,								//监听地址结构体指针
		sizeof(sin));									//监听地址结构体大小

	

	if (base)		//base指针不为空，进入代码块，开始监听
	{
		cout << "Begin to listen..." << endl;
		/*
		* 将指向循环对象的指针传递给libevent，启动事件循环
		* 监听evconnlistener_new_bind所注册的事件和所绑定的回调函数的调用
		*/
		event_base_dispatch(base);
	}

	//释放资源
	if (ev)
	{
		evconnlistener_free(ev);
	}
	if (base)
	{
		event_base_free(base);
	}
	testout("server end!");
	return 0;
}