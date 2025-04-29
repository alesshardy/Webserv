#include "Utils.hpp"

int ft_stoi(const std::string &input)
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

unsigned long long ft_stoll(const std::string &input)
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

std::string toString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

bool    directoryExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}
bool    fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}


