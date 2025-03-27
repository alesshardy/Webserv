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

// void    Config::parseConfigFile(const std::string &filePath, Config &config)
// {
//     // verif si fichier existe
//     std::ifstream   file(filePath.c_str());
//     if (!file.is_open())
//     {
//         throw ("Error: Impossible to open file!");
//     }
//     if (file.peek() == EOF)
//     {
//         // gerer plus tard fichier par default car vide
//     }

//     std::string line;
//     std::string param;
//     bool    inServBloc = false;
//     bool    inLocBloc  = false;
//     bool    isOpen = false;

//     //Boucle principale
//     while (std::getline(file, line))
//     {
//         //supp les white space inutile
//         eraseWhiteSpace(line);
//         // ignorer les lignes vides et les commentaires
//         if (line.empty() || line[0] == '#')
//             continue;
//         //
//         while(line)

        
//     }
// }

bool    Config::verifKeyServer(std::string token)
{
    if (token == "server")
        return true;
    return false;
}

bool    Config::verifKeyOther(std::string token)
{
    if (token == "location" || token == "listen" || token == "server_name" || token == "root"
    || token == "index" || token == "error_page" || token == "client_max_body_size" 
    || token == "alias" || token == "allow_methods" || token == "upload_path" || token == "autoindex")
        return true;
    return false;
}

//TEST AJOUT SIMPLE SANS PARSER
void        Config::addArgToServerBloc(std::string arg, std::string lastKey, BlocServer &current)
{
    if (lastKey == "listen")
    {
        current.addListen(arg);
    }
    else if (lastKey == "server_name")
    {
        current.addServerName(arg);
    }
    else if (lastKey == "root")
    {   
        if (!current.getRoot().empty())
            throw std::runtime_error("ERREUR : double root dans bloc server ");    
        current.setRoot(arg);
    }
    else if (lastKey == "index")
    {
        current.addIndex(arg);
    }
    // bourbier quand plusieur element sur meme ligne
    // if (lastKey == "error_page")
    // {
    //     current.addErrorPage(arg);
    // }
    else if (lastKey == "client_max_body_size")
    {
        current.setClientMaxBodySize(Utils::ft_stolonglong(arg));
    }
    else
        throw std::runtime_error("ERREUR : pas la bonne cle pour le bloc serveur ");    
}

void        Config::addArgToLocationBloc(std::string arg, std::string lastKey, BlocLocation &current)
{
    if (lastKey == "root")
    {
        current.setRoot(arg);
    }
    else if (lastKey == "alias")
    {
        current.setAlias(arg);
    }
    else if (lastKey == "index")
    {
        current.addIndex(arg);
    }
    else if (lastKey == "allow_methods")
    {
        current.addAllowMethod(arg);
    }
    else if (lastKey == "upload_path")
    {
        current.setUploadPath(arg);
    }
    else if (lastKey == "autoindex")
    {
        if (arg == "true")
            current.setAutoIndex(true);
        current.setAutoIndex(false);
    }
    else
        throw std::runtime_error("ERREUR : pas la bonne cle pour le bloc location ");
}



