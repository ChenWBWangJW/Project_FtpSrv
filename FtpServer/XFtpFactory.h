#pragma once
#include "XTask.h"

class XFtpFactory
{
public:
	/*
	*  Get()����ģʽ��Singleton Pattern������ȷ��һ����ֻ��һ��ʵ�������ṩȫ�ַ��ʵ��Թ���������ʹ��
	*/
	static XFtpFactory*Get()
	{
		static XFtpFactory f;
		return &f;
	}

	//���ݲ�ͬ���������ͣ�������ͬ��������
	XTask* CreateTask();

private:
	XFtpFactory();
};
