#ifndef COMMANDLINEPARSER_HPP
#define COMMANDLINEPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "LogManager.hpp"
#include "../includes/Webserv.hpp"

#define DEFAULT_CONFIG_FILE "testConfig/default.conf"

class CommandLineParser
{
    private:
        std::vector<std::string>            _options;
        std::string                         _configFilePath;

        void       _parseCommandLine(int argc, char **argv);
    
    public:
        CommandLineParser(int argc, char **argv);
        ~CommandLineParser();

        bool        hasOption(const std::string &option) const;
        bool        hasConfigFile() const;
        std::string getConfigFilePath() const;
        void        printHelp() const;
        bool        isValidOption(const std::string &option);
};

#endif // COMMANDLINEPARSER_HPP