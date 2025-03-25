#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>

class Config
{
    private:
            std::vector<BlocServer>    _server;

    public:
};

class BlocLocation
{
    private:
            std::string                 _root;
            std::string                 _alias;
            std::vector<std::string>    _index;
            std::vector<std::string>    _allowMethod; //une seul clef mais plusieur valeur en face
            std::vector<std::string>    _cgiExtension;
            std::string                 _uploadPath;
            bool                        _autoIndex;
            
    public:
};

class BlocServer
{
    private:
            std::vector<int>            _listen;
            std::vector<std::string>    _serverName;
            std::string                 _root;
            std::vector<std::string>    _index;
            std::map<int, std::string>    _errorPage;
            long long                   _clientMaxBodySize;
            std::map<std::string, BlocLocation> _location;
    
    public:
};

#endif