#include "Request.hpp"

Request::Request(Client *client, Server *server): _client(client), _server(server), _body(NULL),_raw(""), _method(""), _uri(""), _version(""), _currentHeaderKey(""), _statusCode(200), _state(0), _inHeader(false), _i(0), _isChunked(false), _maxBodySize(DEFAULT_CLIENT_MAX_BODY_SIZE), _contentLength(0), _timeOut(std::time(NULL)), _isCgi(false), _queryString(""), _port(0), _cgi(NULL)
{
    setPort(_client->getClientSocket()->get_port());
}

Request::Request(Request const & copy)
{
    *this = copy;
}

Request &Request::operator=(Request const & rhs)
{
    if (this != &rhs)
    {
        this->_client = rhs._client;
        this->_server = rhs._server;
        this->_raw = rhs._raw;
        this->_method = rhs._method;
        this->_uri = rhs._uri;
        this->_version = rhs._version;
        this->_query = rhs._query;
        this->_headers = rhs._headers;
        this->_currentHeaderKey = rhs._currentHeaderKey;
        this->_statusCode = rhs._statusCode;
        this->_state = rhs._state;
        this->_inHeader = rhs._inHeader;
        this->_i = rhs._i;
        this->_isChunked = rhs._isChunked;
        this->_maxBodySize = rhs._maxBodySize;
        this->_contentLength = rhs._contentLength;
        this->_timeOut = rhs._timeOut;
        this->_isCgi = rhs._isCgi;
        this->_queryString = rhs._queryString;
        this->_port = rhs._port;

        if (this->_body)
        {
            delete this->_body;
            this->_body = NULL;
        }
        if (rhs._body)
            this->_body = new RequestBody(*rhs._body);

        if (this->_cgi)
        {
            delete this->_cgi;
            this->_cgi = NULL;
        }
        if (rhs._cgi)
            this->_cgi = new CgiRequest(*rhs._cgi); 
    }
    return (*this);
}

Request::~Request()
{
    if (_body)
    {
        delete _body;
        _body = NULL;
    }
    if (_cgi)
    {
        delete _cgi;
        _cgi = NULL;
    }
}

void Request::handleError(int code, int state, const std::string& errorMessage)
{
    setCode(code);      
    setState(state);     
    // LogManager::log(LogManager::ERROR, errorMessage.c_str()); 
    throw std::runtime_error(errorMessage);
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
        if (_state == QUERY)
            parseQuery();
        if (_state == VERSION) 
            parseVersion();
        // parse header qui boucle entre key et value jusqu'au body
        if (_state == HEADER_KEY || _state == HEADER_VALUE)
            parseHeader();
        if (_state == HEADER_CHECK)
            checkHeader();
        if (_state == BODY)
            parseBody();
        if (_state == CGI)
            parseCgi();
        if  (_state == END)
                return ;
    }
    catch (const std::runtime_error &e)
    {
        setState(ERROR);
        throw; // Rethrow the exception if needed
    }
}

/**
 * @brief Parse la méthode HTTP.
 * 
 */
void Request::parseMethod()
{
    // LogManager::log(LogManager::DEBUG, "Parse method");
    if (_raw.empty())
       return (handleError(400, ERROR, "ERROR : empty request"));

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        return ;
    LogManager::log(LogManager::INFO, "%.*s", 25, _raw.substr(0, _raw.find("\n")).c_str());
    while (_i < endLine && _raw[_i] != ' ')
    {
        _method += _raw[_i];
        _i++;
    }

    LogManager::log(LogManager::DEBUG, ("Http method: " + _method).c_str());

    if (_method != "GET" && _method != "POST" && _method != "DELETE" && _method != "PUT")
        return (handleError(501, ERROR, "ERROR : unsupported HTTP method"));

    while (_i < endLine && _raw[_i] == ' ')
        _i++;

    if (_i >= endLine)
        return (handleError(400, ERROR, "ERROR: Missing URI after method"));
    _raw.erase(0, _i);
    _i = 0;
    setState(URI);
}

/**
 * @brief Parse l'URI de la requête HTTP.
 * 
 */
