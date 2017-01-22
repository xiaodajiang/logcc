#ifndef __LOGLIB_H__
#define __LOGLIB_H__

#ifdef LOGLIB_EXPORTS
#define LOGLIB_API __declspec(dllexport)
#else
#define LOGLIB_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define LOG_LEVEL_NOT		-1
#define LOG_LEVEL_TRACE		0
/* turn on all logging */
#define LOG_LEVEL_ALL LOG_LEVEL_TRACE
#define LOG_LEVEL_DEBUG		1
#define LOG_LEVEL_INFO		2
#define LOG_LEVEL_WARN		3
#define LOG_LEVEL_ERROR		4
#define LOG_LEVEL_FATAL		5
/* turn off all logging */
#define LOG_LEVEL_OFF		6

/**
 * 初始化日志化，创建日志对象
 * @param loggerName 日志对象名称
 * @param appenderType 挂接器类型
 * @param isLayout 是否使用自定义输出格式
 * @param scriptFile 脚本文件
 * @return 成功返回0
 * @remark
 */
LOGLIB_API int log_init(const char* loggerName = NULL, int appenderType = 0, int isLayout = 0, const char* scriptFile = NULL);
LOGLIB_API int log_deinit();

LOGLIB_API int log_setLevel(int level);
LOGLIB_API int log_setAppender(int appenderType, const char* logFile, const char* ipAddr, int port);

LOGLIB_API void log_trace(const char* file, int line, int level, const char* format, ...);
LOGLIB_API void log_debug(const char* file, int line, const char* format, ...);
LOGLIB_API void log_error(const char* file, int line, const char* format, ...);

LOGLIB_API void log_trace_w(const char* file, int line, int level, const wchar_t* format, ...);
LOGLIB_API void log_debug_w(const char* file, int line, const wchar_t* format, ...);
LOGLIB_API void log_error_w(const char* file, int line, const wchar_t* format, ...);

#define LOG_TRACE(level, fmt, ...)	log_trace(__FILE__, __LINE__, level, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)	log_debug(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)	log_error(__FILE__, __LINE__, fmt, __VA_ARGS__)

#define LOG_TRACEW(level, fmt, ...)	log_trace_w(__FILE__, __LINE__, level, fmt, __VA_ARGS__)
#define LOG_DEBUGW(fmt, ...)	log_debug_w(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define LOG_ERRORW(fmt, ...)	log_error_w(__FILE__, __LINE__, fmt, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif