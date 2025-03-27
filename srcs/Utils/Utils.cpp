#include "Utils.hpp"

int Utils::ft_stoi(const std::string &input)
{
    int ret;

    std::istringstream iss(input);
    
    iss >> ret;
    if (iss.fail())
    {
        throw std::invalid_argument("stoi");
    }
    
    return ret;
}

unsigned long long Utils::ft_stolonglong(const std::string &input)
{
    unsigned long long ret;

    std::istringstream iss(input);
    
    iss >> ret;
    if (iss.fail())
    {
        throw std::invalid_argument("stoi");
    }
    
    return ret;
}