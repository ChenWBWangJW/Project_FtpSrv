#pragma once

class XTask
{
public:
	//一个人客户端一个base
	struct event_base* base = 0;

	//连接的套接字
	int sock = 0;

	//线程池ID
	int thread_id = 0;		

	//初始化任务
	virtual bool Init() = 0;
};