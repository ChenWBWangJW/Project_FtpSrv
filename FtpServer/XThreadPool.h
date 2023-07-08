#pragma once
#include<vector>

class XThread;
class XTask;
class XThreadPool{
public:
	/* 单例模式是一种创建型设计模式
	它保证一个类只有一个实例对象
	并提供一个全局访问点来访问这个实例对象
	单例模式通常用于管理全局资源
	例如日志、配置文件、数据库连接等*/
	static XThreadPool *Get()
	{
		static XThreadPool tp;
		return &tp;
	}
	void Init(int threadCount);		//初始化所有线程

	//分发任务给线程
	void Dispatch(XTask* );	//arg是任务所带的参数，可以自己重新实现，弄成更复杂的形式

private:
	int threadCount;			//线程数量
	int lastThread = -1;			//最后一个线程的id
	std::vector<XThread *> threads;	//线程列表,用vector容器来管理线程
	XThreadPool() {};
};