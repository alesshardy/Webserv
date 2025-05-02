#include "Response.hpp"


std::string extractFilePath(const std::string &request);
std::string getContentType(const std::string &filePath);

Response::Response(Client* client)
{
    _client_fd = client->getClientFd();
    _request = client->getRequest();
    _r_state = R_INIT;
    _response_body = "";
    _response_header = "";
    _response_status = "";
    _response_code = "";
    _server = client->getServer();
    _timeOut = NULL;

    
}
Response::~Response()
{
    // Destructor logic if needed
}

int Response::buildResponse(int epoll_fd)
{
    (void)epoll_fd; // Suppress unused parameter warning SIUU
    
    if (_request->getState() == ERROR)
    {
        LogManager::log(LogManager::DEBUG, "Request in ERROR stat,  building response");
        handleError(_request->getStatusCode());
        return 0;
    }

    if (_r_state != R_CHUNK)
        setRState(R_PROCESSING);

    if (_request->_isCgi)
    {
        LogManager::log(LogManager::DEBUG, "Request is CGI, processing CGI response");
        _handleCgi();
        return 0;
    }
    
    if (_isRedirect())
    {
        LogManager::log(LogManager::DEBUG, "Redirecting request");
        setRState(R_END);
        return 0;
    }


    // Build the response based on the request method
    if (_request->getMethod() == "GET")
        _handleGet();
    else if (_request->getMethod() == "POST")
        _handlePost();
    else if (_request->getMethod() == "DELETE")
        _handleDelete();
    else
    {
        // SIUUU gerer plus tard le code 500
        LogManager::log(LogManager::ERROR, "Unsupported HTTP method: %s", _request->getMethod().c_str());
    }
    return 0;
}
void Response::setResponseHeader(const std::string& header)
{
    _response_header = header;
}
void Response::setResponseBody(const std::string& body)
{
    _response_body = body;
}
void Response::setResponseStatus(const std::string& status)
{
    _response_status = status;
}
void Response::setResponseCode(const std::string& code)
{
    _response_code = code;
}
void Response::setRequest(Request* request)
{
    _request = request;
}
void Response::setServer(Server* server)
{
    _server = server;
}
void Response::setClientFd(int client_fd)
{
    _client_fd = client_fd;
}
void Response::setClientSocket(Socket* client_socket)
{
    _client_socket = client_socket;
}


std::string Response::resolveFilePath()
{
    // Récupérer le chemin URI demandé
    std::string uri = _request->getUri();
    if (uri == "/")
        uri = ""; // Laisser vide pour ajouter l'index automatiquement

    // Récupérer le bloc serveur correspondant
    BlocServer* matchingServer = _request->getMatchingServer();
    if (!matchingServer)
        throw std::runtime_error("ERROR: No matching server found");

    // Vérifier si une location correspond à l'URI
    const BlocLocation* location = _request->getMatchingLocation();
    std::string rootOrAlias;
    std::vector<std::string> indexFiles;

    if (location)
    {
        LogManager::log(LogManager::DEBUG, "Matching location found: %s", location->getRoot().c_str());
        rootOrAlias = location->getAlias().empty() ? location->getRoot() : location->getAlias();
        indexFiles = location->getIndex();
    }
    else
    {
        LogManager::log(LogManager::DEBUG, "No matching location found, using server root: %s", matchingServer->getRoot().c_str());
        rootOrAlias = matchingServer->getRoot();
        indexFiles = matchingServer->getIndex();
    }

    // Construire le chemin complet
    std::string filePath = rootOrAlias + uri;

    // Si le chemin correspond à un répertoire, ajouter un fichier d'index
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
    {
        // Vérifier si la requête est une demande de listing
        if (_request->getHeader("list").first == "list" && _request->getHeader("list").second == "true")
        {
            LogManager::log(LogManager::DEBUG, "Skipping autoindex logic for directory listing request: %s", filePath.c_str());
            return filePath; // Retourner directement le chemin du répertoire
        }
        if (!filePath.empty() && filePath[filePath.size() - 1] != '/')
            filePath += "/";

        // Parcourir les fichiers d'index définis dans la configuration
        for (std::vector<std::string>::const_iterator it = indexFiles.begin(); it != indexFiles.end(); ++it)
        {
            std::string indexPath = filePath + *it;
            if (stat(indexPath.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode))
            {
                LogManager::log(LogManager::DEBUG, "Index file found: %s", indexPath.c_str());
                return indexPath;
            }
        }

        // Si aucun fichier d'index n'est trouvé
        if (location && location->getAutoIndex())
        {
            LogManager::log(LogManager::DEBUG, "Autoindex is enabled for directory: %s", filePath.c_str());
            return filePath; // Retourner le répertoire pour lister son contenu
        }
        else
        {
            
            LogManager::log(LogManager::ERROR, "No index file found and autoindex is disabled for directory: %s", filePath.c_str());
            BlocServer* matchingServer = _server->getMatchingServer(_request);
            _response = ErrorPage::getErrorPage(403, matchingServer->getErrorPage());
            setRState(R_END);
        }
    }

    // Retourner le chemin du fichier si ce n'est pas un répertoire
    return filePath;
}

