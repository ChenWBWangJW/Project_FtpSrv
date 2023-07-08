#include "XFtpUSER.h"
#include "testUtil.h"

void XFtpUSER::Parse(std::string, std::string)
{
	testout("At XFtpUSER::Parse()");
	ResCMD("230 Login successful.\r\n");
}