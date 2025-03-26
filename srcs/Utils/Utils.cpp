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