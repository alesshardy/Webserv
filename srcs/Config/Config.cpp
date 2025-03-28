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
    // BlocLocation    currentLocation;
    // BlocServer      currentServer;

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
                if (isKey) {
                    std::cout << "Clé trouvé1 : " << token << std::endl;
                    lastKey = token;
                    if (!verifKeyServer(token))
                        throw std::runtime_error("ERREUR : first key must be 'server' ");
                } else {
                    std::cout << "Argument trouvé1 : " << token << std::endl;
                }
                // requete doit finir par ';' partie 1
                if (semicolonCount != 1 && token != "server")
                {
                    throw std::runtime_error("ERREUR : ligne requete doit finir par ';' OU premier bloc doit etre server ");
                }
                token.clear(); // Vider le token à la fin de la ligne
            }

            // // REGLER CE PUTIN DE PB D"ESPACE DE CON
            // if (semicolonCount != 1 && lastKey != "server" && lastKey != "location")
            // {
            //     std::cout << "SIUUU " << lastKey << "nb "<< semicolonCount << std::endl;//SIUUU
            //     throw std::runtime_error("ERREUR : SVPPPP");
            // }
            semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
            contentAfterSemicolon = false;
            isKey = true; // Réinitialiser pour la nouvelle ligne
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
        } 
        else if (c == '}') 
        {
            std::cout << "Fin de bloc" << std::endl;
            // gerer les blocs vides
            if (emptyBloc)
            {
                throw std::runtime_error("Erreur : le fichier ne peux contenir de bloc vide") ;
            }
            if (inLocBloc == true)
                inLocBloc = false;
            depth--;
            isKey = true; // Réinitialiser pour la fin d'un bloc
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
                }
                token.clear(); // Vider le token après un point-virgule
            }
            else if (isKey == true)
            {
                throw std::runtime_error("Erreur : unexpected ';' ");
            }
            isKey = true; // Réinitialiser pour que le prochain mot soit une clé
        }
        // requete doit finir par ';' partie 2
        else if (semicolonCount == 1 && !isspace(c) && c != '\n')
        {
            throw std::runtime_error("Erreur : ligne requete doit finir par ';2' ");
            return ;
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
                }
                token.clear();
            }
        } 
        // Construction des mots-clés
        else 
        {
            emptyBloc = false;
            token += c; // Construire le mot clé
        }
    }
    if (depth != 0)
        throw std::runtime_error("Erreur : fermeture de bloc incorrect!");

    file.close();
}
