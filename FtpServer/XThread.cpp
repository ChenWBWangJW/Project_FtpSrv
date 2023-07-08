#include <thread>
#include <iostream>
using namespace std;
#include <unistd.h>

#include <event2/event.h>

#include "testUtil.h"
#include "XThread.h"
#include "XTask.h"

/*
 *  �̼߳������̶߳��Լ���֪ͨ
 *	�����߳���ǰ���̷߳���һ��֪ͨʱ
 *	���̻߳�ͨ�� libevent ���¼�ѭ�����Ƽ�������֪ͨ����ִ����Ӧ�Ĵ����߼�
 *	����fd�ǹܵ��Ķ��ˣ�which���¼�����
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
* �����̣߳�����һ��ִ��������Ϊ���ܻ�û��������
*/
void XThread::Start()
{
	testout(id << "thread At Start()");
	if(Setup())
		cout << "Thread ID: " << id << " Setup success!" << endl;
	thread th(&XThread::Main, this);		//�߳�һ�����������Ϳ�ʼִ�У�
	th.detach();							//�̷߳��룬����Ҫ�ȴ��߳̽�����ͬ����̶߳�������
}

/*
* �߳�����ʱ�������飬���߳�ִ�еĴ����
*/
void XThread::Main()
{
	cout << "Thread ID: " << id << " At Main() begin" << endl;
	//������base��һ���Ѿ���setup������ͨ�����ýṹ���ʼ����libevent
	event_base_dispatch(base);									//ִ�е���δ������м������ȴ����߳�֪ͨ
	event_base_free(base);
	cout << "Thread ID: " << id << " At Main() end" << endl;
}

bool XThread::Setup()
{
	testout (id << "thread At Setup()");

	//�ܵ�ͨ����0����1д
	int fds[2];							

	if (pipe(fds))
	{
		cerr << "pipe failed!" << endl;
		return false;
	}

	//�������߳�֪ͨ���̴߳�������
	notify_send_fd = fds[1];		//д��

	/**************************************
	* @Ŀ�ģ�
	*	����lievent�����ģ���ͨ�����ýṹ����´����¼�����������
	*
	* @������
	*	event_config_new()��
	*		����һ���µ��¼����ö��󣬼� event_config �ṹ���ʵ��
	*		�¼����ö������������¼��������Ժ�ѡ��
	*
	*	event_config_set_flag()��
	*		�����¼����ö�������Ժ�ѡ�����EVENT_BASE_FLAG_NOLOCK��ʾ����Ҫ��
	*		�����־����������ܣ�����Ҫȷ���¼��������ڶ���߳���ͬʱʹ��
	*
	*	event_base_new_with_config()��
	*		����һ���µ��¼���������������������һ���µ��¼���
	*		������������ö������������ʼ���¼�����������һ��ָ���´������¼�����ָ��
	*
	*	event_config_free():
			�ͷ��¼����ö���ṹ����ڴ�
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
	* @Ŀ�ģ�
	*	����һ���µ��¼����󣬼� event �ṹ���ʵ��
	*	��ӹܵ������¼��������̳߳ؼ����Լ�ִ������, ��this����
	*
	* @������
	*	event_new(struct event_base *base, evutil_socket_t fd, short events, event_callback_fn callback, void *arg)��
	*		- base���¼�����ע�ᵽ���¼���
	*		- fd��Ҫ���ӵ��ļ�������
	*		- events���¼������ͣ�����������ֵ�����
	*			EV_TIMEOUT����ʱ�¼�
	*			EV_READ���ɶ��¼�
	*			EV_WRITE����д�¼�
	*			EV_PERSIST���־��¼������¼�һֱ���֣����ᱻ�Զ�ɾ��
	*			EV_ET����Ե�����¼�����ֻ�е�״̬�ı�ʱ�Ŵ����¼�
	*			EV_FINALIZE���¼��ѱ��ͷţ����ٱ�ʹ��
	*			...
	*		- callback���¼�����ʱ���õĺ���
	*		- arg�����ݸ��ص������Ĳ���
	*
	*	@����ֵ��
	*		�ɹ�����ָ���´������¼������ָ�룬ʧ�ܷ���NULL
	*
	*
	*	event_add(struct event *ev, const struct timeval *tv)��
	*		- ev��Ҫ��ӵ��¼�
	*		- tv���¼���ʱʱ�䣬���ΪNULL�����¼����ᳬʱ
	*
	* @˵����
	*	ʹ�� event_new() �����¼�����󣬿��Խ�����ӵ��¼����У�������һ��ָ���¼�����ָ��
	*	ͨ������ event_add() �������¼���ӵ��¼�ѭ���У��Ա����¼�����ʱ������Ӧ�Ļص�����
	*
	*	�¼�����Ĵ�����Ϊ�˹����ļ����������¼�����
	*	��ָ����Ӧ�Ļص��������û�����
	*	ͨ���¼�����libevent ���Թ���ʹ����¼���ʵ���¼������ı��ģ��
	***************************************/
	event *ev = event_new(base, fds[0], EV_READ | EV_PERSIST, Notify_cb, this);
	event_add(ev, 0);

	return true;
}



void XThread::Activate()
{
	testout("Thread ID: " << id << " At Activate");

	int re = write(notify_send_fd, "c", 1);						//��ܵ�д��һ���ֽڣ������߳�
	if (re <= 0)
	{
		cerr << "XThread::Activate() write failed!" << endl;
	}
	//�������б��л�ȡ���񲢳�ʼ��
	XTask *t = NULL;
	tasks_mutex.lock();
	if (tasks.empty())
	{
		tasks_mutex.unlock();
		return;
	}
	t = tasks.front();		//��ȡ����
	tasks.pop_front();		//��������
	tasks_mutex.unlock();
	t->Init();			//��ʼ�����񣬼���XFtpServerCMD����󣬵���CMD��ʵ���������ĳ�Ա����
}

//�������
void XThread::AddTask(XTask* t)			//�ú�������XThread�࣬����ʵ���������Ϊ���̶߳����������
{
	if (!t) return;

	/*
	* �ú��������ݶ���ΪXFtpServerCMD����󣬸���̳���XTask��
	* thisָ��ǰ��XThread�����Ҳ���Ǵ��̳߳���ȡ�����߳�
	*/
	t->base = this->base;	// XFtpServerCMD�����̳��̵߳�libevent�����ģ���������������������ע���¼���ʹ�̴߳���������ӵ��¼�
	tasks_mutex.lock();		//�����������������
	tasks.push_back(t);		//�������
	tasks_mutex.unlock();	//����
}
XThread::XThread()
{
}

XThread::~XThread()
{
}