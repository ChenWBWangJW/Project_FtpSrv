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
//�ַ�������ͷ�ļ�
#include <string>
#include <string.h>
using std::string;
#include <fstream>

//libevent���ͷ�ļ�
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <event2/http.h>

//�̳߳����ͷ�ļ�
#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"
#include "testUtil.h"
#include "XFtpFactory.h"

#define SPORT 21		//FTP�˿ں�
#define BUFS 1024

#define XThreadPoolGet XThreadPool::Get()

/*********************************************
* @˵����
*		1.�����̳߳�����
*		2.�������ӵ��׽��ִ��ݸ�����
*		3.������ַ����̳߳�
* 
* @Ŀ�ģ�
*		�ѵȴ����ӵ��׽��ִ��ݸ��̳߳أ����߳�
*		��ȥ����
* 
* @��ע��
*		XFtpFactory��������XTask�����࣬�ṩһ
*		����������Ľӿڣ����������ڴ�������ʱ��
*		���ݲ�ͬ���������ͣ�������ͬ�������ࡣ
* 
*		Dispatch���̳߳صķַ�������������ַ�
*		�̳߳أ��̳߳��Զ������������Run������
* 
*		task��Ա����sock�����Ѿ����ӵ��׽��֣�
*		֮����������ͨ������׽��ֺͿͻ��˽�
*		�н�����
**********************************************/
void listen_cb(struct evconnlistener* ev, evutil_socket_t s, struct sockaddr * addr, int socklen, void* arg)
{
	testout("main thread listen_cb");
	sockaddr_in *sin = (sockaddr_in*)addr;

	/*
	* �̳߳�������
	*/
	XTask *task = XFtpFactory::Get()->CreateTask();		//�����̳߳�����ָ��ָ��ú�ʽ���صĶ���
	task->sock = s;										//�������ӵ��׽��ִ��ݸ�����
	XThreadPoolGet->Dispatch(task);						//������ַ����̳߳�
}

/*********************************************
* @˵����
*		1.��ʼ���̳߳�
*		2.��ʼ��libevent
*		3.���������׽���
*
* @Ŀ�ģ�
*		    ����������listen_cb��libevent�󶨣�
*		�û�����ʱ��libevent��ͨ���⺯�����ݻ�
*		���������ڻص������ﴦ���Ѿ����ӵ��׽�
*		�֡�
**********************************************/
int main() 
{
	/*
	* 0.
	* ���������д���ѹرյĹܵ����׽���ʱ����ֹ
	* ͨ������ SIGPIPE �źţ�������д��ʧ��ʱ�ֶ�������󣬶������ý���Ĭ����ֹ
	*/
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		errmsg("signal(SIGPIPE, SIG_IGN) error!");
	
	/*
	* 1.
	* ��ʼ���̳߳�
	*/
	XThreadPoolGet->Init(10);

	/*
	* 2.
	* ��ʼ��libevent������һ��ָ��libevent�¼�ѭ�������ָ��base
	*/
	event_base * base = event_base_new();
	if(!base)
		errmsg("main thread event_base_new failed!");

	/*
	* 3.
	* ��ʼ����ַ�ṹ��
	*/
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	/*
	* 4.
	* ͨ��libevent�⺯������һ��TCP��������evconnlistener ���ͽṹ��ָ��ev��
	* ͨ��evconnlistener_new_bind��������listen_cb�ص�������ע��
	*/
	evconnlistener* ev = evconnlistener_new_bind (
		base,											//libevent�¼�ѭ������ָ��
		listen_cb,										//�ص�����
		base,											//�ص���������
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,		//��־λ
		10,												//���Ӷ��д�С����Ӧlisten������backlog����
		(sockaddr*)&sin,								//������ַ�ṹ��ָ��
		sizeof(sin));									//������ַ�ṹ���С

	

	if (base)		//baseָ�벻Ϊ�գ��������飬��ʼ����
	{
		cout << "Begin to listen..." << endl;
		/*
		* ��ָ��ѭ�������ָ�봫�ݸ�libevent�������¼�ѭ��
		* ����evconnlistener_new_bind��ע����¼������󶨵Ļص������ĵ���
		*/
		event_base_dispatch(base);
	}

	//�ͷ���Դ
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