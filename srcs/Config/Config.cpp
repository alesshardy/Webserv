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

// FONCTION UTILS AU VERIF DE L'ARG
/**
 * @brief verifie si un chemin donne est un repertoire valid sur le systeme de fichier
 * 
 * @param path 
 * @return true 
 * @return false 
 */
bool        Config::isValidRoot(const std::string &path) // enlever com pour l'utiliser
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
    // Vérifier qu'il y a exactement 4 segments et pas de point final
    if (count != 4 || ip[ip.size() - 1] == '.')
        return false;

    return count == 4; // Doit avoir exactement 4 segments
}

// SOUS FONCTION D'AJOUT AU BLOC SERVEUR && DE PARSING DE L'ARGUMENT DE CHAQUE KEY
void    Config::handleListen(const std::string &arg, BlocServer &current)
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
                throw std::runtime_error("ERROR : arg Listen not correct");
            }
        }
        else
        {
            try {
                port = Utils::ft_stoi(arg);
            }
            catch (const std::exception &e){
                throw std::runtime_error("ERROR : arg Listen not correct");
            }
        }
        if (port < 1 || port > 65535)
            throw std::runtime_error("ERROR : Invalid port number in listen directive.");
        if (!isValidIPv4(ip))
            throw std::runtime_error("ERROR : Invalid ip: "+ ip + " in listen directive.");
        current.addListen(Listen(ip, port));
}

void    Config::handleServerName(const std::string &arg, BlocServer &current)
{
    current.addServerName(arg);
}

void    Config::handleRootBlocServer(const std::string &arg, BlocServer &current, int argNb)
{
    if (argNb > 1)
        throw std::runtime_error("ERROR : too many argument for root key");
    if (!current.getRoot().empty())
        throw std::runtime_error("ERROR : double Root in server bloc");
    if (!isValidRoot(arg))
        throw std::runtime_error("ERROR : root must be an absolute path");   
    current.setRoot(arg);
}

void    Config::handleIndexBlocServer(const std::string &arg, BlocServer &current)
{
    current.addIndex(arg);
}

