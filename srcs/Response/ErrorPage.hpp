#ifndef ERRORPAGE_HPP
#define ERRORPAGE_HPP

#include <iostream>
#include <map>

#include "LogManager.hpp"
#include "Utils.hpp"

class   ErrorPage
{
    private:


    public:
        static std::string  getErrorPage(int error_code, const std::map<int, std::string>& error_pages);
        
    
};







#endif