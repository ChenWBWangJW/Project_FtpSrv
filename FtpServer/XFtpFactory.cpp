#include "XFtpFactory.h"
#include "XFtpServerCMD.h"
#include "XFtpUSER.h"
#include "XFtpLIST.h"
#include "XFtpPORT.h"
#include "XFtpRETR.h"
#include "XFtpSTOR.h"
#include "testUtil.h"

/*********************************************
* @˵����
*		1.ʵ�����������(XFtpServerCMD ����)
*		2.�����������ע���������
*		3.�����б�������������
*		4.ע���ļ������������
*
* @Ŀ�ģ�
*		���Ҫ�����FTP�����USER��PORT�ȡ���
*�У�USER��PORT������Ĵ�������XFtpUSER��XFtpPORT
* ���࣬��Щ�඼��XFtpServerCMD������࣬��XFtpServerCMD
* ����XTask������࣬������Щ�඼�������࣬����
* Run������
* 
* @������
*		XFtpUSER:
*			ʵ��USER���������Ҫʵ�־����¼�߼�
* 
*		XFtpPORT:
*			ʵ��PORT�������IP��ַ�Ͷ˿ں�
* 
*		XFtpRETE:
*			ʵ��RETR��������ļ�
* 
*		XFtpSTOR:
*			ʵ��STOR����ϴ��ļ�
**********************************************/
XTask *XFtpFactory::CreateTask()		//����ֵΪXTask*����������ָ�������XFtpFactory��ĳ�Ա����
{
	testout("At XFtpFactory::CreateTask");
	XFtpServerCMD *x = new XFtpServerCMD();		//ע���������
	
	x->Reg("USER", new XFtpUSER());				//ע��USER������������ڽ����û�����

	x->Reg("PORT", new XFtpPORT());				//ע��PORT������������ڽ���IP��ַ�Ͷ˿ں�

	XFtpTask *list = new XFtpLIST();			//�����б�������������
	x->Reg("PWD", list);						//ע��PWD���������������ʾ��ǰĿ¼
	x->Reg("LIST", list);						//ע��LIST���������������ʾ��ǰĿ¼�µ��ļ��б�
	x->Reg("CWD", list);						//ע��CWD��������������л�Ŀ¼
	x->Reg("CDUP", list);						//ע��CDUP��������������л�����һ��Ŀ¼

	x->Reg("RETR", new XFtpRETR());				//ע��RETR������������������ļ�
	x->Reg("STOR", new XFtpSTOR());				//ע��STOR��������������ϴ��ļ�

	return x;
}

XFtpFactory::XFtpFactory()
{
}