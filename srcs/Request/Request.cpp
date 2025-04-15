/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apintus <apintus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 13:23:15 by tpassin           #+#    #+#             */
/*   Updated: 2025/04/14 18:32:35 by apintus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(Client *client, Server *server): _client(client), _server(server), _raw(""), _method(""), _uri(""), _version(""), _path(""), _query(""), _currentHeaderKey(""), _statusCode(-1), _state(0), _i(0), _isChunked(false), _maxBodySize(DEFAULT_CLIENT_MAX_BODY_SIZE), _contentLength(0), _body(){}

Request::Request(Request const & copy)
{
    *this = copy;
}

Request & Request::operator=(Request const & rhs)
{
    if (this != &rhs)
    {
        this->_client = rhs._client;
        this->_server = rhs._server;
        this->_raw = rhs._raw;
        this->_method = rhs._method;
        this->_uri = rhs._uri;
        this->_version = rhs._version;
        this->_path = rhs._path;
        this->_query = rhs._query;
        this->_headers = rhs._headers;
        this->_currentHeaderKey = rhs._currentHeaderKey;
        this->_statusCode = rhs._statusCode;
        this->_state = rhs._state;
        this->_i = rhs._i;
        this->_isChunked = rhs._isChunked;
        this->_maxBodySize = rhs._maxBodySize;
        this->_contentLength = rhs._contentLength;
        this->_body = rhs._body;
    }
    return (*this);
}

Request::~Request(){}

const std::string &Request::getMethod() const
{
    return (this->_method);
}

const std::string &Request::getUri() const
{
    return (this->_uri);
}

const std::string &Request::getVersion() const
{
    return (this->_version);
}

const std::string &Request::getPath() const
{
    return (this->_path);
}

const int &Request::getStatusCode() const
{
    return (this->_statusCode);
}

const std::map<std::string, std::string> &Request::getHeaders() const
{
    return (this->_headers);
}

const std::string &Request::getQuery() const
{
    return (this->_query);
}

void Request::setCode(int const code){this->_statusCode = code;}

const int &Request::getState() const
{
    return (_state);
}

void Request::parseRequest(std::string str)
{
    try
    {
        this->_raw += str;

        if (_state == START) 
            parseMethod();
        if (_state == URI) 
            parseUri();
        // if (_state == ) QUERY:
        //     parseQuery(_state, _raw);
        if (_state == VERSION) 
            parseVersion();
        // parse header qui boucle entre key et value jusqu'au body
        if (_state == HEADER_KEY || _state == HEADER_VALUE)
            parseHeader();
        if (_state == HEADER_CHECK)
            checkHeader();
        if (_state == BODY)
            parseBody();
    }
    catch (const std::runtime_error &e)
    {
        LogManager::log(LogManager::ERROR, e.what());
        throw; // Rethrow the exception if needed
    }
}

// Version qui clean au fur et a mesure
// void Request::parseRequest(std::string str)
// {
//     try
//     {
//         this->_raw += str;

//         if (_state == START) 
//         {
//             parseMethod();
//             // Ne pas nettoyer ici car parseUri et parseVersion continueront sur la même ligne
//         }
//         if (_state == URI) 
//         {
//             parseUri();
//             // Ne pas nettoyer ici pour la même raison
//         }
//         if (_state == VERSION) 
//         {
//             parseVersion();
//             // Maintenant on peut nettoyer la première ligne
//             size_t end_of_request_line = _i;
//             clearProcessedData(end_of_request_line);
//         }
//         if (_state == HEADER_KEY || _state == HEADER_VALUE)
//         {
//             // size_t start_idx = _i;
//             parseHeader();
//             // Nettoyer les headers traités sauf si on est en attente de plus de données
//             if (_state != HEADER_KEY && _state != HEADER_VALUE)
//             {
//                 clearProcessedData(_i);
//             }
//         }
//         if (_state == HEADER_CHECK)
//             checkHeader();
//         if (_state == BODY)
//         {
//             size_t start_idx = _i;
//             parseBody();
//             // Si le body est complet ou non-existant, on peut tout nettoyer
//             if (_state == END)
//             {
//                 _raw.clear();
//                 _i = 0;
//             }
//             // Si le parsing du body est en cours, nettoyer ce qui a été traité
//             else if (_i > start_idx)
//             {
//                 clearProcessedData(start_idx);
//             }
//         }
//     }
//     catch (const std::runtime_error &e)
//     {
//         LogManager::log(LogManager::ERROR, e.what());
//         throw;
//     }
// }

void Request::parseMethod()
{
    // LogManager::log(LogManager::DEBUG, "Parse method");
    if (_raw.empty())
        throw std::runtime_error("ERROR : empty request");

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    while (_i < endLine && _raw[_i] != ' ')
    {
        _method += _raw[_i];
        _i++;
    }

    LogManager::log(LogManager::DEBUG, ("Http method: " + _method).c_str());

    if (_method != "GET" && _method != "POST" && _method != "DELETE" && _method != "PUT")
        throw std::runtime_error("ERROR : unsupported HTTP method");

    while (_i < endLine && _raw[_i] == ' ')
        _i++;

    if (_i >= endLine)
        throw std::runtime_error("ERROR: Missing URI after method");

    _state = URI;
}

void Request::parseUri()
{
    // LogManager::log(LogManager::DEBUG, "Parse uri");
    while (_i < _raw.size() && _raw[_i] == ' ')
        _i++;

    if (_i >= _raw.size() || _raw[_i] != '/')
        throw std::runtime_error("ERROR: Uri unknown format");

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    while (_i < endLine && _raw[_i] != ' ')
    {
        _uri += _raw[_i];
        _i++;
    }

    while (_i < endLine && _raw[_i] == ' ')
        _i++;

    if (_i >= endLine)
        throw std::runtime_error("ERROR: Missing Version after uri");
    
    LogManager::log(LogManager::DEBUG, ("Http uri: " + _uri).c_str());
    // SECU
    if (_uri.size() > URI_MAX_SIZE)
        throw std::runtime_error("ERROR: URI size exceeds 2048 characters");

    _state = VERSION;
}

void Request::parseVersion()
{
    // LogManager::log(LogManager::DEBUG, "Parse version");
    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        throw std::runtime_error("ERROR : invalid request format");

    _version = _raw.substr(_i, endLine - _i);

    _i = endLine + 2; // Passer "\r\n"
    if (_version != "HTTP/1.1")
        throw std::runtime_error("ERROR: Bad Version");
    LogManager::log(LogManager::DEBUG, ("Http version: " + _version).c_str());
    // SECU 
    if (_raw.size() > 8192)
        throw std::runtime_error("ERROR: Request line exceeds 8 KB");
    _state = HEADER_KEY;
}

// NOUVELLE VERSION 
void Request::parseHeaderKey()
{
    // LogManager::log(LogManager::DEBUG, "Parse HeaderKey");
    if (_raw.empty())
        throw std::runtime_error("ERROR : empty request");
    
    // size_t endLine = _raw.find("\r\n", _i);
    // if (endLine != std::string::npos)
    if (_raw[_i] == '\r' && _raw[_i + 1] == '\n')
    {
        // if (this->_method == "GET")
        //     _state = END; // SUITE CHANGE EN BODY
        // else if (this->_method == "PUT" || this->_method == "POST")
        //     _state = BODY;
        _state = HEADER_CHECK;
        return ;
    }

    size_t colonPos = _raw.find(":", _i);
    if (colonPos == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    while (_i < colonPos)
    {
        _currentHeaderKey += _raw[_i];
        _i++;
    }
    _i += 2;
    LogManager::log(LogManager::DEBUG, ("HeaderKey " + _currentHeaderKey).c_str());

    _state = HEADER_VALUE;
}

void Request::parseHeaderValue()
{
    // LogManager::log(LogManager::DEBUG, "Parse HeaderValue");
    if (_raw.empty())
        throw std::runtime_error("ERROR : empty request");

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        return ;    
        // throw std::runtime_error("ERROR : invalid request format");
    std::string _currentHeaderValue;
    while (_i < endLine)
    {
        _currentHeaderValue += _raw[_i];
        _i++;
    }

    _i += 2;
    // _headers[_currentHeaderKey] = _currentHeaderValue;
    parseHeaderKeyValue(_currentHeaderKey, _currentHeaderValue);
    LogManager::log(LogManager::DEBUG, ("HeaderValue " + _currentHeaderValue).c_str());
    _currentHeaderKey.clear();
    _state = HEADER_KEY;
}

void   Request::parseHeader()
{
    int boucleK = 0;
    int boucleV = 0;

    while (_state >= HEADER_KEY && _state <= HEADER_VALUE)
    {
        if (_state == HEADER_KEY)
        {
            parseHeaderKey();
            boucleK++;
            boucleV = 0;
        }
        if (_state == HEADER_VALUE)
        {
            parseHeaderValue();
            boucleV++;
            boucleK = 0;
        }
        if (boucleK > 3 || boucleV > 3)
            break;
    }
}


void Request::parseHeaderKeyValue(const std::string& headerKey, const std::string& headerValue)
{
    // Vérifier si l'en-tête est unique
    if (headerKey == "Host" || headerKey == "Content-Length" || headerKey == "Transfer-Encoding")
    {
        if (_headers.find(headerKey) != _headers.end())
        {
            throw std::runtime_error("ERROR: Duplicate header: " + headerKey);
        }
        _headers[headerKey] = headerValue;
    }

    // VOIR POUR LES AUTRES SI ECRASER OU CONCATENER
    
    // // Gérer les en-têtes multiples autorisés
    // else if (headerKey == "Accept" || headerKey == "Accept-Encoding" || headerKey == "Cookie")
    // {
    //     if (_headers.find(headerKey) != _headers.end())
    //     {
    //         _headers[headerKey] += ", " + headerValue;
    //     }
    //     else
    //     {
    //         _headers[headerKey] = headerValue;
    //     }
    // }
    // En-têtes génériques
    else
    {
        _headers[headerKey] = headerValue;
    }
}


// CHECK HEADER
void    Request::checkHeader()
{
    LogManager::log(LogManager::DEBUG, "Checking headers ...");

    if (_headers.find("Host") == _headers.end())
        throw std::runtime_error("ERROR: Missing Host Header");
    
    // FInd max Body size
    getMaxBodySize();
    
    if (_headers.find("Content-Length") != _headers.end())
    {
        long long contentLength;
        
        try{
            contentLength = Utils::ft_stolonglong(_headers["Content-Length"]);
        }
        catch (const std::exception &e){
            throw std::runtime_error("ERROR : Invalid Content-Length value"); 
        }
        if (contentLength < 0)
            throw std::runtime_error("ERROR: Invalid Content-Length value");
        _contentLength = contentLength; // stocker la taille du content length
    }

    if (_headers.find("Transfer-Encoding") != _headers.end()) 
    {
        std::string transferEncoding = _headers["Transfer-Encoding"];
        if (transferEncoding.find("chunked") == std::string::npos)
            throw std::runtime_error("ERROR: Unsupported Transfer-Encoding (must include 'chunked')");
        _isChunked = true; // Mettre le flag chunked
    }

    if (_headers.find("Content-Length") != _headers.end() &&
        _headers.find("Transfer-Encoding") != _headers.end())
        throw std::runtime_error("ERROR: Both Content-Length and Transfer-Encoding are present");

    // verfier aussi si il y en aucun des deux

    _state = BODY;
    LogManager::log(LogManager::DEBUG, "Checking headers DONE");
}


// RePASSER VERIFIER CETTE FONCTION SIUUUUUUU
void Request::getMaxBodySize()
{
    // Vérifier la présence de l'en-tête Host
    if (_headers.find("Host") == _headers.end())
        throw std::runtime_error("ERROR: Missing Host Header");

    // Extraire le nom d'hôte et le port depuis l'en-tête Host
    std::string hostValue = _headers["Host"];
    size_t colonPos = hostValue.find(':');
    std::string hostName = hostValue.substr(0, colonPos);
    int port = (colonPos != std::string::npos) ? Utils::ft_stoi(hostValue.substr(colonPos + 1)) : 80;

    // Conversion du port en chaîne (C++98 compatible)
    std::ostringstream oss;
    oss << port;
    std::string portStr = oss.str();
    LogManager::log(LogManager::DEBUG, ("Extracted Host: " + hostName + ", Port: " + portStr).c_str());

    // Accéder à la configuration via le getter
    Config config = _server->get_config();

    // Trouver le bloc server correspondant
    BlocServer* matchingServer = NULL; // Utiliser NULL au lieu de nullptr
    for (size_t i = 0; i < config.getServers().size(); ++i) {
        BlocServer& server = config.getServer(i);

        // Vérifier si le port correspond
        bool portMatches = false;
        for (size_t j = 0; j < server.getListen().size(); ++j) {
            if (server.getListen()[j].getPort() == port) {
                portMatches = true;
                break;
            }
        }

        // Vérifier si le server_name correspond
        bool serverNameMatches = false;
        for (size_t j = 0; j < server.getServerName().size(); ++j) {
            if (server.getServerName()[j] == hostName) {
                serverNameMatches = true;
                break;
            }
        }

        // Si le port et le server_name correspondent, sélectionner ce bloc
        if (portMatches && serverNameMatches) {
            matchingServer = &server;
            break;
        }

        // Si seul le port correspond, garder ce bloc comme fallback
        if (portMatches && !matchingServer) {
            matchingServer = &server;
        }
    }

    // Si aucun bloc server ne correspond, lever une erreur
    if (!matchingServer) {
        throw std::runtime_error("ERROR: No matching server block found for the request");
    }

    // Récupérer client_max_body_size
    _maxBodySize = matchingServer->getClientMaxBodySize();

    // Conversion de _maxBodySize en chaîne (C++98 compatible)
    oss.str(""); // Réinitialiser le flux
    oss.clear(); // Réinitialiser les flags d'erreur
    oss << _maxBodySize;
    std::string maxBodySizeStr = oss.str();
    LogManager::log(LogManager::DEBUG, ("Max body size set to: " + maxBodySizeStr).c_str());
}

// BODYYYYYYYYYYYYY
void Request::parseBody()
{
    LogManager::log(LogManager::DEBUG, "Parse Body");
    if (_method == "GET")
    {
        _state = END;
        return;
    }
    
    if (_contentLength > _maxBodySize)
        throw std::runtime_error("ERROR: Request body exceeds the maximum allowed size");

    if (!_body)
        _body = new RequestBody(_maxBodySize, _isChunked);
        
    try{
        
        if (_isChunked)
        {
            LogManager::log(LogManager::DEBUG, "Parse Body en mode CHUNKED");
            _body->parseChunked(_raw, _i);
            // GERER EN MODE CHUNMKER    
        }
        else
        {
            LogManager::log(LogManager::DEBUG, "Parse Body en mode LENGTH");
            _body->parseContentLength(_raw, _i, _contentLength);
        }
        if (_body->isComplete())
        {
            _state = END;
            LogManager::log(LogManager::DEBUG, "Parse body DONE!");
            std::cout << _body->getBody() << std::endl;
        }
        
    } catch (const std::exception &e) {
        LogManager::log(LogManager::ERROR, e.what());
        throw; // Relancer l'exception pour traitement en amont
    }
    
}


/**
 * Supprime les données déjà traitées de _raw et réinitialise l'index
 * @param processed_bytes Nombre d'octets déjà traités
 */
void Request::clearProcessedData(size_t processed_bytes)
{
    if (processed_bytes > 0 && processed_bytes <= _raw.size())
    {
        _raw.erase(0, processed_bytes);
        _i = 0; // Réinitialiser l'index
        LogManager::log(LogManager::DEBUG, "Cleared processed data, new raw size: %zu", _raw.size());
    }
}


