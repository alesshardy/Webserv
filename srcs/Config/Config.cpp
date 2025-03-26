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

void Config::parseConfigFile(const std::string &filePath, Config &config) 
{
    (void)config;
    std::ifstream file(filePath.c_str());
    if (!file) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << filePath << std::endl;
        return;
    }

    char c;
    std::string token;
    bool inComment = false;
    int depth = 0; // Niveau d'imbrication
    int semicolonCount = 0; // Compteur de points-virgules par ligne
    bool contentAfterSemicolon = false; // Indique si du contenu suit un point-virgule sur la même ligne
    bool isKey = true; // Indique si le mot actuel est une clé (premier mot d'une ligne ou d'un bloc)

    // bool inLocBloc = false;
    // bool inServBloc = false;
    // BlocLocation    currentLocation;
    // BlocServer      currentServer;


    while (file.get(c)) {
        // Gestion des commentaires
        if (inComment) {
            if (c == '\n') {
                inComment = false; // Fin du commentaire
                semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
                contentAfterSemicolon = false; // SERT A RIEN CETTE MERDRE ????
                isKey = true; // Réinitialiser pour la nouvelle ligne
            }
            continue;
        }

        if (c == '#') {
            inComment = true; // Début d’un commentaire
            continue;
        }

        // Gestion des sauts de ligne
        if (c == '\n') {
            if (!token.empty()) {
                if (isKey) {
                    std::cout << "Clé trouvée : " << token << std::endl;

                } else {
                    std::cout << "Argument trouvé : " << token << std::endl;
                }
                token.clear(); // Vider le token à la fin de la ligne
            }
            semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
            contentAfterSemicolon = false;
            isKey = true; // Réinitialiser pour la nouvelle ligne
        } 
        // Gestion des blocs
        else if (c == '{') {
            std::cout << "Début de bloc" << std::endl;
            depth++;
            isKey = true; // Réinitialiser pour le début d'un bloc
        } else if (c == '}') {
            std::cout << "Fin de bloc" << std::endl;
            depth--;
            isKey = true; // Réinitialiser pour la fin d'un bloc
        } 
        // Gestion des points-virgules
        else if (c == ';') {
            semicolonCount++;
            if (semicolonCount > 1) {
                std::cerr << "Erreur : plus d'une requête par ligne détectée." << std::endl;
                return;
            }
            if (!token.empty()) {
                if (isKey) {
                    std::cout << "Clé trouvée : " << token << std::endl;
                } else {
                    std::cout << "Argument trouvé : " << token << std::endl;
                }
                token.clear(); // Vider le token après un point-virgule
            }
            isKey = true; // Réinitialiser pour que le prochain mot soit une clé
        }
        // requete doit finir par ';'
        else if (semicolonCount == 1 && !isspace(c) && c != '\n')
        {
            std::cerr << "Erreur : ligne requete doit finir par ';' " << std::endl;
            return ;
        } 
        // Gestion des espaces
        else if (isspace(c)) {
            if (!token.empty()) {
                if (isKey) {
                    std::cout << "Clé trouvée : " << token << std::endl;
                    isKey = false; // Le prochain mot sera un argument
                } else {
                    std::cout << "Argument trouvé : " << token << std::endl;
                }
                token.clear();
            }
        } 
        // Construction des mots-clés
        else {
            token += c; // Construire le mot clé
        }
    }

    file.close();
}
