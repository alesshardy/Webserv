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

// VERIF KEY
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

//VERIF ARG

/**
 * @brief verifie si un chemin donne est un repertoire valid sur le systeme de fichier
 * 
 * @param path 
 * @return true 
 * @return false 
 */
bool        Config::isValidRoot(const std::string &path)
{
    // struct stat info;
    // return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
    (void)path;
    return true;
}

bool Config::isValidIPv4(const std::string &ip)
{
    std::istringstream iss(ip);
    std::string segment;
    int count = 0;

    while (std::getline(iss, segment, '.'))
    {
        count++;
        if (count > 4) // Trop de segments
            return false;

        if (segment.empty() || segment.size() > 3) // Segment vide ou trop long
            return false;

        // Vérifier que chaque caractère est un chiffre
        for (std::string::size_type i = 0; i < segment.size(); ++i)
        {
            if (!std::isdigit(segment[i]))
                return false;
        }

        // Convertir le segment en entier
        int value;
        std::istringstream segmentStream(segment);
        segmentStream >> value;

        if (value < 0 || value > 255) // Hors plage
            return false;
    }

    return count == 4; // Doit avoir exactement 4 segments
}

//TEST AJOUT SIMPLE SANS PARSER
void        Config::addArgToServerBloc(std::string arg, std::string lastKey, BlocServer &current, int argNb)
{
    if (lastKey == "listen")
    {
        size_t colonPos = arg.find(':');
        std::string ip = "0.0.0.0"; 
        int port;
        if (colonPos != std::string::npos)
        {
            ip = arg.substr(0, colonPos);
            try { port = Utils::ft_stoi(arg.substr(colonPos + 1));
            }
            catch (const std::exception &e){
                throw std::runtime_error("ERROR : arg Listen not correct");// WTF PK ca display le msg que je veux
            }
        }
        else
        {
            try {
                port = Utils::ft_stoi(arg);
            }
            catch (const std::exception &e){
                throw std::runtime_error("ERROR : arg Listen not correct");// WTF PK ca display le msg que je veux
            }
        }
        if (port < 1 || port > 65535)
            throw std::runtime_error("ERROR : Invalid port number in listen directive.");
        if (!isValidIPv4(ip))
            throw std::runtime_error("ERROR : Invalid ip: "+ ip + " in listen directive.");
        current.addListen(Listen(ip, port));
    }
    else if (lastKey == "server_name")
    {
        current.addServerName(arg);
    }
    else if (lastKey == "root")
    {
        if (argNb > 1)
            throw std::runtime_error("ERROR : too many argument for root key");
        if (!current.getRoot().empty())
            throw std::runtime_error("ERROR : double Root in server bloc");
        if (!isValidRoot(arg))
            throw std::runtime_error("ERROR : root must be an absolute path");   
        current.setRoot(arg);
    }
    else if (lastKey == "index")
    {
        current.addIndex(arg);
    }
    //choisi d'ecraser si plusieur code erreur car nginx accepte doublon
    else if (lastKey == "error_page")
    {
        if (argNb == 1)
        {
            int code ;
            try {
                code = Utils::ft_stoi(arg);
            }
            catch (const std::exception &e){
                throw std::runtime_error("ERROR : wrong error_page code");   
            }
            if (code >= 300 && code <= 599)
                current.setTmpErrorCode(code);
            else
                throw std::runtime_error("ERROR : wrong error_page code must be between 300 && 599");   
        }
        else if (argNb == 2)
        {
            if (!current.hasTmpErrorCode())
                throw std::runtime_error("ERROR : no code define before arg");
            int code = current.getTmpErrorCode();
            current.addErrorPage(code, arg);
            current.clearTmpErrorCode();
        }
        else
            throw std::runtime_error("ERROR : too many arg for error_page");   
    }
    else if (lastKey == "client_max_body_size")
    {
        current.setClientMaxBodySize(Utils::ft_stolonglong(arg));
    }
    else
        throw std::runtime_error("ERROR : bad key " + lastKey + " for server bloc ");    
}

void        Config::addArgToLocationBloc(std::string arg, std::string lastKey, BlocLocation &current, int argNb)
{
    if (lastKey == "root")
    {
        if (argNb > 1)
            throw std::runtime_error("ERROR : too many argument for root key");
        if (!current.getRoot().empty())
            throw std::runtime_error("ERROR : double root in location bloc");
        if (!current.getAlias().empty())
            throw std::runtime_error("ERROR : can't have a root if already an alias");
        if (!isValidRoot(arg))
            throw std::runtime_error("ERROR : root must be an absolute path");
        current.setRoot(arg);
    }
    else if (lastKey == "alias")
    {  
        if (!current.getAlias().empty())
            throw std::runtime_error("ERROR : double alias in location bloc");
        if (!current.getRoot().empty())
            throw std::runtime_error("ERROR : can't have a alias if already an root");
        current.setAlias(arg);
    }
    else if (lastKey == "index")
    {
        current.addIndex(arg);
    }
    else if (lastKey == "allow_methods")
    {
        const std::set<std::string> &methods = current.getAllowMethod();
        if (methods.find(arg) != methods.end())
            throw std::runtime_error("ERROR : Method '" + arg + "' already define");
        if (arg == "GET" || arg == "POST" || arg == "DELETE")
            current.addAllowMethod(arg);
        else
            throw std::runtime_error("ERROR : bad argument for allow_methods key");
    }
    else if (lastKey == "upload_path")
    {
        current.setUploadPath(arg);
    }
    else if (lastKey == "autoindex")
    {   
        if (argNb > 1)
            throw std::runtime_error("ERROR :too many argument for autoindex key");
        if (arg == "on")
            current.setAutoIndex(true);
        else if (arg == "off")
            current.setAutoIndex(false);
        else 
            throw std::runtime_error("ERROR : bad argument for autoindex key");
    }
    else
        throw std::runtime_error("ERROR : bad key for location bloc");
}

