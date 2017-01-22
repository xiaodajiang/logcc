// loglibtest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../loglib/loglib.h"

#ifdef _DEBUG
#pragma comment(lib, "../debug/loglib.lib")
#else
#pragma comment(lib, "../Release/loglib.lib")
#endif



int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0;
	//ret = log_init(0,4,1,"");
	//ret = log_init(0,0,1,("..\\configfile\\log4cplus.properties"));
	ret = log_init(0,0,1,("log4cplus.properties"));
	//ret = log_init();
	assert(ret == 0);
	LOG_DEBUG("this is debug info检测.");
	LOG_DEBUGW(_T("this is test."));

	LOG_ERROR(("str = %s."), ("11111111111"));
	LOG_ERRORW(_T("str = %s."), _T("22222222222222222222"));
	log_deinit();
	system("pause");
	return 0;
}

