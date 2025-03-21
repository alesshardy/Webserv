#ifndef LOGMANAGER_HPP
#define LOGMANAGER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <stdarg.h>
#include <map>

class LogManager
{
    public:
        enum LogLevel
        {
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            CRITICAL,
            FATAL
        };

    private:
        static bool _logStatus;
        static bool _logFileStatus;
        static bool _logConsoleStatus;
        static std::string _logFileName;
        static std::map<LogLevel, std::string> _logLevelStr;
        static std::map<LogLevel, std::string> _logLevelColor;

        // Méthodes utilitaires
        static std::string _generateLogFileName();
        static void _generateLogLevelStr();
        static void _generateLogLevelColor();
        static std::string _formatLog(LogLevel level, const char *msg, const char *time, bool colored);
        static void _printLog(LogLevel level, const char *msg, const char *time);
        static void _writeLogInFile(LogLevel level, const char *msg, const char *time);

    public:
        static void initialize();
        static void log(LogLevel level, const char *message, ...);

        // setters
        static void setLogStatus(bool status);
        static void setLogFileStatus(bool status);
        static void setLogConsoleStatus(bool status);

        // getters
        static bool getLogStatus();
        static bool getLogFileStatus();
        static bool getLogConsoleStatus();
        static std::string getLogFileName();
        static std::string getLogLevelStr(LogLevel level);
        static std::string getLogLevelColor(LogLevel level);
};

#endif // LOGMANAGER_HPP