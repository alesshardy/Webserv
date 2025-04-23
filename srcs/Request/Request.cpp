#include "Request.hpp"

Request::Request(Client *client, Server *server): _client(client), _server(server), _body(),_raw(""), _method(""), _uri(""), _version(""), _currentHeaderKey(""), _statusCode(200), _state(0), _inHeader(false), _i(0), _isChunked(false), _maxBodySize(DEFAULT_CLIENT_MAX_BODY_SIZE), _contentLength(0), _timeOut(std::time(NULL)), _isCgi(false){}

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

        if (this->_body)
        {
            delete this->_body;
            this->_body = NULL;
        }
        if (rhs._body)
            this->_body = new RequestBody(*rhs._body); 
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
}

// // Getters & setters
// const std::string &Request::getMethod() const
// {
//     return (this->_method);
// }

// const std::string &Request::getUri() const
// {
//     return (this->_uri);
// }

// const std::string &Request::getVersion() const
// {
//     return (this->_version);
// }

// const int &Request::getStatusCode() const
// {
//     return (this->_statusCode);
// }

// const std::map<std::string, std::string> &Request::getHeaders() const
// {
//     return (this->_headers);
// }

// const std::map<std::string, std::string> &Request::getQuery() const
// {
//     return (this->_query);
// }

// const int &Request::getState() const 
// {
//     return (this->_state);
// }

// RequestBody *Request::getBody() const
// {
//     return (this->_body);
// }

// BlocServer *Request::getMatchingServer() const
// {
//     return (this->_matchingServer);
// }

// BlocLocation *Request::getMatchingLocation() const
// {
//     return (this->_matchingLocation);
// }

// void Request::setCode(int const & code)
// {
//     this->_statusCode = code;
// }

// void Request::setState(int const & state)
// {
//     this->_state = state;
// }

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
    while (_i < endLine && _raw[_i] != ' ')
    {
        _method += _raw[_i];
        _i++;
    }

    LogManager::log(LogManager::DEBUG, ("Http method: " + _method).c_str());

    if (_method != "GET" && _method != "POST" && _method != "DELETE" && _method != "PUT")
        return (handleError(400, ERROR, "ERROR : unsupported HTTP method"));

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
        return (handleError(400, ERROR, "ERROR: URI size exceeds 2048 characters"));
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

    std::string queryString = _uri.substr(queryStart + 1);
    _uri.erase(queryStart);

    std::string key, value;
    size_t pos = 0;
    size_t length = queryString.length();

    while (pos < length)
    {
        // Trouve la prochaine '=' ou '&'
        size_t equalPos = queryString.find('=', pos);
        size_t ampersandPos = queryString.find('&', pos);

        // Cas 1: Pas de '=' avant '&' (clé sans valeur)
        if (equalPos == std::string::npos || (ampersandPos != std::string::npos && ampersandPos < equalPos))
        {
            key = queryString.substr(pos, ampersandPos - pos);
            value.clear(); // Pas de valeur
            pos = (ampersandPos == std::string::npos) ? length : ampersandPos + 1;
        }
        // Cas 2: '=' trouvé avant '&' (clé + valeur)
        else
        {
            key = queryString.substr(pos, equalPos - pos);
            value = queryString.substr(equalPos + 1, 
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

    std::cout << "version: " << _version << std::endl;
    _i = endLine + 2; // Passer "\r\n"
    if (_version != "HTTP/1.1")
        return (handleError(400, ERROR, "ERROR: Bad Version"));
    LogManager::log(LogManager::DEBUG, ("Http version: " + _version).c_str());
    // SECU 
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
        _currentHeaderValue += _raw[_i];
        _i++;
    }
    _i += 2;
    // _headers[_currentHeaderKey] = _currentHeaderValue;
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


// CHECK HEADER / SIUUUUUUUUU decoupercette fonction propre 
void    Request::checkHeader()
{        
    LogManager::log(LogManager::DEBUG, "Checking headers ...");

    if (_headers.find("Host") == _headers.end())
        throw std::runtime_error("ERROR: Missing Host Header");
    
    //WAAAAA
    // Trouver le bloc serveur correspondant
    _matchingServer = _server->getMatchingServer(this);
    if (!_matchingServer)
        throw std::runtime_error("ERROR: No matching server block found for the request");

    // Vérifier si l'URI correspond à un CGI
    _matchingLocation = _matchingServer->getMatchingLocation(_uri);
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
    
    // FInd max Body size
    getMaxBodySize();
    
    if (_headers.find("Content-Length") != _headers.end())
    {
        long long contentLength;
        
        try{
            contentLength = ft_stoll(_headers["Content-Length"]);
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

    // Passer la séquence \r\n\r\n si elle n'a pas encore été sautée
    if (_raw.substr(_i, 2) == "\r\n")
    {
        _i += 2;
        _raw.erase(0, _i);
        _i = 0;
    }

    // CHECK ALLOW METHOD DANS LE BLOC LOCATION
    if (_matchingLocation && !_matchingLocation->getAllowMethod().empty())
    {
        const std::set<std::string>& allowMethods = _matchingLocation->getAllowMethod();
        if (allowMethods.find(_method) == allowMethods.end())
            throw std::runtime_error("ERROR: Method ["+ getMethod() + "] not allowed in this location");
        else
            LogManager::log(LogManager::DEBUG, "Good AllowMethod: [%s] find in bloc location ", getMethod().c_str());
    }

    //WAAAAAAAAA
    // Passer à l'état BODY ou CGI
    if (_isCgi && _contentLength == 0)
        setState(CGI); // Passer directement à l'exécution du CGI
    else
        setState(BODY);
    _timeOut = std::time(NULL); // Reset timmer pour le body
    LogManager::log(LogManager::DEBUG, "Checking headers DONE");
}

//WAAAAA
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
    // trouver le bloc serveur correspondant
    // BlocServer* matchingServer = _server->getMatchingServer(this);
    // // Si aucun bloc server ne correspond, lever une erreur
    // if (!matchingServer) 
    //     throw std::runtime_error("ERROR: No matching server block found for the request");

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
        _state = END;
        return;
    }
    
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

    //siuuu continuer ca mere jppp
    

    _state = END;
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
    return (now - _timeOut > 10); // SIUU CHANGER EN 60 sec
}