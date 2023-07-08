#include <thread>
#include <iostream>
using namespace std;
#include <unistd.h>

#include <event2/event.h>

#include "testUtil.h"
#include "XThread.h"
#include "XTask.h"

/*
 *  线程监听主线程对自己的通知
 *	当主线程向当前子线程发送一个通知时
 *	子线程会通过 libevent 的事件循环机制监听到该通知，并执行相应的处理逻辑
 *	其中fd是管道的读端，which是事件类型
 */
static void Notify_cb(evutil_socket_t fd, short which, void* arg)
{
	XThread *t = (XThread*)arg;
	t->Notify(fd, which);
}

void XThread::Notify(evutil_socket_t fd, short which)
{
	testout ("Thread ID: "<< id << " At Notify()");
	char buf[2] = { 0 };
	int len = read(fd, buf, 1);
	if (len <= 0) return;
	cout << "Thread ID:" << id << "recv: " << buf << endl;
}

/*
* 启动线程，但不一定执行任务，因为可能还没出现任务
*/
void XThread::Start()
{
	testout(id << "thread At Start()");
	if(Setup())
		cout << "Thread ID: " << id << " Setup success!" << endl;
	thread th(&XThread::Main, this);		//线程一旦被创建，就开始执行；
	th.detach();							//线程分离，不需要等待线程结束，同意该线程独立运行
}

/*
* 线程启动时做的事情，即线程执行的代码块
*/
void XThread::Main()
{
	cout << "Thread ID: " << id << " At Main() begin" << endl;
	//所传递base是一个已经在setup函数中通过配置结构体初始化的libevent
	event_base_dispatch(base);									//执行到这段代码后进行监听，等待主线程通知
	event_base_free(base);
	cout << "Thread ID: " << id << " At Main() end" << endl;
}

bool XThread::Setup()
{
	testout (id << "thread At Setup()");

	//管道通道，0读，1写
	int fds[2];							

	if (pipe(fds))
	{
		cerr << "pipe failed!" << endl;
		return false;
	}

	//用于主线程通知子线程处理任务
	notify_send_fd = fds[1];		//写端

	/**************************************
	* @目的：
	*	创建lievent上下文，即通过配置结构体对新创建事件基进行配置
	*
	* @函数：
	*	event_config_new()：
	*		创建一个新的事件配置对象，即 event_config 结构体的实例
	*		事件配置对象用于设置事件基的特性和选项
	*
	*	event_config_set_flag()：
	*		设置事件配置对象的特性和选项，其中EVENT_BASE_FLAG_NOLOCK表示不需要锁
	*		这个标志可以提高性能，但需要确保事件基不会在多个线程中同时使用
	*
	*	event_base_new_with_config()：
	*		创建一个新的事件基，即由上述配置设置一个新的事件基
	*		函数会根据配置对象的设置来初始化事件基，并返回一个指向新创建的事件基的指针
	*
	*	event_config_free():
			释放事件配置对象结构体的内存
	***************************************/
	event_config * ev_conf = event_config_new();
	event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
	this->base = event_base_new_with_config(ev_conf);
	event_config_free(ev_conf);
	if (!base) 
	{
		cout << "event_base_new_with_config failed!" << endl;
		return false;
	}

	/**************************************
	* @目的：
	*	创建一个新的事件对象，即 event 结构体的实例
	*	添加管道监听事件，用于线程池激活自己执行任务, 将this传入
	*
	* @函数：
	*	event_new(struct event_base *base, evutil_socket_t fd, short events, event_callback_fn callback, void *arg)：
	*		- base：事件将被注册到的事件基
	*		- fd：要监视的文件描述符
	*		- events：事件的类型，可以是以下值的组合
	*			EV_TIMEOUT：超时事件
	*			EV_READ：可读事件
	*			EV_WRITE：可写事件
	*			EV_PERSIST：持久事件，即事件一直保持，不会被自动删除
	*			EV_ET：边缘触发事件，即只有当状态改变时才触发事件
	*			EV_FINALIZE：事件已被释放，不再被使用
	*			...
	*		- callback：事件触发时调用的函数
	*		- arg：传递给回调函数的参数
	*
	*	@返回值：
	*		成功返回指向新创建的事件对象的指针，失败返回NULL
	*
	*
	*	event_add(struct event *ev, const struct timeval *tv)：
	*		- ev：要添加的事件
	*		- tv：事件超时时间，如果为NULL，则事件不会超时
	*
	* @说明：
	*	使用 event_new() 创建事件对象后，可以将其添加到事件基中，并返回一个指向事件基的指针
	*	通过调用 event_add() 函数将事件添加到事件循环中，以便在事件发生时触发相应的回调函数
	*
	*	事件对象的创建是为了关联文件描述符和事件类型
	*	并指定对应的回调函数和用户数据
	*	通过事件对象，libevent 可以管理和处理事件，实现事件驱动的编程模型
	***************************************/
	event *ev = event_new(base, fds[0], EV_READ | EV_PERSIST, Notify_cb, this);
	event_add(ev, 0);

	return true;
}



void XThread::Activate()
{
	testout("Thread ID: " << id << " At Activate");

	int re = write(notify_send_fd, "c", 1);						//向管道写入一个字节，激活线程
	if (re <= 0)
	{
		cerr << "XThread::Activate() write failed!" << endl;
	}
	//从任务列表中获取任务并初始化
	XTask *t = NULL;
	tasks_mutex.lock();
	if (tasks.empty())
	{
		tasks_mutex.unlock();
		return;
	}
	t = tasks.front();		//获取任务
	tasks.pop_front();		//弹出任务
	tasks_mutex.unlock();
	t->Init();			//初始化任务，激活XFtpServerCMD类对象，调用CMD类实例化后对象的成员函数
}

//添加任务
void XThread::AddTask(XTask* t)			//该函数属于XThread类，用于实例化对象后为该线程对象添加任务
{
	if (!t) return;

	/*
	* 该函数所传递对象为XFtpServerCMD类对象，该类继承自XTask类
	* this指当前的XThread类对象，也就是从线程池中取出的线程
	*/
	t->base = this->base;	// XFtpServerCMD类对象继承线程的libevent上下文，命令处理任务在这个上下文注册事件，使线程处理任务添加的事件
	tasks_mutex.lock();		//互斥条件下添加任务
	tasks.push_back(t);		//添加任务
	tasks_mutex.unlock();	//解锁
}
XThread::XThread()
{
}

XThread::~XThread()
{
}