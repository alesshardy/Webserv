#ifndef CGIREQUEST_H
#define CGIREQUEST_H

#include "Request.hpp"

class Request;

class CgiRequest
{
    private:
            Request *_request;
            std::map <std::string, std::string> _env;
            char**      _envp;
            std::string _cgiPath;
            std::string _scriptCgi;
            char **args[3];
            pid_t _pid;
            int _stdin;
            int _stdout;

            void _initEnv();
            void _CgiConvertEnvToChar();
            void _printEnv();
            
    public:
            CgiRequest(Request *request, std::string cgiPath, std::string scriptCgi);
            CgiRequest(CgiRequest const & copy);
            ~CgiRequest();
            
            void executeCgi();
};

#endif