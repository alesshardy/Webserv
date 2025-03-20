#ifndef LOGMANAGER_HPP
#define LOGMANAGER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

class LogManager
{

    private:
        bool _logStatus;
        bool _logFileStatus;
        bool _logConsoleStatus;




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

        static void log(LogLevel level, const std::string &message, ...);

        // setters
        void setLogStatus(bool status);
        void setLogFileStatus(bool status);
        void setLogConsoleStatus(bool status);

        // getters
        bool getLogStatus() const;
        bool getLogFileStatus() const;
        bool getLogConsoleStatus() const;
};

#endif // LOGMANAGER_HPP
