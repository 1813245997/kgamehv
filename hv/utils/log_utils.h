#pragma once
#define ENABLE_HV_DEBUG_LOG 1 // 设为 0 表示关闭调试日志


/**
* @file LogMacros.h
* @brief Defines macros for logging various levels of messages and a function for log printing.
*
* @date 2024-04-20
* @author killer
*/

 
/**
 * @enum __log_type
 * @brief Enumeration of log message types.
 *
 * Enumeration for different types of log messages: debug, error, dump, and info.
 */
enum __log_type
{
    LOG_TYPE_DEBUG, /**< Debug message type */
    LOG_TYPE_ERROR, /**< Error message type */
    LOG_TYPE_DUMP, /**< Dump message type */
    LOG_TYPE_INFO /**< Informational message type */
};



/**
 * @brief Logs a formatted message of the specified type.
 *
 * This function prints a formatted log message of the given type (debug, error, dump, or info).
 *
 * @param type The type of log message (debug, error, dump, or info).
 * @param fmt The format string for the message.
 * @param ... Additional arguments for the formatted message.
 */
void LogPrint(__log_type type, const char* fmt, ...);


 

#define LogError(format, ...) \
        LogPrint(LOG_TYPE_ERROR, "  [hv][%s:%d] " format, __func__, __LINE__, ##__VA_ARGS__)

#define LogDebug(format, ...) \
        LogPrint(LOG_TYPE_DEBUG, "  [hv][%s:%d] " format, __func__, __LINE__, ##__VA_ARGS__)

#define LogDump(format, ...) \
        LogPrint(LOG_TYPE_DUMP, " [hv][%s:%d] " format, __func__, __LINE__, ##__VA_ARGS__)

#define LogInfo(format, ...) \
        LogPrint(LOG_TYPE_INFO, "  [hv][%s:%d] " format, __func__, __LINE__, ##__VA_ARGS__)

 