std::string Response::generateDirectoryListing(const std::string& directoryPath, const std::string& uri, const std::string& format)
{
    DIR* dir = opendir(directoryPath.c_str());
    if (!dir)
        throw std::runtime_error("ERROR: Failed to open directory");

    std::ostringstream output;

    if (format == "html")
    {
        // Générer une réponse HTML
        output << "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " << uri << "</title>\n</head>\n<body>\n";
        output << "<h1>Index of " << uri << "</h1>\n<ul>\n";
    }
    else if (format == "json")
    {
        // Générer une réponse JSON
        output << "[";
    }
    else
    {
        throw std::invalid_argument("Unsupported format: " + format);
    }

    struct dirent* entry;
    bool first = true;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;

        std::string fullPath = directoryPath + "/" + name;
        struct stat fileStat;
        if (stat(fullPath.c_str(), &fileStat) == 0)
        {
            if (S_ISDIR(fileStat.st_mode))
                name += "/"; // Ajouter un slash pour les répertoires
        }

        if (format == "html")
        {
            output << "<li><a href=\"" << uri << "/" << name << "\">" << name << "</a></li>\n";
        }
        else if (format == "json")
        {
            if (!first)
                output << ",";
            first = false;
            output << "\"" << name << "\"";
        }
    }

    closedir(dir);

    if (format == "html")
    {
        output << "</ul>\n</body>\n</html>";
    }
    else if (format == "json")
    {
        output << "]";
    }

    return output.str();
}


// void Response::_handleGet()
// {
//     try
//     {
//         // Récupérer dynamiquement le chemin du fichier
//         std::string filePath = resolveFilePath();
//         LogManager::log(LogManager::DEBUG, "Resolved file path: %s", filePath.c_str());

//         // Vérifier si l'en-tête "list" est présent et a la valeur "true"
//         std::pair<std::string, std::string> listHeader = _request->getHeader("list");
//         if (listHeader.first == "list" && listHeader.second == "true")
//         {
//             LogManager::log(LogManager::DEBUG, "Directory listing requested via header for: %s", filePath.c_str());

//             // Vérifier si le chemin correspond à un répertoire
//             struct stat fileStat;
//             if (stat(filePath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
//             {
//                 // Générer un Directory Listing au format JSON
//                 std::string directoryListing = generateDirectoryListing(filePath, _request->getUri(), "json");

//                 // Construire la réponse HTTP
//                 _response = "HTTP/1.1 200 OK\r\n";
//                 _response += "Content-Type: application/json\r\n";
//                 _response += "Content-Length: " + toString(directoryListing.size()) + "\r\n";
//                 _response += "\r\n";
//                 _response += directoryListing;

//                 setRState(R_END);
//                 return;
//             }
//             else
//             {
//                 LogManager::log(LogManager::ERROR, "Requested path is not a directory: %s", filePath.c_str());
//                 _response = "HTTP/1.1 400 Bad Request\r\n\r\n";
//                 setRState(R_END);
//                 return;
//             }
//         }

//         // Vérifier si le chemin correspond à un répertoire
//         struct stat fileStat;
//         if (stat(filePath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
//         {
//             const BlocLocation* location = _request->getMatchingLocation();
//             if (location && location->getAutoIndex())
//             {
//                 LogManager::log(LogManager::DEBUG, "Autoindex is enabled, generating directory listing for: %s", filePath.c_str());
//                 std::string directoryListing = generateDirectoryListing(filePath, _request->getUri(), "html");
//                 _response = "HTTP/1.1 200 OK\r\n";
//                 _response += "Content-Type: text/html\r\n";
//                 _response += "Content-Length: " + toString(directoryListing.size()) + "\r\n";
//                 _response += "\r\n";
//                 _response += directoryListing;
//                 setRState(R_END);
//                 return;
//             }
//             else
//             {
//                 LogManager::log(LogManager::ERROR, "Access denied: Autoindex is disabled for directory: %s", filePath.c_str());
//                 BlocServer* matchingServer = _server->getMatchingServer(_request);
//                 _response = ErrorPage::getErrorPage(403, matchingServer->getErrorPage());
//                 setRState(R_END);
//                 return;
//             }
//         }

//         // Vérifier si le fichier existe et est accessible
//         if (stat(filePath.c_str(), &fileStat) != 0)
//         {
//             LogManager::log(LogManager::ERROR, "File not found: %s", filePath.c_str());
//             BlocServer* matchingServer = _server->getMatchingServer(_request);
//             _response = ErrorPage::getErrorPage(404, matchingServer->getErrorPage());
//             setRState(R_END);
//             return;
//         }

//         if (access(filePath.c_str(), R_OK) != 0)
//         {
//             LogManager::log(LogManager::ERROR, "Access denied: %s", filePath.c_str());
//             BlocServer* matchingServer = _server->getMatchingServer(_request);
//             _response = ErrorPage::getErrorPage(403, matchingServer->getErrorPage());
//             setRState(R_END);
//             return;
//         }

