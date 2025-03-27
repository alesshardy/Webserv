#ifndef LOGMANAGER_HPP
#define LOGMANAGER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
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

        static void _writeLogInFile(LogLevel level, const char *msg, const char *time);
        static void _printLog(LogManager::LogLevel level, const char *msg, const char *time);


        // template<typename T>
        // static void _printLog(LogLevel level, T msg, const char *time);

        // template<typename T>
        // static void _writeLogInFile(LogLevel level, T msg, const char *time);

    public:
        // template<typename T>
        // static void log(LogLevel level, T msg);
        static void log(LogLevel level, const char *message, ...);
        static void initialize();
        static void setLogStatus(bool status);
        static void setLogFileStatus(bool status);
        static void setLogConsoleStatus(bool status);
        static bool getLogStatus();
        static bool getLogFileStatus();
        static bool getLogConsoleStatus();
        static std::string getLogFileName();
        static std::string getLogLevelStr(LogLevel level);
        static std::string getLogLevelColor(LogLevel level);
};

// Inclure le fichier .tpp ici
// #include "LogManager.tpp"

#endif // LOGMANAGER_HPP