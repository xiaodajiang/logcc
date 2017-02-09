// loglib.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include "loglib.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/socketappender.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/sleep.h>
#include <io.h>
#include <iostream>

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

#ifdef WIN64
	#ifdef UNICODE
		#ifdef _DEBUG
		#pragma comment(lib, "lib\\x64\\log4cplusUSD.lib")
		#else
		#pragma comment(lib, "lib\\x64\\log4cplusUS.lib")
		#endif
	#else
		#ifdef _DEBUG
		#pragma comment(lib, "lib\\x64\\log4cplusSD.lib")
		#else
		#pragma comment(lib, "lib\\x64\\log4cplusS.lib")
		#endif
	#endif
#else
	#ifdef UNICODE
		#ifdef _DEBUG
		#pragma comment(lib, "lib\\win32\\log4cplusUSD.lib")
		#else
		#pragma comment(lib, "lib\\win32\\log4cplusUS.lib")
		#endif
	#else
		#ifdef _DEBUG
		#pragma comment(lib, "lib\\win32\\log4cplusSD.lib")
		#else
		#pragma comment(lib, "lib\\win32\\log4cplusS.lib")
		#endif
	#endif
#endif


/* Appender type define */
#define APPENDER_TYPE_CONSOLE		0
#define APPENDER_TYPE_FILE			1
#define APPENDER_TYPE_FILE_DAILY	2
#define APPENDER_TYPE_NTEVENTLOG	3
#define APPENDER_TYPE_SCOKET		4

 
/* Appender pattern define */
#define LOG_PATTERN_1 LOG4CPLUS_TEXT("[%D{%Y-%m-%d %H:%M:%S.%q}] [%t] [%-5p] - %m%n")
#define LOG_PATTERN_2 LOG4CPLUS_TEXT("[%D{%Y-%m-%d %H:%M:%S.%q}] [%t] [%-5p] - %m [%l]%n")
#define LOG_PATTERN_3 LOG4CPLUS_TEXT("[%D{%Y-%m-%d %H:%M:%S.%q}] [%t] [%-5p] [%c{2}] %%%x%% - %m [%l]%n")
#define LOG_PATTERN_4 LOG4CPLUS_TEXT("[%D{%Y-%m-%d %H:%M:%S.%q}] [%t] [%-5p] [%.15c{3}] %%%x%% - %m [%l]%n");

#define LOG_PATTERN_5 LOG4CPLUS_TEXT("[%F::%L] [%t] [%-5p] \t%m%n");
#define LOG_PATTERN_6 LOG4CPLUS_TEXT("[%D{%m/%d/%y %H:%M:%S,%q} %t %-5p] - %m%n")

#define LOG_PATTERN LOG_PATTERN_3

bool logging_started = false;
Logger g_logger;
#define LOG_OBJ g_logger//Logger::getRoot()

static std::string ws2s(const std::wstring& ws)  
{  
	std::string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "");  
	const wchar_t* _Source = ws.c_str();  
	size_t _Dsize = 2 * ws.size() + 1;  
	char *_Dest = new char[_Dsize];  
	memset(_Dest, 0, _Dsize);  
	wcstombs(_Dest, _Source, _Dsize);  
	std::string result = _Dest;  
	delete[]_Dest;  
	setlocale(LC_ALL, curLocale.c_str());  
	return result;  
}

static std::wstring s2ws(const std::string& s)  
{  
	setlocale(LC_ALL, "");  
	const char* _Source = s.c_str();  
	size_t _Dsize = s.size() + 1;  
	wchar_t *_Dest = new wchar_t[_Dsize];  
	wmemset(_Dest, 0, _Dsize);  
	mbstowcs(_Dest, _Source, _Dsize);  
	std::wstring result = _Dest;  
	delete[]_Dest;  
	setlocale(LC_ALL, "C");  
	return result;  
}  

static void debug(const char* msg)
{
	LOG4CPLUS_DEBUG(LOG_OBJ, msg);
}

static void info( const char* msg )
{
	LOG4CPLUS_INFO(LOG_OBJ, msg);
}

static void warning(const char* msg)
{
	LOG4CPLUS_WARN(LOG_OBJ, msg);
}

static void error(const char* msg )
{
	LOG4CPLUS_ERROR(LOG_OBJ, msg);
}

