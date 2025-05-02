#include "CommandLineParser.hpp"

// Constructors and destructors

CommandLineParser::CommandLineParser(int argc, char **argv)
{
    _parseCommandLine(argc, argv);
    LogManager::setLogStatus(true);
    if (hasOption("debug"))
    {
        LogManager::setDebugLogStatus(true);
        LogManager::log(LogManager::INFO, "Debug mode enabled.");
    }

    if (hasOption("logfile"))
    {
        LogManager::setLogFileStatus(true);
        LogManager::log(LogManager::INFO, "Logging to file enabled.");
    }
}

CommandLineParser::~CommandLineParser()
{
}

/***************************************************  COMMAND LINE ********************************************************/

/**
 * @brief Affiche l'aide pour la ligne de commande.
 * 
 */
void    CommandLineParser::printHelp() const
{
    std::cout << "Usage: ./webserv [configuration_file] [options]" << std::endl;
    std::cout << "Configuration file: Path to the configuration file (default: " << DEFAULT_CONFIG_FILE << ")" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --debug            Enable debug mode" << std::endl;
    std::cout << "  --logfile          Enable logging to file" << std::endl;   
    std::cout << "  --help            Show this help message" << std::endl;
}

/**
 * @brief Parse la ligne de commande et remplit les options.
 * 
 * @param argc 
 * @param argv 
 */
void    CommandLineParser::_parseCommandLine(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        // Vérifie si l'argument est une option (commence par '--')
        if (arg.rfind("--", 0) == 0)
        {
            // Vérifie si l'option est valide
            if (isValidOption(arg))
            {
                // Si c'est une option, ajoutez-la à la liste des options
                _options.push_back(arg);
            }
            else
            {
                LogManager::log(LogManager::ERROR, "Invalid option: %s", arg.c_str());
                throw std::invalid_argument("Invalid option: " + arg);
            }
        }
        else
        {
            // Si ce n'est pas une option, c'est le fichier de configuration
            if (_configFilePath.empty())
            {
                _configFilePath = arg;
                LogManager::log(LogManager::DEBUG, "Configuration file set to: %s", _configFilePath.c_str());
            }
            else
            {
                throw std::invalid_argument("Multiple configuration files are not allowed.");
            }
        }
    }
    if (_configFilePath.empty())
    {
        _configFilePath = DEFAULT_CONFIG_FILE;
        LogManager::log(LogManager::INFO, "No configuration file provided. Using default: %s", DEFAULT_CONFIG_FILE);
    }
}

/**
 * @brief Vérifie si un fichier de configuration a été fourni.
 * 
 * @return true 
 * @return false 
 */
bool    CommandLineParser::hasConfigFile() const
{
    return !_configFilePath.empty();
}

/**
 * @brief Récupère le chemin du fichier de configuration.
 * 
 * @return std::string 
 */
std::string CommandLineParser::getConfigFilePath() const
{
    if (_configFilePath.empty())
    {
        throw std::runtime_error("No configuration file provided.");
    }
    return _configFilePath;
}

/**
 * @brief  Vérifie si une option est présente dans la ligne de commande.
 * 
 * @param option 
 * @return true 
 * @return false 
 */
bool CommandLineParser::hasOption(const std::string &option) const
{
    for (std::vector<std::string>::const_iterator it = _options.begin(); it != _options.end(); ++it)
    {
        if (*it == option)
        {
            return true;
        }
    }
    return false;
}


bool     CommandLineParser::isValidOption(const std::string& option)
{
    return (option == "--debug" || option == "--logfile" || option == "--help");
}