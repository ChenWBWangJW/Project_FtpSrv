#include "XFtpPORT.h"
#include "testUtil.h"

#include <iostream>
#include <vector>
using namespace std;

/*********************************************
* @˵����
*		��XFtpPORT::Parse()�У�������ip��ַ�Ͷ�
* �ںŲ���������Ҫ����cmdTask�С������ͻ��˷�
* ��200PORT command successful.
**********************************************/
void XFtpPORT::Parse(string type, string msg)
{
	testout("XFtpPORT::Parse()");
	// PORT 127,0,0,1,70,96\r\n
	// PORT n1,n2,n3,n4,n5,n6\r\n
	// PROT = n5*256 + n6

	vector<string>vals;
	string tmp = "";
	for (int i = 5; i < msg.size(); i++)			//�ӵ�5���ַ���ʼѭ������Ϊǰ����ַ���"PORT "
	{
		if (msg[i] == ',' || msg[i] == '\r')		//�����򲻽���ѭ�������������ַ���д��tmp��
		{
			vals.push_back(tmp);					//�Ƕ��Ż��߻س���tmp{�ϴ�ѭ����ֵ������д��vals��
			tmp = "";								//����tmp����
			continue;								//�ص�ѭ���ж�����
		}
		tmp += msg[i];								//������д��tmp��
	}
	if (vals.size() != 6)							//ѭ������������vals��Ӧ����ip��ַ�Ͷ˿ںŵ�6�����֣��������򱨴�
	{
		ResCMD("501 Syntax error in parameters or arguments.\r\n");
		return;
	}

	//������ip��ַ�Ͷ˿ںţ�����������Ҫ����cmdTask��
	ip = vals[0] + "." + vals[1] + "." + vals[2] + "." + vals[3];		//ƴ��ip��ַ
	port = atoi(vals[4].c_str()) * 256 + atoi(vals[5].c_str());			//ƴ�Ӷ˿ں�
	cmdTask->ip = ip;													//��������Ҫ����cmdTask��
	cmdTask->port = port;												//��������Ҫ����cmdTask��
	testout("IP: " << ip);
	testout("PORT: " << port);
	ResCMD("200 PORT command successful.\r\n");
}
