#pragma once

class XTask
{
public:
	//һ���˿ͻ���һ��base
	struct event_base* base = 0;

	//���ӵ��׽���
	int sock = 0;

	//�̳߳�ID
	int thread_id = 0;		

	//��ʼ������
	virtual bool Init() = 0;
};