static void trace(const char* msg)
{
	LOG4CPLUS_TRACE(LOG_OBJ, msg);
}

static void fatal(const char* msg)
{
	LOG4CPLUS_FATAL(LOG_OBJ, msg);
}


int log_init(const char* loggerName, int appenderType, int isLayout, const char* scriptFile)
{
	if (logging_started)
		return 0;

	log4cplus::initialize();
	g_logger = Logger::getRoot();
	
	/* create log object */
	if(scriptFile && strlen(scriptFile) > 0){
		if(access(scriptFile, 0) != -1){
			//PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(".\log4cplus.properties"));
			const log4cplus::tstring tsScriptFile = LOG4CPLUS_C_STR_TO_TSTRING(scriptFile);
			PropertyConfigurator::doConfigure(tsScriptFile);
			//LOG4CPLUS_INFO(g_logger, LOG4CPLUS_TEXT("Log System Start."));
			logging_started = true;
			return 0;
		}
		return -1;
	}

	if (loggerName && strlen(loggerName) > 0){
		const log4cplus::tstring tsLoggerName = LOG4CPLUS_C_STR_TO_TSTRING(loggerName);
		g_logger =  Logger::getInstance(tsLoggerName);
	}
	g_logger.setLogLevel(ALL_LOG_LEVEL);

	/* create appender object */
	if(appenderType == APPENDER_TYPE_CONSOLE)
	{
		SharedObjectPtr<Appender> append(new ConsoleAppender());
		append->setName(LOG4CPLUS_TEXT("ConsoleAppender"));

		if(isLayout){ /* set appender layout */
			log4cplus::tstring pattern = LOG_PATTERN;
			//std::tstring pattern = LOG4CPLUS_TEXT("%d{%c} [%t] %-5p [%.15c{3}] %%%x%% - %m [%l]%n");
			append->setLayout( std::auto_ptr<Layout>(new PatternLayout(pattern)) );
		}else{ /* default layout */
			append->setLayout(std::auto_ptr<Layout>(new TTCCLayout()));
		}
		g_logger.addAppender(append);
	}
	else if(appenderType == APPENDER_TYPE_FILE)
	{
		SharedAppenderPtr append(new RollingFileAppender(LOG4CPLUS_TEXT("C:\\Test.log"), 5*1024, 5));
		append->setName(LOG4CPLUS_TEXT("FileAppender"));

		if(isLayout){
			log4cplus::tstring pattern = LOG_PATTERN;
			append->setLayout( std::auto_ptr<Layout>(new PatternLayout(pattern)) );
		}else{
			append->setLayout(std::auto_ptr<Layout>(new TTCCLayout()));
		}
		g_logger.addAppender(append);
	}
	else if(appenderType == APPENDER_TYPE_FILE_DAILY)
	{
		SharedAppenderPtr append(new DailyRollingFileAppender(LOG4CPLUS_TEXT("C:\\Test.log")));
		append->setName(LOG4CPLUS_TEXT("DaliyFileAppender"));

		if(isLayout){
			log4cplus::tstring pattern = LOG_PATTERN;
			append->setLayout( std::auto_ptr<Layout>(new PatternLayout(pattern)) );
		}else{
			append->setLayout(std::auto_ptr<Layout>(new TTCCLayout()));
		}
		g_logger.addAppender(append);
	}
	else if(appenderType == APPENDER_TYPE_SCOKET)
	{
		//tstring serverName = (argc > 1 ? LOG4CPLUS_C_STR_TO_TSTRING(argv[1]) : tstring());
		//tstring host = LOG4CPLUS_TEXT("192.168.2.10");
		tstring host = LOG4CPLUS_TEXT("127.0.0.1");
		SharedAppenderPtr append_1(new SocketAppender(host, 9999));
		append_1->setName( LOG4CPLUS_TEXT("SocketAppender") );
		g_logger.addAppender(append_1);
	}
	else
	{
		return -1;
	} 

	LOG4CPLUS_INFO(g_logger, LOG4CPLUS_TEXT("Log System Start."));
	logging_started = true;
	return 0;
}

int log_deinit()
{
	LOG4CPLUS_INFO(g_logger, LOG4CPLUS_TEXT("Log System Stop."));
	log4cplus::Logger::shutdown();
	logging_started = false;
	return 0;
}