void Request::parseUri()
{
    if (_raw.empty())
        return ;
    // LogManager::log(LogManager::DEBUG, "Parse uri");
    while (_i < _raw.size() && (_raw[_i] == ' ' || _raw[_i] == '\t'))
        _i++;

    if (_i >= _raw.size() || _raw[_i] != '/')
        return (handleError(400, ERROR, "ERROR: Uri unknown format"));

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        return (handleError(400, ERROR, "ERROR : invalid request format"));

    while (_i < endLine && _raw[_i] != ' ')
    {
        _uri += _raw[_i];
        _i++;
    }

    while (_i < endLine && _raw[_i] == ' ')
        _i++;

    if (_i >= endLine)
        return (handleError(400, ERROR, "ERROR: Missing Version after uri"));
    
    LogManager::log(LogManager::DEBUG, ("Http uri: " + _uri).c_str());
    if (_uri.size() > URI_MAX_SIZE)
        return (handleError(414, ERROR, "ERROR: URI size exceeds 2048 characters"));
    _raw.erase(0, _i);
    _i = 0;
    setState(QUERY);
}

/**
 * @brief Parse le Query de la requête HTTP.
 * 
 */
void Request::parseQuery()
{
    size_t queryStart = _uri.find('?');
    if (queryStart == std::string::npos)
        return (setState(VERSION));

    _queryString = _uri.substr(queryStart + 1);
    _uri.erase(queryStart);

    std::string key, value;
    size_t pos = 0;
    size_t length = _queryString.length();

    while (pos < length)
    {
        // Trouve la prochaine '=' ou '&'
        size_t equalPos = _queryString.find('=', pos);
        size_t ampersandPos = _queryString.find('&', pos);

        // Cas 1: Pas de '=' avant '&' (clé sans valeur)
        if (equalPos == std::string::npos || (ampersandPos != std::string::npos && ampersandPos < equalPos))
        {
            key = _queryString.substr(pos, ampersandPos - pos);
            value.clear(); // Pas de valeur
            pos = (ampersandPos == std::string::npos) ? length : ampersandPos + 1;
        }
        // Cas 2: '=' trouvé avant '&' (clé + valeur)
        else
        {
            key = _queryString.substr(pos, equalPos - pos);
            value = _queryString.substr(equalPos + 1, 
                (ampersandPos == std::string::npos) ? length - equalPos - 1 : ampersandPos - equalPos - 1);
            pos = (ampersandPos == std::string::npos) ? length : ampersandPos + 1;
        }
        if (!key.empty())
            _query[key] = value;
    }

    LogManager::log(LogManager::DEBUG, "Parsed query parameters:");
    for (std::map<std::string, std::string>::const_iterator it = _query.begin(); it != _query.end(); ++it)
    {
        for (std::map<std::string, std::string>::iterator it = _query.begin(); it != _query.end(); ++it)
            LogManager::log(LogManager::DEBUG, ("Key: " + it->first + ", Value: " + it->second).c_str());
    }
    setState(VERSION);
}

/**
 * @brief Parse la version de la requête HTTP.
 * 
 */
void Request::parseVersion()
{   
    if (_raw.empty())
        return ;
        
    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
        return (handleError(400, ERROR, "ERROR : invalid request format"));

    _version = _raw.substr(_i, endLine - _i);

    _i = endLine + 2; 
    if (_version != "HTTP/1.1")
        return (handleError(505, ERROR, "ERROR: Bad Version"));
    LogManager::log(LogManager::DEBUG, ("Http version: " + _version).c_str());
    if (_raw.size() > 8192)
        return (handleError(400, ERROR, "ERROR: Request line exceeds 8 KB"));
    _raw.erase(0, _i);
    _i = 0;
    setState(HEADER_KEY);
}
/**
 * @brief Parse le corps de la requête HTTP.
 * 
 */
void Request::parseHeaderKey()
{
    _inHeader = false;
    if (_raw.empty())
        return ;
    // LogManager::log(LogManager::DEBUG, "Parse HeaderKey");

    if (_raw[_i] && _raw[_i] == '\r' && _raw[_i + 1] == '\n')
        return (setState(HEADER_CHECK));

    size_t colonPos = _raw.find(":", _i);
    if (colonPos == std::string::npos)
    {
        _inHeader = true;
        return ;    
    }

    while (_i < colonPos)
    {
        if (iswspace(_raw[_i]))
            return (handleError(400, ERROR, "ERROR: Bad request"));
        _currentHeaderKey += _raw[_i];
        _i++;
    }
    _i += 2;
    _raw.erase(0, _i);
    _i = 0;
    LogManager::log(LogManager::DEBUG, ("HeaderKey " + _currentHeaderKey).c_str());
    setState(HEADER_VALUE);
}