//         // Déterminer le type de contenu
//         std::string contentType = getContentType(filePath);

//         // Choisir entre réponse complète ou chunked
//         if (fileStat.st_size <= CHUNKED_THRESHOLD)
//         {
//             LogManager::log(LogManager::DEBUG, "File size is small enough for full response: %s", filePath.c_str());
//             _sendFullResponse(filePath, contentType);
//             setRState(R_END);
//         }
//         else
//         {
//             LogManager::log(LogManager::DEBUG, "File size is too large for full response, using chunked response: %s", filePath.c_str());
//             _sendChunkedResponse(filePath, contentType);
//             setRState(R_CHUNK); // Passer à l'état CHUNK
//         }
//     }
//     catch (const std::exception& e)
//     {
//         LogManager::log(LogManager::ERROR, "Error in _handleGet: %s", e.what());
//         _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//         setRState(R_END);
//     }
// }

void Response::_handleGet()
{
    try
    {
        // Récupérer dynamiquement le chemin du fichier
        std::string filePath = resolveFilePath();
        LogManager::log(LogManager::DEBUG, "Resolved file path: %s", filePath.c_str());

        // Vérifier si l'en-tête "list" est présent et a la valeur "true"
        std::pair<std::string, std::string> listHeader = _request->getHeader("list");
        if (listHeader.first == "list" && listHeader.second == "true")
        {
            LogManager::log(LogManager::DEBUG, "Directory listing requested via header for: %s", filePath.c_str());

            // Vérifier si le chemin correspond à un répertoire
            struct stat fileStat;
            if (stat(filePath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
            {
                // Générer un Directory Listing au format JSON
                std::string directoryListing = generateDirectoryListing(filePath, _request->getUri(), "json");

                // Construire la réponse HTTP
                _response = "HTTP/1.1 200 OK\r\n";
                _response += "Content-Type: application/json\r\n";
                _response += "Content-Length: " + toString(directoryListing.size()) + "\r\n";
                _response += "\r\n";
                _response += directoryListing;

                setRState(R_END);
                return;
            }
            else
            {
                LogManager::log(LogManager::ERROR, "Requested path is not a directory: %s", filePath.c_str());
                handleError(400, false); // Utiliser handleError pour une erreur 400
                return;
            }
        }

        // Vérifier si le chemin correspond à un répertoire
        struct stat fileStat;
        if (stat(filePath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
        {
            const BlocLocation* location = _request->getMatchingLocation();
            if (location && location->getAutoIndex())
            {
                LogManager::log(LogManager::DEBUG, "Autoindex is enabled, generating directory listing for: %s", filePath.c_str());
                std::string directoryListing = generateDirectoryListing(filePath, _request->getUri(), "html");
                _response = "HTTP/1.1 200 OK\r\n";
                _response += "Content-Type: text/html\r\n";
                _response += "Content-Length: " + toString(directoryListing.size()) + "\r\n";
                _response += "\r\n";
                _response += directoryListing;
                setRState(R_END);
                return;
            }
            else
            {
                LogManager::log(LogManager::ERROR, "Access denied: Autoindex is disabled for directory: %s", filePath.c_str());
                handleError(403, true); // Utiliser handleError pour une erreur 403 avec une page personnalisée
                return;
            }
        }

        // Vérifier si le fichier existe et est accessible
        if (stat(filePath.c_str(), &fileStat) != 0)
        {
            LogManager::log(LogManager::ERROR, "File not found: %s", filePath.c_str());
            handleError(404, true); // Utiliser handleError pour une erreur 404 avec une page personnalisée
            return;
        }

        if (access(filePath.c_str(), R_OK) != 0)
        {
            LogManager::log(LogManager::ERROR, "Access denied: %s", filePath.c_str());
            handleError(403, true); // Utiliser handleError pour une erreur 403 avec une page personnalisée
            return;
        }

        // Déterminer le type de contenu
        std::string contentType = getContentType(filePath);

        // Choisir entre réponse complète ou chunked
        if (fileStat.st_size <= CHUNKED_THRESHOLD)
        {
            LogManager::log(LogManager::DEBUG, "File size is small enough for full response: %s", filePath.c_str());
            _sendFullResponse(filePath, contentType);
            setRState(R_END);
        }
        else
        {
            LogManager::log(LogManager::DEBUG, "File size is too large for full response, using chunked response: %s", filePath.c_str());
            _sendChunkedResponse(filePath, contentType);
            setRState(R_CHUNK); // Passer à l'état CHUNK
        }
    }
    catch (const std::exception& e)
    {
        LogManager::log(LogManager::ERROR, "Error in _handleGet: %s", e.what());
        handleError(500, false); // Utiliser handleError pour une erreur 500
    }
}

bool Response::_isRedirect()
{
    // Vérifier si l'URI correspond à une redirection dans un bloc location
    const BlocLocation* location = _request->getMatchingLocation();
    if (location && !location->getReturnDirectives().empty())
    {
        const std::map<int, std::string>& returnDirectives = location->getReturnDirectives();
        if (!returnDirectives.empty())
        {
            int redirectCode = returnDirectives.begin()->first;
            const std::string& redirectUrl = returnDirectives.begin()->second;

            LogManager::log(LogManager::DEBUG, "Redirect found in location block: code=%d, url=%s", redirectCode, redirectUrl.c_str());

            // Construire la réponse HTTP pour la redirection
            _response = "HTTP/1.1 " + toString(redirectCode) + " Redirect\r\n";
            _response += "Location: " + redirectUrl + "\r\n\r\n";
            setRState(R_END);
            return true; // Retourner immédiatement après avoir trouvé une redirection
        }
    }

    // Vérifier si l'URI correspond à une redirection dans un bloc server
    BlocServer* server = _request->getMatchingServer();
    if (server && !server->getReturnDirectives().empty())
    {
        const std::map<int, std::string>& returnDirectives = server->getReturnDirectives();
        if (!returnDirectives.empty())
        {
            int redirectCode = returnDirectives.begin()->first;
            const std::string& redirectUrl = returnDirectives.begin()->second;

            LogManager::log(LogManager::DEBUG, "Redirect found in server block: code=%d, url=%s", redirectCode, redirectUrl.c_str());

            // Construire la réponse HTTP pour la redirection
            _response = "HTTP/1.1 " + toString(redirectCode) + " Redirect\r\n";
            _response += "Location: " + redirectUrl + "\r\n\r\n";
            setRState(R_END);
            return true;
        }
    }

    // Aucun cas de redirection trouvé
    return false;
}

// void Response::_handleCgi()
// {
//     try
//     {

//         if (!_request->getCgi())
//         {
//             LogManager::log(LogManager::ERROR, "No CGI request associated with this response");
//             _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//             setRState(R_END);
//             return;
//         }

//         std::string tmpFilePath = _request->getCgi()->getTmpFilePath();
//         // Open the temporary file containing the CGI output
//         std::ifstream cgiOutputFile(tmpFilePath.c_str(), std::ios::binary);
//         if (!cgiOutputFile.is_open())
//         {
//             LogManager::log(LogManager::ERROR, "Failed to open CGI output file: %s", _request->getCgi()->getTmpFilePath().c_str());
//             _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//             setRState(R_END);
//             return;
//         }

//         // Read the content of the CGI file
//         std::string cgiOutput((std::istreambuf_iterator<char>(cgiOutputFile)), std::istreambuf_iterator<char>());
//         cgiOutputFile.close();
        
//         LogManager::log(LogManager::DEBUG, "CGI Output:\n%s", cgiOutput.c_str());
//         // Separate headers and body of the CGI response
//         // Normalize line endings to \r\n
//         std::string normalizedCgiOutput;
//         for (size_t i = 0; i < cgiOutput.size(); ++i)
//         {
//             if (cgiOutput[i] == '\n' && (i == 0 || cgiOutput[i - 1] != '\r'))
//                 normalizedCgiOutput += "\r\n";
//             else
//                 normalizedCgiOutput += cgiOutput[i];
//         }

//         // Separate headers and body of the CGI response
//         size_t headerEnd = normalizedCgiOutput.find("\r\n\r\n");
//         if (headerEnd == std::string::npos)
//         {
//             LogManager::log(LogManager::ERROR, "Invalid CGI response format");
//             _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//             setRState(R_END);
//             return;
//         }

//         std::string cgiHeaders = cgiOutput.substr(0, headerEnd + 4);
//         std::string cgiBody = cgiOutput.substr(headerEnd + 4);

//         // Validate and fix CGI headers
//         std::istringstream headerStream(cgiHeaders);
//         std::ostringstream fixedHeaders;
//         std::string line;
//         bool hasContentType = false;

//         while (std::getline(headerStream, line))
//         {
//             if (!line.empty() && line[line.size() - 1] == '\r') // Check the last character
//                 line.erase(line.size() - 1); // Remove the last character

//             if (line.empty())
//                 continue;

//             if (line.find("Content-type:") == 0 || line.find("Content-Type:") == 0)
//                 hasContentType = true;

//             fixedHeaders << line << "\r\n";
//         }

//         if (!hasContentType)
//         {
//             LogManager::log(LogManager::WARNING, "CGI response missing Content-Type header, adding default");
//             fixedHeaders << "Content-Type: text/html\r\n";
//         }

//         fixedHeaders << "\r\n"; // Ensure headers end with a blank line

//         // Build the HTTP response
//         _response = "HTTP/1.1 200 OK\r\n" + fixedHeaders.str() + cgiBody;

//         LogManager::log(LogManager::DEBUG, "CGI response prepared for client %d", _client_fd);
//         setRState(R_END);
//     }
//     catch (const std::exception& e)
//     {
//         LogManager::log(LogManager::ERROR, "Error in _handleCgi: %s", e.what());
//         _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//         setRState(R_END);
//     }
// }

void Response::_handleCgi()
{
    try
    {
        // Vérifier si la requête est associée à un CGI
        if (!_request->getCgi())
        {
            LogManager::log(LogManager::ERROR, "No CGI request associated with this response");
            handleError(500, false); // Utiliser handleError pour une erreur 500
            return;
        }

        // Récupérer le chemin du fichier temporaire contenant la sortie CGI
        std::string tmpFilePath = _request->getCgi()->getTmpFilePath();
        std::ifstream cgiOutputFile(tmpFilePath.c_str(), std::ios::binary);
        if (!cgiOutputFile.is_open())
        {
            LogManager::log(LogManager::ERROR, "Failed to open CGI output file: %s", tmpFilePath.c_str());
            handleError(500, false); // Utiliser handleError pour une erreur 500
            return;
        }

        // Lire le contenu du fichier CGI
        std::string cgiOutput((std::istreambuf_iterator<char>(cgiOutputFile)), std::istreambuf_iterator<char>());
        cgiOutputFile.close();

        LogManager::log(LogManager::DEBUG, "CGI Output:\n%s", cgiOutput.c_str());

        // Normaliser les fins de ligne à "\r\n"
        std::string normalizedCgiOutput;
        for (size_t i = 0; i < cgiOutput.size(); ++i)
        {
            if (cgiOutput[i] == '\n' && (i == 0 || cgiOutput[i - 1] != '\r'))
                normalizedCgiOutput += "\r\n";
            else
                normalizedCgiOutput += cgiOutput[i];
        }

        // Séparer les en-têtes et le corps de la réponse CGI
        size_t headerEnd = normalizedCgiOutput.find("\r\n\r\n");
        if (headerEnd == std::string::npos)
        {
            LogManager::log(LogManager::ERROR, "Invalid CGI response format");
            handleError(500, false); // Utiliser handleError pour une erreur 500
            return;
        }

        std::string cgiHeaders = normalizedCgiOutput.substr(0, headerEnd + 4);
        std::string cgiBody = normalizedCgiOutput.substr(headerEnd + 4);

        // Valider et corriger les en-têtes CGI
        std::istringstream headerStream(cgiHeaders);
        std::ostringstream fixedHeaders;
        std::string line;
        bool hasContentType = false;

        while (std::getline(headerStream, line))
        {
            if (!line.empty() && line[line.size() - 1] == '\r') // Vérifier le dernier caractère
                line.erase(line.size() - 1); // Supprimer le dernier caractère

            if (line.empty())
                continue;

            if (line.find("Content-type:") == 0 || line.find("Content-Type:") == 0)
                hasContentType = true;

            fixedHeaders << line << "\r\n";
        }

        if (!hasContentType)
        {
            LogManager::log(LogManager::WARNING, "CGI response missing Content-Type header, adding default");
            fixedHeaders << "Content-Type: text/html\r\n";
        }

        fixedHeaders << "\r\n"; // S'assurer que les en-têtes se terminent par une ligne vide

        // Construire la réponse HTTP
        _response = "HTTP/1.1 200 OK\r\n" + fixedHeaders.str() + cgiBody;

        LogManager::log(LogManager::DEBUG, "CGI response prepared for client %d", _client_fd);
        setRState(R_END);
    }
    catch (const std::exception& e)
    {
        LogManager::log(LogManager::ERROR, "Error in _handleCgi: %s", e.what());
        handleError(500, false); // Utiliser handleError pour une erreur 500
    }
}

void Response::_sendChunkedResponse(const std::string& filePath, const std::string& contentType)
{
    std::ifstream file(filePath.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        LogManager::log(LogManager::ERROR, "Failed to open file for chunked response: %s", filePath.c_str());
        return;
    }

    _response_header = "HTTP/1.1 200 OK\r\n";
    _response_header += "Content-Type: " + contentType + "\r\n";
    _response_header += "Transfer-Encoding: chunked\r\n";
    _response_header += "\r\n";

    _response = _response_header;

    const size_t chunkSize = 8192; // Taille de chaque chunk
    char buffer[chunkSize];
    while (file.read(buffer, chunkSize) || file.gcount() > 0)
    {
        std::ostringstream chunk;
        chunk << std::hex << file.gcount() << "\r\n"; // Taille du chunk en hexadécimal
        chunk.write(buffer, file.gcount());
        chunk << "\r\n";
        _response += chunk.str();
    }

    _response += "0\r\n\r\n"; // Chunk final
    file.close();

    LogManager::log(LogManager::DEBUG, "Chunked response prepared for client %d: %s", _client_fd, filePath.c_str());
}

void Response::_sendFullResponse(const std::string& filePath, const std::string& contentType)
{
    // Lire le contenu du fichier
    std::ifstream file(filePath.c_str(), std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Construire les en-têtes de la réponse
    std::ostringstream oss;
    oss << content.size();
    _response_header = "HTTP/1.1 200 OK\r\n";
    _response_header += "Content-Type: " + contentType + "\r\n";
    _response_header += "Content-Length: " + oss.str() + "\r\n";
    _response_header += "\r\n";

    // Stocker le corps de la réponse
    _response_body = content;
    _response = _response_header + _response_body;

    LogManager::log(LogManager::DEBUG, "Full response prepared for client %d: %s", _client_fd, filePath.c_str());
}


// void Response::_handlePost()
// {
//     LogManager::log(LogManager::DEBUG, "Handling POST request");

//     if (isFileTransfer(_request->getMethod(), _request->getHeaders()))
//     {
//         LogManager::log(LogManager::DEBUG, "File transfer detected in POST request");

//         // Récupérer le nom du fichier téléversé
//         std::string fileName = extractFileNameFromMultipart(_request->_raw);
//         if (fileName.empty())
//         {
//             LogManager::log(LogManager::ERROR, "Filename header is missing");
//             _response = "HTTP/1.1 400 Bad Request\r\n\r\n";
//             setRState(R_END);
//             return;
//         }

//         // Construire une réponse HTML avec les détails du fichier
//         std::ostringstream responseBody;
//         responseBody << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n";
//         responseBody << "<meta charset=\"UTF-8\">\n<title>Fichier Téléversé</title>\n</head>\n<body>\n";
//         responseBody << "<h1>✅ Téléversement Réussi !</h1>\n";
//         responseBody << "<p>Votre fichier <strong>" << fileName << "</strong> a été téléversé avec succès.</p>\n";
//         responseBody << "<p><a href=\"/uploads/" << fileName << "\" target=\"_blank\">Télécharger ou visualiser le fichier</a></p>\n";
//         responseBody << "<p><a href=\"/filetransfert.html\">Téléverser un autre fichier</a></p>\n";
//         responseBody << "</body>\n</html>";

//         std::string body = responseBody.str();

//         // Construire les en-têtes de la réponse
//         std::ostringstream responseHeader;
//         responseHeader << "HTTP/1.1 200 OK\r\n";
//         responseHeader << "Content-Type: text/html\r\n";
//         responseHeader << "Content-Length: " << body.size() << "\r\n";
//         responseHeader << "\r\n";

//         // Combiner les en-têtes et le corps
//         _response = responseHeader.str() + body;

//         setRState(R_END);
//         return;
//     }

//     // Vérifier si le corps de la requête est complet
//     RequestBody* body = _request->getBody();
//     if (!body || !body->isComplete())
//     {
//         LogManager::log(LogManager::ERROR, "POST request body is incomplete or missing");
//         _response = "HTTP/1.1 400 Bad Request\r\n\r\n";
//         setRState(R_END);
//         return;
//     }

//     // Lire le corps de la requête (si nécessaire pour traitement)
//     try
//     {
//         std::string requestBody = body->readBody();
//         LogManager::log(LogManager::DEBUG, ("POST request body: " + requestBody).c_str());
//         // Traitez les données ici si nécessaire
//     }
//     catch (const std::exception& e)
//     {
//         LogManager::log(LogManager::ERROR, ("Failed to read POST request body: " + std::string(e.what())).c_str());
//         _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//         setRState(R_END);
//         return;
//     }

//     // Construire une réponse simple
//     _response = "HTTP/1.1 200 OK\r\n";
//     _response += "Content-Type: text/plain\r\n";
//     _response += "Content-Length: 19\r\n";
//     _response += "\r\n";
//     _response += "POST request received";

//     setRState(R_END);
//     LogManager::log(LogManager::DEBUG, "POST response prepared for client %d", _client_fd);
// }


void Response::_handlePost()
{
    LogManager::log(LogManager::DEBUG, "Handling POST request");

    // Vérifier si la requête est un transfert de fichier
    if (isFileTransfer(_request->getMethod(), _request->getHeaders()))
    {
        LogManager::log(LogManager::DEBUG, "File transfer detected in POST request");

        // Récupérer le nom du fichier téléversé
        std::string fileName = extractFileNameFromMultipart(_request->_raw);
        if (fileName.empty())
        {
            LogManager::log(LogManager::ERROR, "Filename header is missing");
            handleError(400, false); // Utiliser handleError pour une erreur 400
            return;
        }

        // Construire une réponse HTML avec les détails du fichier
        std::ostringstream responseBody;
        responseBody << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n";
        responseBody << "<meta charset=\"UTF-8\">\n<title>Fichier Téléversé</title>\n</head>\n<body>\n";
        responseBody << "<h1>✅ Téléversement Réussi !</h1>\n";
        responseBody << "<p>Votre fichier <strong>" << fileName << "</strong> a été téléversé avec succès.</p>\n";
        responseBody << "<p><a href=\"/uploads/" << fileName << "\" target=\"_blank\">Télécharger ou visualiser le fichier</a></p>\n";
        responseBody << "<p><a href=\"/filetransfert.html\">Téléverser un autre fichier</a></p>\n";
        responseBody << "</body>\n</html>";

        std::string body = responseBody.str();

        // Construire les en-têtes de la réponse
        std::ostringstream responseHeader;
        responseHeader << "HTTP/1.1 200 OK\r\n";
        responseHeader << "Content-Type: text/html\r\n";
        responseHeader << "Content-Length: " << body.size() << "\r\n";
        responseHeader << "\r\n";

        // Combiner les en-têtes et le corps
        _response = responseHeader.str() + body;

        setRState(R_END);
        return;
    }

    // Vérifier si le corps de la requête est complet
    RequestBody* body = _request->getBody();
    if (!body || !body->isComplete())
    {
        LogManager::log(LogManager::ERROR, "POST request body is incomplete or missing");
        handleError(400, false); // Utiliser handleError pour une erreur 400
        return;
    }

    // Lire le corps de la requête (si nécessaire pour traitement)
    try
    {
        std::string requestBody = body->readBody();
        LogManager::log(LogManager::DEBUG, ("POST request body: " + requestBody).c_str());
        // Traitez les données ici si nécessaire
    }
    catch (const std::exception& e)
    {
        LogManager::log(LogManager::ERROR, ("Failed to read POST request body: " + std::string(e.what())).c_str());
        handleError(500, false); // Utiliser handleError pour une erreur 500
        return;
    }

    // Construire une réponse simple
    _response = "HTTP/1.1 200 OK\r\n";
    _response += "Content-Type: text/plain\r\n";
    _response += "Content-Length: 19\r\n";
    _response += "\r\n";
    _response += "POST request received";

    setRState(R_END);
    LogManager::log(LogManager::DEBUG, "POST response prepared for client %d", _client_fd);
}
// void Response::_handleDelete()
// {
//     // Handle DELETE request
//     LogManager::log(LogManager::DEBUG, "Handling DELETE request");
    
//     std::string filePath = _request->getUri();
//     if (filePath == "/")
//         filePath = ""; // Laisser vide pour ajouter l'index automatiquement
//     // Récupérer le bloc serveur correspondant
//     BlocServer* matchingServer = _request->getMatchingServer();
//     if (!matchingServer)
//         throw std::runtime_error("ERROR: No matching server found");
//     // Vérifier si une location correspond à l'URI
//     const BlocLocation* location = _request->getMatchingLocation();
//     std::string rootOrAlias;
//     std::vector<std::string> indexFiles;
//     if (location)
//     {
//         LogManager::log(LogManager::DEBUG, "Matching location found: %s", location->getRoot().c_str());
//         rootOrAlias = location->getAlias().empty() ? location->getRoot() : location->getAlias();
//         indexFiles = location->getIndex();
//     }
//     else
//     {
//         LogManager::log(LogManager::DEBUG, "No matching location found, using server root: %s", matchingServer->getRoot().c_str());
//         rootOrAlias = matchingServer->getRoot();
//         indexFiles = matchingServer->getIndex();
//     }
//     // Construire le chemin complet
//     std::string filePathToDelete = rootOrAlias + filePath;
//     // Vérifier si le fichier existe
//     if (directoryExists(filePathToDelete))
//     {
//         // Supprimer le répertoire
//         if (rmdir(filePathToDelete.c_str()) == 0)
//         {
//             LogManager::log(LogManager::DEBUG, "Directory deleted successfully: %s", filePathToDelete.c_str());
//             _response = "HTTP/1.1 200 OK\r\n";
//             _response += "Content-Type: text/plain\r\n";
//             _response += "Content-Length: 17\r\n";
//             _response += "\r\n";
//             _response += "Directory deleted";
//         }
//         else
//         {
//             LogManager::log(LogManager::ERROR, "Failed to delete directory: %s", filePathToDelete.c_str());
//             _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//         }
//     }
//     else if (fileExists(filePathToDelete))
//     {
//         // Supprimer le fichier
//         if (remove(filePathToDelete.c_str()) == 0)
//         {
//             LogManager::log(LogManager::DEBUG, "File deleted successfully: %s", filePathToDelete.c_str());
//             _response = "HTTP/1.1 200 OK\r\n";
//             _response += "Content-Type: text/plain\r\n";
//             _response += "Content-Length: 12\r\n";
//             _response += "\r\n";
//             _response += "File deleted";
//         }
//         else
//         {
//             LogManager::log(LogManager::ERROR, "Failed to delete file: %s", filePathToDelete.c_str());
//             _response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//         }
//     }
//     else
//     {
//         LogManager::log(LogManager::ERROR, "File or directory not found: %s", filePathToDelete.c_str());
//         _response = "HTTP/1.1 404 Not Found\r\n\r\n";
//     }
//     setRState(R_END);
//     LogManager::log(LogManager::DEBUG, "DELETE response prepared for client %d", _client_fd);
// }

void Response::_handleDelete()
{
    LogManager::log(LogManager::DEBUG, "Handling DELETE request");

    std::string filePath = _request->getUri();
    if (filePath == "/")
        filePath = ""; // Laisser vide pour ajouter l'index automatiquement

    // Récupérer le bloc serveur correspondant
    BlocServer* matchingServer = _request->getMatchingServer();
    if (!matchingServer)
    {
        LogManager::log(LogManager::ERROR, "No matching server found");
        handleError(500, false); // Utiliser handleError pour une erreur 500
        return;
    }

    // Vérifier si une location correspond à l'URI
    const BlocLocation* location = _request->getMatchingLocation();
    std::string rootOrAlias;
    if (location)
    {
        LogManager::log(LogManager::DEBUG, "Matching location found: %s", location->getRoot().c_str());
        rootOrAlias = location->getAlias().empty() ? location->getRoot() : location->getAlias();
    }
    else
    {
        LogManager::log(LogManager::DEBUG, "No matching location found, using server root: %s", matchingServer->getRoot().c_str());
        rootOrAlias = matchingServer->getRoot();
    }

    // Construire le chemin complet
    std::string filePathToDelete = rootOrAlias + filePath;

    // Vérifier si le fichier ou le répertoire existe
    if (directoryExists(filePathToDelete))
    {
        // Supprimer le répertoire
        if (rmdir(filePathToDelete.c_str()) == 0)
        {
            LogManager::log(LogManager::DEBUG, "Directory deleted successfully: %s", filePathToDelete.c_str());
            _response = "HTTP/1.1 200 OK\r\n";
            _response += "Content-Type: text/plain\r\n";
            _response += "Content-Length: 17\r\n";
            _response += "\r\n";
            _response += "Directory deleted";
        }
        else
        {
            LogManager::log(LogManager::ERROR, "Failed to delete directory: %s", filePathToDelete.c_str());
            handleError(500, false); // Utiliser handleError pour une erreur 500
        }
    }
    else if (fileExists(filePathToDelete))
    {
        // Supprimer le fichier
        if (remove(filePathToDelete.c_str()) == 0)
        {
            LogManager::log(LogManager::DEBUG, "File deleted successfully: %s", filePathToDelete.c_str());
            _response = "HTTP/1.1 200 OK\r\n";
            _response += "Content-Type: text/plain\r\n";
            _response += "Content-Length: 12\r\n";
            _response += "\r\n";
            _response += "File deleted";
        }
        else
        {
            LogManager::log(LogManager::ERROR, "Failed to delete file: %s", filePathToDelete.c_str());
            handleError(500, false); // Utiliser handleError pour une erreur 500
        }
    }
    else
    {
        LogManager::log(LogManager::ERROR, "File or directory not found: %s", filePathToDelete.c_str());
        handleError(404, true); // Utiliser handleError pour une erreur 404 avec une page personnalisée
    }

    setRState(R_END);
    LogManager::log(LogManager::DEBUG, "DELETE response prepared for client %d", _client_fd);
}

void            Response::handleError(int error_code, bool errorPage)
{
    BlocServer* matchingServer = _request->getMatchingServer();
    if (matchingServer && errorPage)
    {
        _response = ErrorPage::getErrorPage(error_code, matchingServer->getErrorPage());
        setRState(R_END);
    }
    else
    {
        _response = "HTTP/1.1 " + toString(error_code) + " Error\r\n\r\n";
        setRState(R_END);
    }
    
}











/********************************/

std::string extractFilePath(const std::string &request)
{
    size_t start = request.find("GET ") + 4;
    size_t end = request.find(" ", start);
    if (start == std::string::npos || end == std::string::npos)
        return "";
    std::string path = request.substr(start, end - start);
    if (path == "/")
        path = "/index.html"; // Par défaut, servir index.html
    return "www/main" + path; // Préfixer avec le répertoire racine
}

bool endsWith(const std::string &str, const std::string &suffix)
{
    if (str.length() >= suffix.length())
    {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}


std::string getContentType(const std::string &filePath)
{
    if (endsWith(filePath, ".html"))
        return "text/html";
    if (endsWith(filePath, ".css"))
        return "text/css";
    if (endsWith(filePath, ".js"))
        return "application/javascript";
    if (endsWith(filePath, ".png"))
        return "image/png";
    if (endsWith(filePath, ".jpg") || endsWith(filePath, ".jpeg"))
        return "image/jpeg";
    if (endsWith(filePath, ".gif"))
        return "image/gif";
    if (endsWith(filePath, ".ico"))
        return "image/x-icon";
    if (endsWith(filePath, ".txt"))
        return "text/plain";
    if (endsWith(filePath, ".json"))
        return "application/json";
    if (endsWith(filePath, ".xml"))
        return "application/xml";
    if (endsWith(filePath, ".pdf"))
        return "application/pdf";
    if (endsWith(filePath, ".zip"))
        return "application/zip";
    if (endsWith(filePath, ".mp4"))
        return "video/mp4";
    if (endsWith(filePath, ".mp3"))
        return "audio/mpeg";
    if (endsWith(filePath, ".svg"))
        return "image/svg+xml";
    if (endsWith(filePath, ".woff"))
        return "font/woff";
    if (endsWith(filePath, ".woff2"))
        return "font/woff2";
    if (endsWith(filePath, ".ttf"))
        return "font/ttf";
    if (endsWith(filePath, ".otf"))
        return "font/otf";
    if (endsWith(filePath, ".eot"))
        return "font/eot";
    if (endsWith(filePath, ".webp"))
        return "image/webp";
    return "application/octet-stream"; // Type par défaut
}

bool Response::isTimeoutExceeded() const
{
    std::time_t now = std::time(NULL);
    return (now - _timeOut > 10);
}

