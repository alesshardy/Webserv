#include "CgiRequest.hpp"

CgiRequest::CgiRequest(Request *request): _request(request){_initEnv();}

CgiRequest::CgiRequest(CgiRequest const & copy){*this = copy;}

CgiRequest::~CgiRequest(){};

void    CgiRequest::_initEnv()
{
    _env["SERVER_NAME"] = "Werbserv";
}