void    Config::handleErrorPage(const std::string &arg, BlocServer &current, int argNb)
{
    //Choisi d'ecraser si plusieurs code erreur car nginx accepte doublon
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

void Config::handleClientMaxBodySize(const std::string &arg, BlocServer &current, int argNb)
{
    std::string numberPart;
    char unit = '\0';
    
    if (argNb > 1)
        throw std::runtime_error("ERROR : invalid number of arguments in client_max_body_size directive");
    
    for (size_t i = 0; i < arg.size(); ++i)
    {
        if (std::isdigit(arg[i]))
            numberPart += arg[i];
        else
        {
            unit = arg[i];
            // ajout verif si caractere apres l'unit
            if (arg[++i] != '\0')
                throw std::runtime_error("ERROR : client_max_body_size character after unit");
            break;
        }   
    }

    long long value;
    try {
        value = Utils::ft_stolonglong(numberPart);
    }
    catch (const std::exception &e){
        throw std::runtime_error("ERROR : client_max_body_size key incompatible value");
    }

    if (unit == 'k' || unit == 'K')
    {
        if (value > LLONG_MAX / 1024)
            throw std::runtime_error("ERROR : client_max_body_size value is too large (overflow)");
        value *= 1024;
    }
    else if (unit == 'm' || unit == 'M')
    {
        if (value > LLONG_MAX / (1024 * 1024))
            throw std::runtime_error("ERROR : client_max_body_size value is too large (overflow)");
        value *= 1024 * 1024;
    }
    else if (unit == 'g' || unit == 'G')
    {
        if (value > LLONG_MAX / (1024 * 1024 * 1024))
            throw std::runtime_error("ERROR : client_max_body_size value is too large (overflow)");
        value *= 1024 * 1024 * 1024;
    }
    else if (unit != '\0')
        throw std::runtime_error("ERROR : invalid unit for client_max_body_size (accepted: k, m, g)");
    if (value <= 0)
            throw std::runtime_error("ERROR : client_max_body_size must be a positive value");
    current.setClientMaxBodySize(value);
}
// SOUS FONCTION D'AJOUT AU BLOC LOCATION && DE PARSING DE L'ARGUMENT DE CHAQUE KEY
void    Config::handleRootBlocLocation(const std::string &arg, BlocLocation &current, int argNb)
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

void    Config::handleAlias(const std::string &arg, BlocLocation &current, int argNb)
{
    if (argNb > 1)
        throw std::runtime_error("ERROR : too many argument for alias key");
    if (!current.getAlias().empty())
        throw std::runtime_error("ERROR : double alias in location bloc");
    if (!current.getRoot().empty())
        throw std::runtime_error("ERROR : can't have a alias if already an root");
    current.setAlias(arg);
}

void    Config::handleIndexBlocLocation(const std::string &arg, BlocLocation &current)
{
    current.addIndex(arg);
}

void    Config::handleAllowMethods(const std::string &arg, BlocLocation &current)
{
    const std::set<std::string> &methods = current.getAllowMethod();
    if (methods.find(arg) != methods.end())
        throw std::runtime_error("ERROR : Method '" + arg + "' already define");
    if (arg == "GET" || arg == "POST" || arg == "DELETE")
        current.addAllowMethod(arg);
    else
        throw std::runtime_error("ERROR : bad argument for allow_methods key");
}

void    Config::handleUploadPath(const std::string &arg, BlocLocation &current)
{
    current.setUploadPath(arg);
}

void    Config::handleAutoIndex(const std::string &arg, BlocLocation &current, int argNb)
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

// MAIN FONCTION D'AJOUT AU BLOC SERVEUR
void        Config::addArgToServerBloc(std::string arg, std::string lastKey, BlocServer &current, int argNb)
{
    if (lastKey == "listen")
        handleListen(arg, current);
    else if (lastKey == "server_name")
        handleServerName(arg, current);
    else if (lastKey == "root")
        handleRootBlocServer(arg, current, argNb);
    else if (lastKey == "index")
        handleIndexBlocServer(arg, current);
    else if (lastKey == "error_page")
        handleErrorPage(arg, current, argNb);
    else if (lastKey == "client_max_body_size")
        handleClientMaxBodySize(arg, current, argNb);
    else
        throw std::runtime_error("ERROR : bad key " + lastKey + " for server bloc ");    
}

void        Config::addArgToLocationBloc(std::string arg, std::string lastKey, BlocLocation &current, int argNb)
{
    if (lastKey == "root")
        handleRootBlocLocation(arg, current, argNb);
    else if (lastKey == "alias")
        handleAlias(arg, current, argNb);
    else if (lastKey == "index")
        handleIndexBlocLocation(arg, current);
    else if (lastKey == "allow_methods")
        handleAllowMethods(arg, current);
    else if (lastKey == "upload_path")
        handleUploadPath(arg, current);
    else if (lastKey == "autoindex")
        handleAutoIndex(arg, current, argNb);
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
                    LogManager::log(LogManager::DEBUG, ("Clé trouvé1 : " + token).c_str());
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
                    LogManager::log(LogManager::DEBUG, ("Argument trouvé1 : " + token).c_str());
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
            isKey = true; // Réinitialiser pour la nouvelle ligne
            argNb = 0;
            lastC = c;
        } 
        // Gestion des blocs
        else if (c == '{') 
        {
            LogManager::log(LogManager::DEBUG, ("Début de bloc"));
            depth++;
            isKey = true; // Réinitialiser pour le début d'un bloc
            emptyBloc = true;
            //FIX
            if (verifKeyServer(token))
            {
                LogManager::log(LogManager::DEBUG, ("Cle trouvee4 : " + token).c_str());
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
            LogManager::log(LogManager::DEBUG, ("Fin de bloc"));
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
                    LogManager::log(LogManager::DEBUG, ("Clé trouvée2 : " + token).c_str());
                    lastKey = token;
                } 
                else 
                {
                    LogManager::log(LogManager::DEBUG, ("Argument trouvé2 : " + token).c_str());
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
                    LogManager::log(LogManager::DEBUG, ("Clé trouvée3 : " + token).c_str());
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
                    LogManager::log(LogManager::DEBUG, ("Argument trouvé3 : " + token).c_str());
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
    LogManager::log(LogManager::DEBUG, ("PRINT PARSE CONFIGURATION :"));
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