void    Config::parseConfigFile(const std::string &filePath, Config &config) 
{
    (void)config;
    std::ifstream file(filePath.c_str());
    if (!file) 
    {
        throw std::runtime_error("Erreur : impossible d'ouvrir le fichier ");
        return;
    }

    char c;
    std::string token;
    bool inComment = false;
    int depth = 0; // Niveau d'imbrication
    int semicolonCount = 0; // Compteur de points-virgules par ligne
    bool contentAfterSemicolon = false; // Indique si du contenu suit un point-virgule sur la même ligne
    bool isKey = true; // Indique si le mot actuel est une clé (premier mot d'une ligne ou d'un bloc)

    bool    emptyBloc = true;
    bool inLocBloc = false;
    bool inLocLine = false;
    std::string lastKey;
    char    lastC;
    BlocLocation    currentLocation;
    BlocServer      currentServer;
    std::string     currentLocationPath;

    while (file.get(c)) 
    {
        // Gestion des commentaires
        if (inComment)
        {
            if (c == '\n')
            {
                inComment = false; // Fin du commentaire
                semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
                contentAfterSemicolon = false; // SERT A RIEN CETTE MERDRE ????
                isKey = true; // Réinitialiser pour la nouvelle ligne
            }
            continue;
        }

        if (c == '#')
        {
            inComment = true; // Début d’un commentaire
            continue;
        }

        // Gestion des sauts de ligne
        if (c == '\n') 
        {
            if (!token.empty()) 
            {
                if (isKey) 
                {
                    std::cout << "Clé trouvé1 : " << token << std::endl;
                    lastKey = token;
                    if (!verifKeyServer(token))
                        throw std::runtime_error("ERREUR : first key must be 'server' ");
                    currentServer = BlocServer();
                } 
                else 
                {
                    std::cout << "Argument trouvé1 : " << token << std::endl;
                }
                // requete doit finir par ';' partie 1
                if (semicolonCount != 1 && token != "server")
                {
                    throw std::runtime_error("ERREUR : ligne requete doit finir par ';' OU premier bloc doit etre server ");
                }
                token.clear(); // Vider le token à la fin de la ligne
            }
            // REGLER CE PUTIN DE PB D"ESPACE DE CON
            if (semicolonCount != 1 && lastKey != "server" && lastKey != "location" && lastC != '\n' && lastC != '}')
            {
                throw std::runtime_error("ERREUR : SVPPPP");
            }
                        semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
            contentAfterSemicolon = false;
            isKey = true; // Réinitialiser pour la nouvelle ligne
            lastC = c;
        } 
        // Gestion des blocs
        else if (c == '{') 
        {
            std::cout << "Début de bloc" << std::endl;
            depth++;
            isKey = true; // Réinitialiser pour le début d'un bloc
            emptyBloc = true;
            if (depth == 2 && inLocBloc == false)
                throw std::runtime_error("Erreur : bloc de lvl 2 doit etre location") ;
            if (depth == 3)
                throw std::runtime_error("Erreur : j'accepte pas les locations imbriquer #inception") ;
            lastC = c;
        } 
        else if (c == '}') 
        {
            std::cout << "Fin de bloc" << std::endl;
            // gerer les blocs vides
            if (emptyBloc)
            {
                throw std::runtime_error("Erreur : le fichier ne peux contenir de bloc vide") ;
            }
            // AJOUT DES BLOC DE CLASS
            if (depth == 2)
            {
                currentServer.addLocation(currentLocationPath, currentLocation);
                currentLocationPath.clear();
            }
            else if (depth == 1)
            {
                config.addServer(currentServer);
            }
            if (inLocBloc == true)
                inLocBloc = false;
            depth--;
            isKey = true; // Réinitialiser pour la fin d'un bloc
            lastC = c;
        } 
        // Gestion des points-virgules
        else if (c == ';') 
        {
            semicolonCount++;
            if (depth == 0)
                throw std::runtime_error("Erreur : directive 'server' has no opening '{'");
            if (semicolonCount > 1) 
            {
                throw std::runtime_error("Erreur : plus d'une requête par ligne détectée.");
            }
            if (!token.empty()) 
            {
                if (isKey) 
                {
                    std::cout << "Clé trouvée2 : " << token << std::endl;
                    lastKey = token;
                } 
                else 
                {
                    std::cout << "Argument trouvé2 : " << token << std::endl;
                    //AJOUT AU CLASS
                    if (depth == 1 && lastKey != "location")
                         addArgToServerBloc(token, lastKey, currentServer);
                     else if (depth == 2)
                         addArgToLocationBloc(token, lastKey, currentLocation);
                }
                token.clear(); // Vider le token après un point-virgule
            }
            else if (isKey == true)
            {
                throw std::runtime_error("Erreur : unexpected ';' ");
            }
            isKey = true; // Réinitialiser pour que le prochain mot soit une clé
            lastC = c;
        }
        // requete doit finir par ';' partie 2
        else if (semicolonCount == 1 && !isspace(c) && c != '\n')
        {
            throw std::runtime_error("Erreur : ligne requete doit finir par ';2' ");
        }
        // Gestion des espaces
        else if (isspace(c)) 
        {
            if (!token.empty()) 
            {
                if (isKey && inLocLine == false) 
                {
                    std::cout << "Clé trouvée3 : " << token << std::endl;
                    lastKey = token;
                    isKey = false; // Le prochain mot sera un argument
                    if (token == "location")
                    {
                        inLocBloc = true;
                        inLocLine = true;
                    }
                    if (!verifKeyOther(token))
                        throw std::runtime_error("ERREUR : key unknown ");
                    
                } 
                else 
                {
                    std::cout << "Argument trouvé3 : " << token << std::endl;
                    inLocLine = false;
                    //AJOUT AU CLASS
                    if (depth == 1 && lastKey != "location")
                        addArgToServerBloc(token, lastKey, currentServer);
                    else if (depth == 1 && lastKey == "location")
                    {
                        currentLocation = BlocLocation();
                        currentLocationPath = token;
                    }
                    else if (depth == 2)
                        addArgToLocationBloc(token, lastKey, currentLocation);
                }
                token.clear();
            }
        } 
        // Construction des mots-clés
        else 
        {
            emptyBloc = false;
            token += c; // Construire le mot clé
            lastC = c;
        }
    }
    if (depth != 0)
        throw std::runtime_error("Erreur : fermeture de bloc incorrect!");

    file.close();
}

