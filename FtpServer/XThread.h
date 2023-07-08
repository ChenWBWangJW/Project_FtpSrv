#pragma once

#include <event2/util.h>
#include<list>
#include<mutex>
class XTask;
struct event_base;
class XThread 
{
public:
	void Start();							//启动线程

	void Main();							//线程入口函数

	void Notify(evutil_socket_t, short);	//线程通知函数,线程分发

	void Activate();						//激活线程

	void AddTask(XTask *);					//添加任务

	bool Setup();							//安装线程，初始化evevnt_base和管道监听事件用于激活

	XThread();								//构造函数

	~XThread();								//析构函数
	int id = 0;								//线程id

private:
	int notify_send_fd = 0;					//线程通知发送管道
	event_base *base = 0;					//libevent事件处理器，为方便管理线程，根据需要实现
	std::list<XTask*> tasks;				//任务列表
	std::mutex tasks_mutex;					//任务锁，在任务链表中添加和删除任务时需要用信号进行互斥
};

