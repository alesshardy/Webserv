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

BlocLocation* BlocServer::getMatchingLocation(const std::string& uri) const
{
    BlocLocation* bestMatch = NULL;
    size_t bestMatchLength = 0;

    // Parcourir toutes les locations définies dans le serveur
    for (std::map<std::string, BlocLocation>::const_iterator it = _location.begin(); it != _location.end(); ++it)
    {
        const std::string& locationPath = it->first;

        // Vérifier si l'URI commence par le chemin de la location
        if (uri.find(locationPath) == 0 && 
            (locationPath.size() > bestMatchLength || bestMatch == NULL)) // Trouver le chemin le plus long
        {
            bestMatch = const_cast<BlocLocation*>(&it->second);
            bestMatchLength = locationPath.size();
            LogManager::log(LogManager::DEBUG, ("Matching location found: " + locationPath).c_str());
        }
    }

    // Retourner le meilleur match ou NULL si aucun n'est trouvé
    return bestMatch;
}