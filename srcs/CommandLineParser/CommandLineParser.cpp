#include "CommandLineParser.hpp"

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
        std::string logFile = getOption("logfile");
        LogManager::log(LogManager::INFO, "Logging to file: %s", logFile.c_str());
    }


    
}

CommandLineParser::~CommandLineParser()
{
}
void    CommandLineParser::printHelp() const
{
    std::cout << "Usage: ./webserv [configuration_file] [options]" << std::endl;
    std::cout << "Configuration file: Path to the configuration file (default: " << DEFAULT_CONFIG_FILE << ")" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --debug            Enable debug mode" << std::endl;
    std::cout << "  --logfile          Enable logging to file" << std::endl;   
    std::cout << "  --help            Show this help message" << std::endl;
}

void    CommandLineParser::_parseCommandLine(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        // Vérifie si l'argument est une option (commence par '--')
        if (arg.rfind("--", 0) == 0)
        {
            size_t equalPos = arg.find('=');
            if (equalPos != std::string::npos)
            {
                std::string key = arg.substr(2, equalPos - 2);
                std::string value = arg.substr(equalPos + 1);
                _options[key] = value;
            }
            else
            {
                _options[arg.substr(2)] = ""; // Option sans valeur
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

bool    CommandLineParser::hasConfigFile() const
{
    return !_configFilePath.empty();
}

std::string CommandLineParser::getConfigFilePath() const
{
    if (_configFilePath.empty())
    {
        throw std::runtime_error("No configuration file provided.");
    }
    return _configFilePath;
}

bool CommandLineParser::hasOption(const std::string &option) const
{
    return _options.find(option) != _options.end();
}

std::string CommandLineParser::getOption(const std::string &option) const
{
    std::map<std::string, std::string>::const_iterator it = _options.find(option);
    if (it == _options.end())
    {
        throw std::runtime_error("Option '" + option + "' not found.");
    }
    return it->second;
}