/******************************************PRINT********************************************* */

void Config::printConfig() const
{
    std::cout << "========== Configuration ==========" << std::endl;

    for (size_t i = 0; i < _server.size(); ++i)
    {
        const BlocServer &server = _server[i];
        std::cout << "Server " << i + 1 << ":" << std::endl;

        // Afficher les directives du serveur
        std::cout << "  Listen: ";
        for (size_t j = 0; j < server.getListen().size(); ++j)
        {
            std::cout << server.getListen()[j];
            if (j < server.getListen().size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;

        std::cout << "  Server Names: ";
        for (size_t j = 0; j < server.getServerName().size(); ++j)
        {
            std::cout << server.getServerName()[j];
            if (j < server.getServerName().size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;

        std::cout << "  Root: " << server.getRoot() << std::endl;

        std::cout << "  Index: ";
        for (size_t j = 0; j < server.getIndex().size(); ++j)
        {
            std::cout << server.getIndex()[j];
            if (j < server.getIndex().size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;

        std::cout << "  Error Pages: ";
        for (std::map<int, std::string>::const_iterator it = server.getErrorPage().begin(); it != server.getErrorPage().end(); ++it)
        {
            std::cout << it->first << " -> " << it->second << "; ";
        }
        std::cout << std::endl;

        std::cout << "  Client Max Body Size: " << server.getClientMaxBodySize() << std::endl;

        // Afficher les locations
        std::cout << "  Locations:" << std::endl;
        for (std::map<std::string, BlocLocation>::const_iterator it = server.getLocation().begin(); it != server.getLocation().end(); ++it)
        {
            const BlocLocation &location = it->second;
            std::cout << "    Path: " << it->first << std::endl;
            std::cout << "      Root: " << location.getRoot() << std::endl;
            std::cout << "      Alias: " << location.getAlias() << std::endl;

            std::cout << "      Index: ";
            for (size_t j = 0; j < location.getIndex().size(); ++j)
            {
                std::cout << location.getIndex()[j];
                if (j < location.getIndex().size() - 1)
                    std::cout << ", ";
            }
            std::cout << std::endl;

            std::cout << "      Allow Methods: ";
            for (std::set<std::string>::const_iterator methodIt = location.getAllowMethod().begin();
                 methodIt != location.getAllowMethod().end(); ++methodIt)
            {
                std::cout << *methodIt;
                std::set<std::string>::const_iterator nextIt = methodIt;
                ++nextIt; // Incrémenter manuellement l'itérateur
                if (nextIt != location.getAllowMethod().end())
                    std::cout << ", ";
            }
            std::cout << std::endl;

            std::cout << "      Upload Path: " << location.getUploadPath() << std::endl;
            std::cout << "      Auto Index: " << (location.getAutoIndex() ? "on" : "off") << std::endl;
        }

        std::cout << "-----------------------------------" << std::endl;
    }

    std::cout << "===================================" << std::endl;
}
