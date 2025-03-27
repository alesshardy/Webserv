#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>
# include <set>
# include <fstream>
# include <sstream>
# include "BlocServer.hpp"
# include "Utils.hpp"

class BlocServer;

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
            void        eraseWhiteSpace(std::string line);
            std::string findFirstWord(std::string line);
            bool        verifKeyServer(std::string token);
            bool        verifKeyOther(std::string token);
};

#endif