/**
 * @brief  Parse la valeur de l'en-tête de la requête HTTP.
 * 
 */
void Request::parseHeaderValue()
{
    bool checkString = false;
    if (_raw.empty())
        return ;
    // LogManager::log(LogManager::DEBUG, "Parse HeaderValue");

    size_t endLine = _raw.find("\r\n", _i);
    if (endLine == std::string::npos)
    {
        _inHeader = true;
        return ;    
    }
    std::string _currentHeaderValue;
    while (_i < endLine)
    {
        if (!iswspace(_raw[_i]))
            checkString = true;
        _currentHeaderValue += _raw[_i];
        _i++;
    }
    if (!checkString)
        return (handleError(400, ERROR, "ERROR: Bad request"));
    _i += 2;
    parseHeaderKeyValue(_currentHeaderKey, _currentHeaderValue);
    LogManager::log(LogManager::DEBUG, ("HeaderValue " + _currentHeaderValue).c_str());
    _currentHeaderKey.clear();
    _raw.erase(0, _i);
    _i = 0;
    setState(HEADER_KEY);
}

/**
 * @brief Parse les en-têtes de la requête HTTP.
 * 
 */
void   Request::parseHeader()
{
    while (_state >= HEADER_KEY && _state <= HEADER_VALUE)
    {
        if (_state == HEADER_KEY)
            parseHeaderKey();
        if (_state == HEADER_VALUE)
            parseHeaderValue();
        if (_inHeader || _raw.empty())
        {
            LogManager::log(LogManager::DEBUG, "Exiting header parsing loop: incomplete buffer or empty data");
            break ;       
        }
    }
}

void Request::parseHeaderKeyValue(const std::string& headerKey, const std::string& headerValue)
{
    // Vérifier si l'en-tête est unique
    if (headerKey == "Host" || headerKey == "Content-Length" || headerKey == "Transfer-Encoding")
    {
        if (_headers.find(headerKey) != _headers.end())
            return (handleError(400, ERROR, "ERROR: Duplicate header: " + headerKey));
        _headers[headerKey] = headerValue;
    }

    // Gérer les en-têtes multiples autorisés
    else if (headerKey == "Accept" || headerKey == "Accept-Encoding" || headerKey == "Cookie")
    {
        if (_headers.find(headerKey) != _headers.end())
            _headers[headerKey] += ", " + headerValue;
        else
            _headers[headerKey] = headerValue;
    }
    else
        _headers[headerKey] = headerValue;
}

/******************************* CHEcK AVANT BODY/CGI *****************************/

/**
 * @brief Vérifie les en-têtes de la requête HTTP.
 * 
 */
void Request::checkHeader()
{
    LogManager::log(LogManager::DEBUG, "Checking headers ...");

    checkHostHeader();
    findMatchingServerAndLocation();
    checkCgi();
    getMaxBodySize();
    validateContentLengthAndEncoding();
    skipHeaderEndSequence();
    checkAllowedMethods();

    if (_isCgi && _contentLength == 0)
        setState(CGI); // Passer directement à l'exécution du CGI
    else
        setState(BODY);

    _timeOut = std::time(NULL); // Reset timer pour le body
    LogManager::log(LogManager::DEBUG, "Checking headers DONE");
}

/**
 * @brief Vérifie si l'en-tête "Host" est présent dans la requête.
 * 
 */
void Request::checkHostHeader()
{
    if (_headers.find("Host") == _headers.end())
        throw std::runtime_error("ERROR: Missing Host Header");
}

/**
 * @brief Trouve le serveur et la localisation correspondants à la requête.
 * 
 */
void Request::findMatchingServerAndLocation()
{
    _matchingServer = _server->getMatchingServer(this);
    if (!_matchingServer)
        throw std::runtime_error("ERROR: No matching server block found for the request");

    _matchingLocation = _matchingServer->getMatchingLocation(_uri);
}

/**
 * @brief Vérifie si la requête cible un script CGI.
 * 
 */