//A decouper fonction principale pour verifier que le fichier config est correct
void    Config::parseConfigFile(const std::string &filePath, Config &config) 
{
    (void)config;
    std::ifstream file(filePath.c_str());
    if (!file) 
    {
        throw std::runtime_error("ERROR : impossible to open file");
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
    int    argNb = 0;

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
                        throw std::runtime_error("ERROR : first key must be 'server' ");
                    // FIX
                    if (depth != 0)
                        throw std::runtime_error("ERROR : server key inside a server bloc ");
                    currentServer = BlocServer(); //SIUUU
                } 
                else 
                {
                    std::cout << "Argument trouvé1 : " << token << std::endl;
                    argNb++;
                }
                // requete doit finir par ';' partie 1
                if (semicolonCount != 1 && token != "server")
                {
                    throw std::runtime_error("ERROR : line request must end by ';' OR first bloc must be server ");
                }
                token.clear(); // Vider le token à la fin de la ligne
            }

            // REGLER CE PUTIN DE PB D"ESPACE DE CON
            if (semicolonCount != 1 && lastKey != "server" && lastKey != "location" && lastC != '\n' && lastC != '}' && lastKey != "")
            {
                throw std::runtime_error("ERROR : line request must end by ';'");
            }
            semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
            contentAfterSemicolon = false;
            isKey = true; // Réinitialiser pour la nouvelle ligne
            argNb = 0;
            lastC = c;
        } 
        // Gestion des blocs
        else if (c == '{') 
        {
            std::cout << "Début de bloc" << std::endl;
            depth++;
            isKey = true; // Réinitialiser pour le début d'un bloc
            emptyBloc = true;
            //FIX
            if (verifKeyServer(token))
            {
                std::cout << "Cle trouvee4 : "<< token << std::endl;
                lastKey = token;
                token.clear();
            }

            if (depth == 2 && inLocBloc == false)
                throw std::runtime_error("ERROR : bloc of lvl 2 must be location") ;
            if (depth == 3)
                throw std::runtime_error("ERROR : we don't accept location in location were are not in inception") ;
            lastC = c;
        } 
        else if (c == '}') 
        {
            std::cout << "Fin de bloc" << std::endl;
            // gerer les blocs vides
            if (emptyBloc)
            {
                throw std::runtime_error("ERROR : file can't have empty bloc") ;
            }
            // AJOUT DES BLOC DE CLASS
            if (depth == 2)
            {
                if (currentServer.locationExists(currentLocationPath))
                    throw std::runtime_error("ERROR : Location path '" + currentLocationPath + "' already exists in the server block.");
                currentServer.addLocation(currentLocationPath, currentLocation);
                currentLocationPath.clear();
            }
            else if (depth == 1)
            {
                config.addServer(currentServer);
                currentServer = BlocServer();
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
                throw std::runtime_error("ERROR : directive 'server' has no opening '{'");
            if (semicolonCount > 1) 
            {
                throw std::runtime_error("ERROR : multiple request in one line");
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
                    argNb++;
                    if (depth == 1 && lastKey != "location")
                         addArgToServerBloc(token, lastKey, currentServer, argNb);
                     else if (depth == 2)
                         addArgToLocationBloc(token, lastKey, currentLocation, argNb);
                }
                token.clear(); // Vider le token après un point-virgule
            }
            else if (isKey == true)
            {
                throw std::runtime_error("ERROR : unexpected ';' ");
            }
            isKey = true; // Réinitialiser pour que le prochain mot soit une clé
            lastC = c;
        }
        // requete doit finir par ';' partie 2
        else if (semicolonCount == 1 && !isspace(c) && c != '\n')
        {
            throw std::runtime_error("ERROR : line request must end by ';' ");
        }
        // Gestion des espaces
        else if (isspace(c)) 
        {
            if (!token.empty()) 
            {
                if (isKey && inLocLine == false) 
                {
                    std::cout << "Clé trouvée3 : " << token << std::endl;
                    // FIX location sans {
                    if (lastKey == "location" && depth != 2)
                        throw std::runtime_error("ERROR : if location key must open a bloc");

                    lastKey = token;
                    isKey = false; // Le prochain mot sera un argument
                    if (token == "location")
                    {
                        inLocBloc = true;
                        inLocLine = true;
                    }
                    // FIX
                    if (verifKeyServer(token) && depth == 0)
                    {
                        token.clear();
                        continue;
                    }

                    if (!verifKeyOther(token))
                        throw std::runtime_error("ERROR : key unknown ");
                } 
                else 
                {
                    std::cout << "Argument trouvé3 : " << token << std::endl;
                    inLocLine = false;
                    //AJOUT AU CLASS
                    argNb++;
                    if (depth == 1 && lastKey != "location")
                        addArgToServerBloc(token, lastKey, currentServer, argNb);
                    else if (depth == 1 && lastKey == "location")
                    {
                        currentLocation = BlocLocation();
                        currentLocationPath = token;
                    }
                    else if (depth == 2)
                        addArgToLocationBloc(token, lastKey, currentLocation, argNb);
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
        throw std::runtime_error("ERROR : a bloc is not close!");

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
            const Listen &listen = server.getListen()[j];
            std::cout << listen.getIp() << ":" << listen.getPort();
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
