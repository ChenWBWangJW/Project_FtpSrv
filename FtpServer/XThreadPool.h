#pragma once
#include<vector>

class XThread;
class XTask;
class XThreadPool{
public:
	/* ����ģʽ��һ�ִ��������ģʽ
	����֤һ����ֻ��һ��ʵ������
	���ṩһ��ȫ�ַ��ʵ����������ʵ������
	����ģʽͨ�����ڹ���ȫ����Դ
	������־�������ļ������ݿ����ӵ�*/
	static XThreadPool *Get()
	{
		static XThreadPool tp;
		return &tp;
	}
	void Init(int threadCount);		//��ʼ�������߳�

	//�ַ�������߳�
	void Dispatch(XTask* );	//arg�����������Ĳ����������Լ�����ʵ�֣�Ū�ɸ����ӵ���ʽ

private:
	int threadCount;			//�߳�����
	int lastThread = -1;			//���һ���̵߳�id
	std::vector<XThread *> threads;	//�߳��б�,��vector�����������߳�
	XThreadPool() {};
};