void Request::checkCgi()
{
    if (_matchingLocation && _matchingLocation->getCgiExtensions().count(getUriExtension()))
    {
        _isCgi = true; // Marquer la requête comme CGI
        LogManager::log(LogManager::DEBUG, "Request targets a CGI script");
    }
    else
    {
        _isCgi = false; // Marquer la requête comme non CGI
        LogManager::log(LogManager::DEBUG, "Request does not target a CGI script");
    }
}

/**
 * @brief Vérifie la validité de la longueur et de l'encodage du contenu.
 * 
 */
// void Request::validateContentLengthAndEncoding()
// {
//     if (_headers.find("Content-Length") != _headers.end())
//     {
//         for (size_t i = 0; i < _headers["Content-Length"].length(); i++)
//         {
//             if (!isdigit(_headers["Content-Length"][i]))
//                 handleError(400, ERROR, "ERROR: Bad request");
//         }
//         long long contentLength;
//         try
//         {
//             contentLength = ft_stoll(_headers["Content-Length"]);
//         }
//         catch (const std::exception &e)
//         {
//             handleERROR()
//             throw std::runtime_error("ERROR: Invalid Content-Length value");
//         }
//         if (contentLength < 0)
//             throw std::runtime_error("ERROR: Invalid Content-Length value");
//         _contentLength = contentLength; // Stocker la taille du content length
//     }

//     if (_headers.find("Transfer-Encoding") != _headers.end())
//     {
//         std::string transferEncoding = _headers["Transfer-Encoding"];
//         if (transferEncoding.find("chunked") == std::string::npos)
//             throw std::runtime_error("ERROR: Unsupported Transfer-Encoding (must include 'chunked')");
//         _isChunked = true; // Mettre le flag chunked
//     }

//     if (_headers.find("Content-Length") != _headers.end() &&
//         _headers.find("Transfer-Encoding") != _headers.end())
//         throw std::runtime_error("ERROR: Both Content-Length and Transfer-Encoding are present");
// }

void Request::validateContentLengthAndEncoding()
{
    if (_headers.find("Content-Length") != _headers.end())
    {
        for (size_t i = 0; i < _headers["Content-Length"].length(); i++)
        {
            if (!isdigit(_headers["Content-Length"][i]))
                return (handleError(400, ERROR, "Bad request: Content-Length must be a positive integer"));
        }
        long long contentLength;
        try
        {
            contentLength = ft_stoll(_headers["Content-Length"]);
        }
        catch (const std::exception &e)
        {
            return (handleError(400, ERROR, "Invalid Content-Length value: conversion failed"));
        }

        if (contentLength < 0)
            return (handleError(400, ERROR, "Invalid Content-Length: negative value"));
        _contentLength = contentLength;
    }

    // Validation de Transfer-Encoding
    if (_headers.find("Transfer-Encoding") != _headers.end())
    {
        std::string transferEncoding = _headers["Transfer-Encoding"];
        if (transferEncoding.find("chunked") == std::string::npos)
            return (handleError(400, ERROR, "Unsupported Transfer-Encoding (must include 'chunked')"));
        _isChunked = true;
    }

    if (_headers.find("Content-Length") != _headers.end() &&
        _headers.find("Transfer-Encoding") != _headers.end())
            return (handleError(400, ERROR, "Both Content-Length and Transfer-Encoding are present"));
}

/**
 * @brief Ignore la séquence de fin d'en-tête "\r\n\r\n".
 * 
 */
void Request::skipHeaderEndSequence()
{
    if (_raw.substr(_i, 2) == "\r\n")
    {
        _i += 2;
        _raw.erase(0, _i);
        _i = 0;
    }
}

/**
 * @brief Vérifie les méthodes autorisées dans le bloc de localisation.
 * 
 */
void Request::checkAllowedMethods()
{
    if (_matchingLocation && !_matchingLocation->getAllowMethod().empty())
    {
        const std::set<std::string>& allowMethods = _matchingLocation->getAllowMethod();
        if (allowMethods.find(_method) == allowMethods.end())
            handleError(405, ERROR, "ERROR: Method [" + getMethod() + "] not allowed in this location");
        else
            LogManager::log(LogManager::DEBUG, "Good AllowMethod: [%s] found in bloc location", getMethod().c_str());
    }
}

/**
 * @brief Récupère l'URI de la requête HTTP.
 * 
 * @return std::string 
 */
