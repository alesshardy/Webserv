#include "Config.hpp"

// Constructors and destructors
Config::Config()
{
}

Config::Config(const Config &copy)
{
    *this = copy;
}

Config::~Config()
{
}

Config &Config::operator=(const Config &assign)
{
    if (this != &assign)
    {
        _server = assign._server;
    }
    return (*this);
}

//Setters
void Config::addServer(const BlocServer &server)
{
    _server.push_back(server);
}

//Getters
BlocServer &Config::getServer(size_t index)
{
    return (_server[index]);
}

const std::vector<BlocServer> &Config::getServers() const
{
    return (_server);
}


//Parsing

/**
 * @brief supprime les espaces au debut et a la fin d'une ligne
 * 
 * @param line 
 */
void    Config::eraseWhiteSpace(std::string line)
{
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
}
/**
 * @brief permet de recuperer le premier mot d'une ligne en la lisant dans un flux
 * 
 * @param line 
 * @return std::string 
 */
std::string Config::findFirstWord(std::string line)
{
        std::istringstream flux(line);
        std::string word;
        flux >> word;
        return word;
}

void    Config::parseConfigFile(const std::string &filePath, Config &config)
{
    // verif si fichier existe
    std::ifstream   file(filePath.c_str());
    if (!file.is_open())
    {
        throw ("Error: Impossible to open file!");
    }
    if (file.peek() == EOF)
    {
        // gerer plus tard fichier par default car vide
    }

    std::string line;
    std::string param;
    bool    inServBloc = false;
    bool    inLocBloc  = false;
    bool    isOpen = false;

    //Boucle principale
    while (std::getline(file, line))
    {
        //supp les white space inutile
        eraseWhiteSpace(line);
        // ignorer les lignes vides et les commentaires
        if (line.empty() || line[0] == '#')
            continue;
        //
        while(line)

        
    }
}
