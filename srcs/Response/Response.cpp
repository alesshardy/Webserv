#include "Response.hpp"


std::string extractFilePath(const std::string &request);
std::string getContentType(const std::string &filePath);

Response::Response(Client* client)
{
    _client_fd = client->getClientFd();
    _request = client->getRequest();
    _response_body = "";
    _response_header = "";
    _response_status = "";
    _response_code = "";
}
Response::~Response()
{
    // Destructor logic if needed
}

int Response::buildResponse(int epoll_fd)
{
    (void)epoll_fd; // Suppress unused parameter warning

    // Build the response based on the request method
    if (_request->getMethod() == "GET")
        _handleGet();
    else if (_request->getMethod() == "POST")
        _handlePost();
    else if (_request->getMethod() == "DELETE")
        _handleDelete();
    else if (_request->getMethod() == "PUT")
        _handlePut();
    else
        LogManager::log(LogManager::ERROR, "Unsupported HTTP method: %s", _request->getMethod().c_str());
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
void Response::setServer(BlocServer* server)
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
// Handle different HTTP methods
void Response::_handleGet()
{
    // Récupérer le chemin du fichier demandé
    std::string filePath = _request->getUri();
    if (filePath == "/")
        filePath = "/index.html"; // Par défaut, servir index.html
    
    filePath = "www/main" + filePath; // Préfixer avec le répertoire racine
    LogManager::log(LogManager::DEBUG, "File path: %s", filePath.c_str());

     // Vérifier si le chemin correspond à un répertoire
     struct stat fileStat;
     if (stat(filePath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
     {
         // Si c'est un répertoire, ajouter index.html
         if (!filePath.empty() && filePath[filePath.size() - 1] != '/')
            filePath += "/";
         filePath += "index.html";
         LogManager::log(LogManager::DEBUG, "Path is a directory, appending index.html: %s", filePath.c_str());
     }

    // Ouvrir le fichier demandé
    std::ifstream file(filePath.c_str(), std::ios::binary);
    if (file.is_open())
    {
        LogManager::log(LogManager::DEBUG, "File opened successfully: %s", filePath.c_str());

        // Lire le contenu du fichier
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Déterminer le type de contenu
        std::string contentType = getContentType(filePath);

        // Construire les en-têtes de la réponse
        std::ostringstream oss;
        oss << content.size();
        _response_header = "HTTP/1.1 200 OK\r\n";
        _response_header += "Content-Type: " + contentType + "\r\n";
        _response_header += "Content-Length: " + oss.str() + "\r\n";
        _response_header += "\r\n";

        // Stocker le corps de la réponse
        _response_body = content;

        LogManager::log(LogManager::INFO, "Response prepared for client %d: %s", _client_fd, filePath.c_str());
    }
    else
    {
        // Si le fichier n'est pas trouvé, retourner une réponse 404
        _response_body = "<html><body><h1>404 Not Found</h1><p>File not found.</p></body></html>";
        std::ostringstream oss;
        oss << _response_body.size();
        _response_header = "HTTP/1.1 404 Not Found\r\n";
        _response_header += "Content-Type: text/html\r\n";
        _response_header += "Content-Length: " + oss.str() + "\r\n";
        _response_header += "\r\n";

        LogManager::log(LogManager::ERROR, "File not found: %s", filePath.c_str());
    }

}
void Response::_handlePost()
{
    // Handle POST request
    LogManager::log(LogManager::INFO, "Handling POST request");
    // Implement POST logic here
}
void Response::_handleDelete()
{
    // Handle DELETE request
    LogManager::log(LogManager::INFO, "Handling DELETE request");
    // Implement DELETE logic here
}
void Response::_handlePut()
{
    // Handle PUT request
    LogManager::log(LogManager::INFO, "Handling PUT request");
    // Implement PUT logic here
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
    return "application/octet-stream"; // Type par défaut
}
