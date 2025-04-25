#ifndef CGIREQUEST_H
#define CGIREQUEST_H

#include "Request.hpp"
#include "sys/wait.h"

class Request;

class CgiRequest
{
        friend class Response;
    private:
            Request *_request;
            std::map <std::string, std::string> _env;
            char**      _envp;
            char *_argv[3];
            std::string _cgiPath;
            std::string _scriptCgi;
            pid_t _pid;
            int _stdin;
            int _stdout;
            int _fd;
            std::string _tmpFilePath;

            void _initEnv();
            void _CgiConvertEnvToChar();
            void _printEnv();
            
    public:
            CgiRequest(Request *request, std::string cgiPath, std::string scriptCgi);
            CgiRequest(CgiRequest const & copy);
            ~CgiRequest();

            std::string getTmpFilePath() const {return _tmpFilePath;};
            
            void executeCgi();
            void checkEnd();
};

#endif