void log_trace(const char* file, int line, int level, const char* format, ...)
{
	try
	{
		char buf[1024];
		va_list ap;
		va_start(ap, format);
		//_vsntprintf_s(buf, 1024, format, ap);
		_vsnprintf_s(buf, 1024, format, ap);
		va_end(ap);

		if(strlen(buf) <= 0) return;

		const log4cplus::tstring tsBuf = LOG4CPLUS_C_STR_TO_TSTRING(buf);

		if(level == LOG_LEVEL_TRACE){
			if(file && line > 0){
				g_logger.forcedLog(TRACE_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(TRACE_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_DEBUG){
			if(file && line > 0){
				g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_INFO){
			if(file && line > 0){
				g_logger.forcedLog(INFO_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(INFO_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_WARN){
			if(file && line > 0){
				g_logger.forcedLog(WARN_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(WARN_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_ERROR){
			if(file && line > 0){
				g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_FATAL){
			if(file && line > 0){
				g_logger.forcedLog(FATAL_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(FATAL_LOG_LEVEL, tsBuf);
			}
		}
	}
	catch(std::exception& e) {
		std::cout << "Exception: " << e.what () << std::endl;
	}
}

void log_trace_w(const char* file, int line, int level, const wchar_t* format, ...)
{
	try
	{
		wchar_t buf[1024];
		va_list ap;
		va_start(ap, format);
		_vsnwprintf_s(buf, 1024, format, ap);
		va_end(ap);

		if(wcslen(buf) <= 0) return;

		const log4cplus::tstring tsBuf = LOG4CPLUS_C_STR_TO_TSTRING(ws2s(buf));

		if(level == LOG_LEVEL_TRACE){
			if(file && line > 0){
				g_logger.forcedLog(TRACE_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(TRACE_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_DEBUG){
			if(file && line > 0){
				g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_INFO){
			if(file && line > 0){
				g_logger.forcedLog(INFO_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(INFO_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_WARN){
			if(file && line > 0){
				g_logger.forcedLog(WARN_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(WARN_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_ERROR){
			if(file && line > 0){
				g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf);
			}
		}else if (level == LOG_LEVEL_FATAL){
			if(file && line > 0){
				g_logger.forcedLog(FATAL_LOG_LEVEL, tsBuf, file, line);
			}else{
				g_logger.forcedLog(FATAL_LOG_LEVEL, tsBuf);
			}
		}
	}
	catch(std::exception& e) {
		std::cout << "Exception: " << e.what () << std::endl;
	}
}

void log_debug(const char* file, int line, const char* format, ...)
{
	try{
		char buf[1024] = {0};
		va_list ap;
		va_start(ap, format);
		//_vsntprintf_s(buf, 1024, format, ap);
		_vsnprintf_s(buf, 1024, format, ap);
		va_end(ap);

		if(strlen(buf) <= 0) return;

		const log4cplus::tstring tsBuf = LOG4CPLUS_C_STR_TO_TSTRING(buf);

		if(file && line > 0){
			g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf, file, line);
		}else{
			g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf);
		}
	}catch(std::exception& e){
		std::cout << "Exception: " << e.what () << std::endl;
	}
}

void log_debug_w(const char* file, int line, const wchar_t* format, ...)
{
	try{
		wchar_t buf[1024] = {0};
		va_list ap;
		va_start(ap, format);
		_vsnwprintf_s(buf, 1024, format, ap);
		va_end(ap);

		if(wcslen(buf) <= 0) return;

		const log4cplus::tstring tsBuf = LOG4CPLUS_C_STR_TO_TSTRING(ws2s(buf));

		if(file && line > 0){
			g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf, file, line);
		}else{
			g_logger.forcedLog(DEBUG_LOG_LEVEL, tsBuf);
		}


	}
	catch(std::exception& e){
		std::cout << "Exception: " << e.what () << std::endl;
	}
}


void log_error(const char* file, int line, const char* format, ...)
{
	try{
		char buf[1024] = {0};
		va_list ap;
		va_start(ap, format);
		_vsnprintf_s(buf, 1024, format, ap);
		va_end(ap);

		if(strlen(buf) <= 0) return;

		const log4cplus::tstring tsBuf = LOG4CPLUS_C_STR_TO_TSTRING(buf);

		if(file && line > 0){
			g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf, file, line);
		}else{
			g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf);
		}
	}
	catch(std::exception& e) {
		std::cout << "Exception: " << e.what () << std::endl;
	}
}

void log_error_w(const char* file, int line, const wchar_t* format, ...)
{
	try{
		wchar_t buf[1024] = {0};
		va_list ap;
		va_start(ap, format);
		_vsnwprintf_s(buf, 1024, format, ap);
		va_end(ap);

		if(wcslen(buf) <= 0) return;

		const log4cplus::tstring tsBuf = LOG4CPLUS_C_STR_TO_TSTRING(ws2s(buf));

		if(file && line > 0){
			g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf, file, line);
		}else{
			g_logger.forcedLog(ERROR_LOG_LEVEL, tsBuf);
		}
	}
	catch(std::exception& e) {
		std::cout << "Exception: " << e.what () << std::endl;
	}
}


int log_setLevel(int level)
{
	switch(level)
	{
	case LOG_LEVEL_TRACE:
		LOG_OBJ.setLogLevel(TRACE_LOG_LEVEL);break;
	case LOG_LEVEL_DEBUG:
		LOG_OBJ.setLogLevel(DEBUG_LOG_LEVEL);break;
	case LOG_LEVEL_WARN:
		LOG_OBJ.setLogLevel(WARN_LOG_LEVEL);break;
	case LOG_LEVEL_ERROR:
		LOG_OBJ.setLogLevel(ERROR_LOG_LEVEL);break;
	case LOG_LEVEL_FATAL:
		LOG_OBJ.setLogLevel(FATAL_LOG_LEVEL);break;
	case LOG_LEVEL_OFF:
		LOG_OBJ.setLogLevel(OFF_LOG_LEVEL);break;
	default:
		LOG_OBJ.setLogLevel(INFO_LOG_LEVEL);break;
	}
	return 0;
}

static void useConsole()
{
	LOG_OBJ.removeAllAppenders();
	SharedAppenderPtr append(new ConsoleAppender());
	append->setName(LOG4CPLUS_TEXT("myconsolelog"));
	tstring pattern = LOG_PATTERN;
	auto_ptr<Layout> lay(new PatternLayout(pattern));
	append->setLayout(lay);
	LOG_OBJ.addAppender(append);
}

static void useRollingFile(const char* szFileName)
{
	if(szFileName == NULL) return;
	const log4cplus::tstring tsFileName = LOG4CPLUS_C_STR_TO_TSTRING(szFileName);

	LOG_OBJ.removeAllAppenders();
	SharedAppenderPtr append(new RollingFileAppender(tsFileName));
	append->setName(LOG4CPLUS_TEXT("myrflog"));
	tstring pattern = LOG_PATTERN;
	auto_ptr<Layout> lay(new PatternLayout(pattern));
	append->setLayout(lay);
	LOG_OBJ.addAppender(append);
}

static void useDailyRollingFile(const char* szFileName /*= "d:\\mylog.log"*/)
{
	if(szFileName == NULL) return;
	const log4cplus::tstring tsFileName = LOG4CPLUS_C_STR_TO_TSTRING(szFileName);
	
	LOG_OBJ.removeAllAppenders();
	SharedAppenderPtr append(new DailyRollingFileAppender(tsFileName));
	append->setName(LOG4CPLUS_TEXT("mydrflog"));
	tstring pattern = LOG_PATTERN;
	auto_ptr<Layout> lay(new PatternLayout(pattern));
	append->setLayout(lay);
	LOG_OBJ.addAppender(append);
}

int log_setAppender(int appenderType, const char* logFile, const char* ipAddr, int port)
{
	if (appenderType == APPENDER_TYPE_CONSOLE){
		useConsole();
	}else if (appenderType == APPENDER_TYPE_FILE){
		if (logFile){
			useRollingFile(logFile);
		}else{
			return -1;
		}
	}else if(appenderType == APPENDER_TYPE_FILE_DAILY){
		if (logFile){
			useDailyRollingFile(logFile);
		}else{
			return -1;
		}
	}else if(appenderType == APPENDER_TYPE_SCOKET){
		return -1;
	}else{
		return -1;
	}
	return 0;
}
