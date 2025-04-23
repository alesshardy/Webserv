#ifndef CGIREQUEST_H
#define CGIREQUEST_H

#include "Request.hpp"

class Request;

class CgiRequest
{
    private:
            Request *_request;
            std::map <std::string, std::string> _env;
           
            void _initEnv();
            void _executeCgi();

    public:
            CgiRequest(Request *request);
            CgiRequest(CgiRequest const & copy);
            ~CgiRequest();

};

#endif