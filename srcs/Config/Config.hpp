#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>
# include <set>
# include <fstream>
# include <sstream>
# include "BlocServer.hpp"
# include "BlocLocation.hpp"
# include "Utils.hpp"
# include "Listen.hpp"
# include "LogManager.hpp"

#include <sys/stat.h>
# include <climits>

class BlocServer;
class BlocLocation;
class Listen;

class Config
{
    private:
            std::vector<BlocServer>    _server;

    public:
            Config();
            Config(const Config &copy);
            ~Config();

            Config &operator=(const Config &assign);

            //Setters
            void    addServer(const BlocServer &server);

            //Getters
            const std::vector<BlocServer> &getServers() const;
            BlocServer &getServer(size_t index);

            //Validation
            bool    validateConfig() const;

            //Parsing
            void        parseConfigFile(const std::string &filePath, Config &config);
            bool        verifKeyServer(std::string token);
            bool        verifKeyOther(std::string token);

            // Remplir les class
            void        addArgToServerBloc(std::string token, std::string lastKey, BlocServer &current, int argNb);
            void        addArgToLocationBloc(std::string token, std::string lastKey, BlocLocation &current, int argNb);

            void        handleListen(const std::string &arg, BlocServer &current);
            void        handleServerName(const std::string &arg, BlocServer &current);
            void        handleRootBlocServer(const std::string &arg, BlocServer &current, int argNb);
            void        handleIndexBlocServer(const std::string &arg, BlocServer &current);
            void        handleErrorPage(const std::string &arg, BlocServer &current, int argNb);
            void        handleClientMaxBodySize(const std::string &arg, BlocServer &current, int argNb);

            void        handleRootBlocLocation(const std::string &arg, BlocLocation &current, int argNb);
            void        handleAlias(const std::string &arg, BlocLocation &current, int argNb);
            void        handleIndexBlocLocation(const std::string &arg, BlocLocation &current);
            void        handleAllowMethods(const std::string &arg, BlocLocation &current);
            void        handleUploadPath(const std::string &arg, BlocLocation &current);
            void        handleAutoIndex(const std::string &arg, BlocLocation &current, int argNb);

            // VErif les arg
            bool        isValidRoot(const std::string &path);
            bool        isValidIPv4(const std::string &ip);

            // print
            void        printConfig()const;
};

#endif