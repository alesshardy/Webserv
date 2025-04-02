#include "Utils.hpp"

int Utils::ft_stoi(const std::string &input)
{
    long long ret;
    std::istringstream iss(input);
    
    iss >> ret;
    if (iss.fail())
    {
        throw std::invalid_argument("stoi");
    }
    char extra;
    if (iss >> extra)
    {
        throw std::invalid_argument("stoi: extra characters found");
    }
    // Vérifier les limites de l'entier
    if (ret < std::numeric_limits<int>::min() || ret > std::numeric_limits<int>::max())
    {
      throw std::out_of_range("stoi: value out of range");
    }
    
    return static_cast<int>(ret);
}

unsigned long long Utils::ft_stolonglong(const std::string &input)
{
    unsigned long long ret;

    std::istringstream iss(input);
    
    iss >> ret;
    if (iss.fail())
    {
        throw std::invalid_argument("stoLONlong");
    }
    
    return ret;
}