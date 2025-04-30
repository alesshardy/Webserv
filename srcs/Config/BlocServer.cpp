#include "BlocServer.hpp"

//Constructors and destructors
BlocServer::BlocServer()
{
    _clientMaxBodySize = DEFAULT_CLIENT_MAX_BODY_SIZE;
    _hasTmpErrorCode = false;
}

BlocServer::BlocServer(const BlocServer &copy)
{
    *this = copy;
}

BlocServer::~BlocServer()
{
}

BlocServer &BlocServer::operator=(const BlocServer &assign)
{
    if (this != &assign)
    {
        _listen = assign._listen;
        _serverName = assign._serverName;
        _root = assign._root;
        _index = assign._index;
        _errorPage = assign._errorPage;
        _clientMaxBodySize = assign._clientMaxBodySize;
        _location = assign._location;
        _returnDirectives = assign._returnDirectives;
    }
    return (*this);
}

//Setters
void    BlocServer::addLocation(const std::string &path, const BlocLocation &location) 
{
    _location[path] = location;
};

//Validation
bool BlocServer::validateServer() const
{
    return (!_listen.empty());
}

bool BlocServer::locationExists(const std::string &locationPath) const
{
    return _location.find(locationPath) != _location.end();
}

int	BlocServer::_checkPathsMatch(const std::string &path, const std::string &parentPath) const
{
    size_t	pathSize = path.size();
    size_t	parentPathSize = parentPath.size();
    if (path.compare(0, parentPathSize, parentPath) == 0)
        if (pathSize== parentPathSize || path[parentPathSize] == '/' || parentPath == "/")
            return (1);
    return (0);
}

BlocLocation* BlocServer::getMatchingLocation(const std::string& uri) const
{
    BlocLocation* bestMatch = NULL;
    size_t bestMatchLength = 0;

    // Parcourir toutes les locations définies dans le serveur
    for (std::map<std::string, BlocLocation>::const_iterator it = _location.begin(); it != _location.end(); ++it)
    {
        const std::string& locationPath = it->first;

        // Utiliser _checkPathsMatch pour vérifier si l'URI correspond au chemin de la location
        if (_checkPathsMatch(uri, locationPath))
        {
            // Trouver le chemin le plus long correspondant
            if (locationPath.size() > bestMatchLength || bestMatch == NULL)
            {

                bestMatch = const_cast<BlocLocation*>(&it->second);
                bestMatchLength = locationPath.size();
                LogManager::log(LogManager::DEBUG, ("Matching location found: " + locationPath).c_str());
            }
        }
    }

    // Retourner le meilleur match ou NULL si aucun n'est trouvé
    return bestMatch;
}
