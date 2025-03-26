#include "BlocServer.hpp"

//Constructors and destructors
BlocServer::BlocServer()
{
    _clientMaxBodySize = DEFAULT_CLIENT_MAX_BODY_SIZE;
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

