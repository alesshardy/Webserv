#include "Response.hpp"

Response::Response(int client_fd, Socket* client_socket, Request* request, BlocServer* server)
    : _client_fd(client_fd), _client_socket(client_socket), _request(request), _server(server)
{
    // Initialize response components
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
    // Build the response based on the request method
    if (_request->getMethod() == "GET")
        _handleGet();
    else if (_request->getMethod() == "POST")
        _handlePost();
    else if (_request->getMethod() == "DELETE")
        _handleDelete();
    else if (_request->getMethod() == "PUT")
        _handlePut();
    else if (_request->getMethod() == "HEAD")
        _handleHead();
    else
        LogManager::log(LogManager::ERROR, "Unsupported HTTP method: %s", _request->getMethod().c_str());
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
    // Handle GET request
    LogManager::log(LogManager::INFO, "Handling GET request");
    // Implement GET logic here

    
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
