#include "CgiRequest.hpp"

CgiRequest::CgiRequest(Request *request, std::string cgiPath, std::string scriptCgi): _request(request), _cgiPath(cgiPath), _scriptCgi(scriptCgi)
{
    _initEnv();
}

CgiRequest::CgiRequest(CgiRequest const & copy)
{
    *this = copy;
}

CgiRequest::~CgiRequest()
{
}

void CgiRequest::_initEnv()
{  
    LogManager::log(LogManager::DEBUG, ("Init ENV pour CGI"));

    // Initialiser les variables d'environnement CGI
    _env["SERVER_NAME"] = "Webserv";
    _env["SERVER_SOFTWARE"] = "Webserv/1.0";
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["REQUEST_METHOD"] = _request->getMethod();
    _env["SCRIPT_NAME"] = _request->getUri();
    _env["QUERY_STRING"] = _request->getQueryString().empty() ? "" : _request->getQueryString();
    _env["CONTENT_TYPE"] = _request->getHeaders().count("Content-Type") ? _request->getHeaders().at("Content-Type") : "";
    _env["CONTENT_LENGTH"] = _request->getHeaders().count("Content-Length") ? _request->getHeaders().at("Content-Length") : "";
    _env["HTTP_HOST"] = _request->getHeaders().count("Host") ? _request->getHeaders().at("Host") : "";
    _env["SERVER_PORT"] = toString(_request->getPort());
    _env["PATH_INFO"] = _request->getUri();
    _env["PATH_TRANSLATED"] = _request->getMatchingLocation()->getRoot() + _request->getUri();
}

void    CgiRequest::_CgiConvertEnvToChar()
{
    LogManager::log(LogManager::DEBUG, ("Convert Env to Char[]:"));
    char    **envp = new char*[_env.size() + 1];

    size_t i= 0;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
    {
        std::string envStr = it->first + "=" + it->second;
        envp[i] = strdup(envStr.c_str());
        i++;
    }

    envp[i] = NULL;
    if (LogManager::getDebugLogStatus())
    {
        // Debug : Afficher les variables d'environnement converties
        for (size_t j = 0; j < i; ++j)
            std::cout << envp[j] << std::endl;
    }
}
void    CgiRequest::executeCgi()
{
    if (LogManager::getDebugLogStatus())
        _printEnv();
        
    _CgiConvertEnvToChar();

}

// print env
void   CgiRequest::_printEnv()
{
    LogManager::log(LogManager::DEBUG, ("Print ENV:"));
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
    {
        std::cout << it->first << " = " << it->second << std::endl;
    }
}