#pragma once
#include "XTask.h"

class XFtpFactory
{
public:
	/*
	*  Get()单例模式（Singleton Pattern），它确保一个类只有一个实例，并提供全局访问点以供其他对象使用
	*/
	static XFtpFactory*Get()
	{
		static XFtpFactory f;
		return &f;
	}

	//根据不同的任务类型，创建不同的任务类
	XTask* CreateTask();

private:
	XFtpFactory();
};
