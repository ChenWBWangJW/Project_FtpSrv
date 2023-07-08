#include "XFtpFactory.h"
#include "XFtpServerCMD.h"
#include "XFtpUSER.h"
#include "XFtpLIST.h"
#include "XFtpPORT.h"
#include "XFtpRETR.h"
#include "XFtpSTOR.h"
#include "testUtil.h"

/*********************************************
* @说明：
*		1.实例化命令处理器(XFtpServerCMD 对象)
*		2.往命令处理器中注册命令处理器
*		3.创建列表类管理后续命令
*		4.注册文件操作相关命令
*
* @目的：
*		添加要处理的FTP命令，如USER、PORT等。其
*中，USER、PORT等命令的处理器是XFtpUSER、XFtpPORT
* 等类，这些类都是XFtpServerCMD类的子类，而XFtpServerCMD
* 类是XTask类的子类，所以这些类都是任务类，都有
* Run函数。
* 
* @参数：
*		XFtpUSER:
*			实现USER命令，根据需要实现具体登录逻辑
* 
*		XFtpPORT:
*			实现PORT命令，解析IP地址和端口号
* 
*		XFtpRETE:
*			实现RETR命令，下载文件
* 
*		XFtpSTOR:
*			实现STOR命令，上传文件
**********************************************/
XTask *XFtpFactory::CreateTask()		//返回值为XTask*，即任务类指针的属于XFtpFactory类的成员函数
{
	testout("At XFtpFactory::CreateTask");
	XFtpServerCMD *x = new XFtpServerCMD();		//注册命令处理器
	
	x->Reg("USER", new XFtpUSER());				//注册USER命令处理器，用于进行用户鉴定

	x->Reg("PORT", new XFtpPORT());				//注册PORT命令处理器，用于解析IP地址和端口号

	XFtpTask *list = new XFtpLIST();			//创建列表类管理后续命令
	x->Reg("PWD", list);						//注册PWD命令处理器，用于显示当前目录
	x->Reg("LIST", list);						//注册LIST命令处理器，用于显示当前目录下的文件列表
	x->Reg("CWD", list);						//注册CWD命令处理器，用于切换目录
	x->Reg("CDUP", list);						//注册CDUP命令处理器，用于切换到上一级目录

	x->Reg("RETR", new XFtpRETR());				//注册RETR命令处理器，用于下载文件
	x->Reg("STOR", new XFtpSTOR());				//注册STOR命令处理器，用于上传文件

	return x;
}

XFtpFactory::XFtpFactory()
{
}