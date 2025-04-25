#include "CgiRequest.hpp"

CgiRequest::CgiRequest(Request *request, std::string cgiPath, std::string scriptCgi): _request(request), _cgiPath(cgiPath), _scriptCgi(scriptCgi)
{
    _initEnv();

    char tmpFileName[] = "/tmp/cgi_tmpfile_XXXXXX";
    _fd = mkstemp(tmpFileName); // fonction pour creer un fichier temporaire
    if (_fd == -1)
    {
        perror("mkstemp failed");
        throw std::runtime_error("ERROR: failed to create tmp file");
    }
    _tmpFilePath = tmpFileName;
}

CgiRequest::CgiRequest(CgiRequest const & copy)
{
    *this = copy;
}

CgiRequest::~CgiRequest()
{
    if (_envp)
    {
        for (size_t i = 0; _envp[i] != NULL; i++)
            free(_envp[i]);
        delete [] _envp;
    }
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
    _envp = new char*[_env.size() + 1];

    size_t i= 0;
    for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
    {
        std::string envStr = it->first + "=" + it->second;
        _envp[i] = strdup(envStr.c_str());
        i++;
    }

    _envp[i] = NULL;
    if (LogManager::getDebugLogStatus())
    {
        // Debug : Afficher les variables d'environnement converties
        for (size_t j = 0; j < i; ++j)
            std::cout << _envp[j] << std::endl;
    }
}
void CgiRequest::executeCgi()
{
    if (LogManager::getDebugLogStatus())
        _printEnv();

    _CgiConvertEnvToChar();

    // Préparer les arguments pour execve
    _argv[0] = const_cast<char*>(_cgiPath.c_str());
    _argv[1] = const_cast<char*>(_scriptCgi.c_str());
    _argv[2] = NULL;
    
    _stdin = dup(STDIN_FILENO);
    _stdout = dup(STDOUT_FILENO);

    
    std::cout << "Jessaye daafficher le fd : "<<_request->_body->_fd << std::endl;
    LogManager::log(LogManager::DEBUG, ("Reset le body a 0 pour cgi"));
    // si body le reset au debut pour lecture complete necessaire pour les cgi
    if (_request->_body->_fd != -1)
        if (lseek(_request->_body->_fd, 0, SEEK_SET) == -1) // remet a 0 lecture
            throw std::runtime_error("reset lecture body failed");
    
    std::cout << "SIUUUUUUUU " << std::endl;

    LogManager::log(LogManager::DEBUG, ("Execution du cgi"));
    _pid = fork();
    if (_pid == -1)
        throw (std::runtime_error("Fork failed "));
    if (_pid == 0)
    {
        // 1 lire le body
        if (_request->_body->_fd != -1)
            if (dup2(_request->_body->_fd, STDIN_FILENO) == -1)
                throw (std::runtime_error("fail dans l'enfant lecture body"));
        if (dup2(_fd, STDOUT_FILENO) == -1)
            throw (std::runtime_error("fail dans l'enfant sortie"));
        execve(_argv[0], _argv, _envp);
        throw (std::runtime_error("fail childdd execve"));
    }
    else
    {
        // pas de wait ici mise en etat cgi en cours
        LogManager::log(LogManager::DEBUG, ("CGI PROCESS EN COURS"));
    }
}

void    CgiRequest::checkEnd()
{
    if (_request->getState() == END)
        return ;
    int status;
    pid_t wpid = waitpid(_pid, &status, WNOHANG);
    if (wpid == -1)
        throw (std::runtime_error("Error dans waitpid"));
    if (wpid == 0)
        return ; // encore en cours
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        LogManager::log(LogManager::DEBUG, ("cgi done ! State set on END"));
        _request->setState(END);
    }
    else
    {
        LogManager::log(LogManager::DEBUG, ("cgi script crash"));
        _request->handleError(502, ERROR, "cgi script crash");
    }
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