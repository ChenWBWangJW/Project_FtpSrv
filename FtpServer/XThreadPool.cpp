#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"
#include <thread>
#include <iostream>
using namespace std;
#include "testUtil.h"

//���������̳߳�
void XThreadPool::Dispatch(XTask* task)
{
	testout("main:thread At ThreadPool::Dispatch()");

	if (!task) return;
	//��ѯ��������
	int tid = (lastThread + 1) % threadCount;		//�򵥵��ۼӵõ��µ��߳�ID
	lastThread = tid;
	XThread *t = threads[tid];

	t->AddTask(task);		//�������
	t->Activate();		//�����߳�
}

//��ʼ���̳߳�
void XThreadPool::Init(int threadCount)
{
	testout("main:thread At ThreadPool::Init()");

	this->threadCount = threadCount;
	this->lastThread = -1;
	//�����߳�
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