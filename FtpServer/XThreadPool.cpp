#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"
#include <thread>
#include <iostream>
using namespace std;
#include "testUtil.h"

//分配任务到线程池
void XThreadPool::Dispatch(XTask* task)
{
	testout("main:thread At ThreadPool::Dispatch()");

	if (!task) return;
	//轮询分配任务
	int tid = (lastThread + 1) % threadCount;		//简单的累加得到新的线程ID
	lastThread = tid;
	XThread *t = threads[tid];

	t->AddTask(task);		//添加任务
	t->Activate();		//激活线程
}

//初始化线程池
void XThreadPool::Init(int threadCount)
{
	testout("main:thread At ThreadPool::Init()");

	this->threadCount = threadCount;
	this->lastThread = -1;
	//创建线程
	for (int i = 0; i < threadCount; i++)
	{
		cout << "create thread " << i << endl;
		XThread* t = new XThread();
		t->id = i;
		t->Start();
		threads.push_back(t);
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}