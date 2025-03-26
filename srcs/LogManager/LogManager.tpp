#include "LogManager.hpp"


/**
 * @brief 
 * 
 * @tparam T Type du message à afficher
 * @param level niveau de log
 * @param msg   message à afficher
 */
template<typename T>
void LogManager::log(LogLevel level, T msg)
{
    if (!getLogStatus() || (level == DEBUG && !getLogConsoleStatus()))
    {
        return;
    }

    // Obtenir l'heure actuelle
    std::time_t now = std::time(0);
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // Afficher le message de log dans la console
    _printLog(level, msg, timeStr);

    // Écrire le message de log dans un fichier si activé
    if (getLogFileStatus())
    {
        _writeLogInFile(level, msg, timeStr);
    }
}

template<typename T>
void LogManager::_printLog(LogLevel level, T msg, const char *time)
{
    LogManager::initialize();
    std::cout << "[" << time << "] [" << _logLevelColor[level] << _logLevelStr[level] << "\033[0m] " << msg << std::endl;
}

template<typename T>
void LogManager::_writeLogInFile(LogLevel level, T msg, const char *time)
{
    std::ofstream logFile(_logFileName.c_str(), std::ios_base::app);
    if (logFile.is_open())
    {
        logFile << "[" << time << "] [" << _logLevelStr[level] << "] " << msg << std::endl;
        logFile.close();
    }
}