std::string Request::getUriExtension() const
{
    // Trouver la position du dernier point dans l'URI
    size_t dotPos = _uri.find_last_of('.');

    // Si aucun point n'est trouvé ou si le point est à la fin de l'URI, retourner une chaîne vide
    if (dotPos == std::string::npos || dotPos == _uri.size() - 1)
        return "";

    // Retourner l'extension (à partir du dernier point jusqu'à la fin de l'URI)
    return _uri.substr(dotPos);
}

/**
 * @brief Récupère la taille maximale du corps de la requête à partir du bloc serveur correspondant.
 * 
 */
void Request::getMaxBodySize()
{
    // Récupérer client_max_body_size
    _maxBodySize = _matchingServer->getClientMaxBodySize();

    // Conversion de _maxBodySize en chaîne (C++98 compatible)
    std::ostringstream oss;
    oss.str(""); // Réinitialiser le flux
    oss.clear(); // Réinitialiser les flags d'erreur
    oss << _maxBodySize;
    std::string maxBodySizeStr = oss.str();
    LogManager::log(LogManager::DEBUG, ("Max body size set to: " + maxBodySizeStr).c_str());
}

/******************************* PARSING DU BODY *****************************/

/**
 * @brief Parse le corps de la requête HTTP.
 * 
 */
void Request::parseBody()
{   
    LogManager::log(LogManager::DEBUG, "Parse Body");
    if (_method == "GET")
    {
        if (isCgi())
            _state = CGI;
        else
            _state = END;
        return;
    }

    if (_headers.find("Content-Length") == _headers.end() && _headers.find("Transfer-Encoding") == _headers.end())
        return (handleError(400, ERROR, "ERROR: Bad Request, missing Content-Length or Transfer-Encoding"));
    
    if (_contentLength > _maxBodySize)
        throw std::runtime_error("ERROR: Request body exceeds the maximum allowed size");

    if (!_body)
        _body = new RequestBody(_maxBodySize, _isChunked);
        
    try{
        size_t startIndex = _i;
        
        if (_isChunked)
        {
            LogManager::log(LogManager::DEBUG, "Parse Body en mode CHUNKED");
            _body->parseChunked(_raw, _i);
        }
        else
        {
            LogManager::log(LogManager::DEBUG, "Parse Body en mode LENGTH");
            _body->parseContentLength(_raw, _i, _contentLength);
        }

        // Nettoyer les données traitées dans _raw
        clearProcessedData(_i - startIndex);
        
        if (_body->isComplete())
        {
            if (isCgi())
                _state = CGI;
            else
                _state = END;
            LogManager::log(LogManager::DEBUG, "Parse body DONE!");
            std::cout << "ReadtmpFIle :\n" << _body->readBody() << std::endl;
        }
        
    } catch (const std::exception &e) {
        _state = ERROR;
        throw; // Relancer l'exception pour traitement en amont
    }
}

/**
 * @brief Supprime les données traitées de la requête.
 * 
 * @param processedBytes 
 */
void Request::clearProcessedData(size_t processedBytes)
{
    if (processedBytes > 0 && processedBytes <= _raw.size())
    {
        // Log des données qui vont être supprimées
        LogManager::log(LogManager::DEBUG, ("Clearing processed data: \"" + _raw.substr(0, processedBytes) + "\"").c_str());
        _raw.erase(0, processedBytes);
        _i -= processedBytes; // Ajuster l'index pour refléter le nettoyage
    }
}

/************************************** CGI *******************************************/

void    Request::parseCgi()
{
    LogManager::log(LogManager::DEBUG, "Parse CGI");
    std::string cgiPath = _matchingLocation->getCgiPath(getUriExtension());
    std::cout << "CgiPAth = " << cgiPath << std::endl;
    std::string scriptCgi = getUri();
    std::cout << "scriptCgi = " << scriptCgi << std::endl;

    _cgi = new CgiRequest(this, cgiPath, scriptCgi);
    _timeOut = std::time(NULL); // Reset timer pour le Cgi
    _cgi->executeCgi();
}

/******************************************* TIMER *******************************************/

/**
 * @brief Vérifie si le délai d'attente est dépassé.
 * 
 * @return true 
 * @return false 
 */
bool Request::isTimeoutExceeded() const
{
    std::time_t now = std::time(NULL);
    return (now - _timeOut > 60);
}