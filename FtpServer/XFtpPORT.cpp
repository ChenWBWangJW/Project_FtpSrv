#include "XFtpPORT.h"
#include "testUtil.h"

#include <iostream>
#include <vector>
using namespace std;

/*********************************************
* @说明：
*		在XFtpPORT::Parse()中，解析出ip地址和端
* 口号并设置在主要流程cmdTask中。最后向客户端返
* 回200PORT command successful.
**********************************************/
void XFtpPORT::Parse(string type, string msg)
{
	testout("XFtpPORT::Parse()");
	// PORT 127,0,0,1,70,96\r\n
	// PORT n1,n2,n3,n4,n5,n6\r\n
	// PROT = n5*256 + n6

	vector<string>vals;
	string tmp = "";
	for (int i = 5; i < msg.size(); i++)			//从第5个字符开始循环，因为前面的字符是"PORT "
	{
		if (msg[i] == ',' || msg[i] == '\r')		//数字则不进入循环，将该数字字符串写入tmp中
		{
			vals.push_back(tmp);					//是逗号或者回车则将tmp{上次循环赋值的数字写入vals中
			tmp = "";								//更新tmp内容
			continue;								//回到循环判断条件
		}
		tmp += msg[i];								//将数字写入tmp中
	}
	if (vals.size() != 6)							//循环结束后数组vals中应该有ip地址和端口号的6个数字，若不是则报错
	{
		ResCMD("501 Syntax error in parameters or arguments.\r\n");
		return;
	}

	//解析处ip地址和端口号，并设置在主要流程cmdTask中
	ip = vals[0] + "." + vals[1] + "." + vals[2] + "." + vals[3];		//拼接ip地址
	port = atoi(vals[4].c_str()) * 256 + atoi(vals[5].c_str());			//拼接端口号
	cmdTask->ip = ip;													//设置在主要流程cmdTask中
	cmdTask->port = port;												//设置在主要流程cmdTask中
	testout("IP: " << ip);
	testout("PORT: " << port);
	ResCMD("200 PORT command successful.\r\n");
}
