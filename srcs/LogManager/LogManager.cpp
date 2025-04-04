#include "LogManager.hpp"
#include <ctime>
#include <cstdarg>

// Définition des variables statiques
bool LogManager::_logStatus = false;
bool LogManager::_logFileStatus = false;
bool LogManager::_debugLogStatus = false;
std::string LogManager::_logFileName = "log.txt";
std::map<LogManager::LogLevel, std::string> LogManager::_logLevelStr;
std::map<LogManager::LogLevel, std::string> LogManager::_logLevelColor;

// Fonction d'initialisation statique
void LogManager::initialize()
{
    _logLevelStr[DEBUG] = "DEBUG";
    _logLevelStr[INFO] = "INFO";
    _logLevelStr[WARNING] = "WARNING";
    _logLevelStr[ERROR] = "ERROR";
    _logLevelStr[CRITICAL] = "CRITICAL";
    _logLevelStr[FATAL] = "FATAL";

    // Initialisation des couleurs si nécessaire
    _logLevelColor[DEBUG] = "\033[0;36m";
    _logLevelColor[INFO] = "\033[0;32m";
    _logLevelColor[WARNING] = "\033[0;33m";
    _logLevelColor[ERROR] = "\033[0;31m";
    _logLevelColor[CRITICAL] = "\033[0;35m";
    _logLevelColor[FATAL] = "\033[0;41m";
}

/*
 * @brief Enregistre un message de log
 *
 * @param level Le niveau du message
 * @param message Le message à enregistrer
 */
void LogManager::log(LogManager::LogLevel level, const char *message, ...)
{
    // Vérifier si le log est activé pour le niveau DEBUG
    if (LogManager::getLogStatus() == false || (level == DEBUG && LogManager::getDebugLogStatus() == false))
    {
        return;
    }

    // Formater le message de log
    va_list args;
    va_start(args, message);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    // Obtenir l'heure actuelle
    std::time_t now = std::time(0);
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // Afficher le message de log dans la console
    _printLog(level, buffer, timeStr);

    // Écrire le message de log dans un fichier si activé
    if (LogManager::getLogFileStatus())
    {
        _writeLogInFile(level, buffer, timeStr);
    }
}

/*
 * @brief Récupère l'état du log
 *
 * @return true si le log est activé, false sinon
 */
bool LogManager::getLogStatus()
{
    return _logStatus;
}

/*
 * @brief Récupère l'état du log dans un fichier
 *
 * @return true si le log dans un fichier est activé, false sinon
 */
bool LogManager::getLogFileStatus()
{
    return _logFileStatus;
}

/*
 * @brief Récupère l'état du log dans la console
 *
 * @return true si le log dans la console est activé, false sinon
 */
bool LogManager::getDebugLogStatus()
{
    return _debugLogStatus;
}

/*
 * @brief Définit l'état du log
 *
 * @param status true pour activer le log, false pour le désactiver
 */
void LogManager::setLogStatus(bool status)
{
    _logStatus = status;
}

/*
 * @brief Définit l'état du log dans un fichier
 *
 * @param status true pour activer le log dans un fichier, false pour le désactiver
 */
void LogManager::setLogFileStatus(bool status)
{
    _logFileStatus = status;
}

/*
 * @brief Définit l'état du log dans la console
 *
 * @param status true pour activer le log dans la console, false pour le désactiver
 */
void LogManager::setDebugLogStatus(bool status)
{
    _debugLogStatus = status;
}

/*
 * @brief Affiche un message de log dans la console
 *
 * @param level Le niveau du message
 * @param msg Le message à afficher
 * @param time L'heure du message
 */
void LogManager::_printLog(LogManager::LogLevel level, const char *msg, const char *time)
{
    initialize();
    std::cout << "[" << time << "] [" << _logLevelColor[level] << _logLevelStr[level] << "\033[0m] " << msg << std::endl;
}

/*
 * @brief Écrit un message de log dans un fichier
 *
 * @param level Le niveau du message
 * @param msg Le message à écrire
 * @param time L'heure du message
 */
void LogManager::_writeLogInFile(LogManager::LogLevel level, const char *msg, const char *time)
{
    std::ofstream logFile(_logFileName.c_str(), std::ios_base::app);
    if (logFile.is_open())
    {
        logFile << "[" << time << "] [" << _logLevelStr[level] << "] " << msg << std::endl;
        logFile.close();
    }
}